#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include "AnomalyDetector.h"
using namespace std;

const int NUM_SERVOS = NUM_LEGS * SERVOS_PER_LEG;
const string JOINTS[SERVOS_PER_LEG] = {"coxa", "femur", "tibia"};

// tuning values for the mock model
const double OUTLIER_Z = 3.5;            // modified z-score cutoff (Iglewicz & Hoaglin)
const double MIN_SPREAD = 5.0;           // us; keeps tiny differences from looking like outliers
const double HEADROOM_WARNING = 0.60;    // trim using 60% of the room it has
const double HEADROOM_ALERT = 0.90;      // trim using 90% of the room it has
const double NARROW_RANGE = 0.60;        // pulse span under 60% of similar joints

// tripod gait: these three legs carry the robot while the other three swing
// leg indexes: 0 FL, 1 ML, 2 RL, 3 FR, 4 MR, 5 RR
const int TRIPOD_A[3] = {0, 4, 2};
const int TRIPOD_B[3] = {3, 1, 5};

// --- helpers ---

static double median(vector<double> values) {
  sort(values.begin(), values.end());
  size_t n = values.size();
  if (n == 0) {
    return 0.0;
  }
  if (n % 2 == 1) {
    return values[n / 2];
  }
  return (values[n / 2 - 1] + values[n / 2]) / 2.0;
}

// label like "Servo 4 (femur, Middle Left)"
static string servoLabel(Hexapod& robot, Servo * servo) {
  int id = servo->getId();
  return "Servo " + to_string(id) + " (" + servo->getName() + ", " + robot.getLegName(id / SERVOS_PER_LEG) + ")";
}

static string severityText(Severity s) {
  switch (s) {
    case Severity::ALERT:   return "ALERT";
    case Severity::WARNING: return "WARNING";
    default:                return "INFO";
  }
}

// MockAnomalyDetector

string MockAnomalyDetector::getName() const {
  return "Mock AI (offline statistical model)";
}

// compare each servo's trim with the other 5 servos of the same joint type.
// a servo horn put on one spline tooth off shows up as a trim far from its peers.
void MockAnomalyDetector::checkTrimOutliers(Hexapod& robot, vector<Finding>& findings) const {
  for (int joint = 0; joint < SERVOS_PER_LEG; joint++) {
    vector<Servo *> group;
    vector<double> trims;
    for (int leg = 0; leg < NUM_LEGS; leg++) {
      Servo * servo = robot.findServo(leg * SERVOS_PER_LEG + joint);
      if (servo != nullptr) {
        group.push_back(servo);
        trims.push_back(servo->getTrim());
      }
    }
    if (group.size() < 3) {
      continue; // not enough peers to compare
    }

    // median absolute deviation: not thrown off by the outlier itself like a mean would be
    double mid = median(trims);
    vector<double> deviations;
    for (double t : trims) {
      deviations.push_back(fabs(t - mid));
    }
    double spread = max(median(deviations), MIN_SPREAD);

    for (size_t i = 0; i < group.size(); i++) {
      double z = 0.6745 * (trims[i] - mid) / spread;
      if (fabs(z) > OUTLIER_Z) {
        ostringstream msg;
        msg << "Trim of " << group[i]->getTrim() << " us is unusual for a " << JOINTS[joint]
            << " (" << JOINTS[joint] << "s on this robot are typically near " << static_cast<int>(mid) << " us, score "
            << fixed;
        msg.precision(1);
        msg << fabs(z) << ").";
        findings.push_back({Severity::WARNING, servoLabel(robot, group[i]), msg.str(),
                            "Check the servo horn is on the correct spline tooth, re-center it, then re-trim."});
      }
    }
  }
}

// a trim using most of its available room means the joint has drifted far from center
void MockAnomalyDetector::checkTrimHeadroom(Hexapod& robot, vector<Finding>& findings) const {
  for (int id = 0; id < NUM_SERVOS; id++) {
    Servo * servo = robot.findServo(id);
    if (servo == nullptr || servo->getTrim() == 0) {
      continue;
    }
    int trim = servo->getTrim();
    int room = trim > 0 ? servo->getMaxTrim() : -servo->getMinTrim();
    if (room <= 0) {
      continue;
    }
    double used = static_cast<double>(abs(trim)) / room;
    if (used < HEADROOM_WARNING) {
      continue;
    }

    Severity level = used >= HEADROOM_ALERT ? Severity::ALERT : Severity::WARNING;
    string msg = "Trim uses " + to_string(static_cast<int>(used * 100)) + "% of the adjustment room it has ("
                 + to_string(trim) + " of " + (trim > 0 ? "+" : "-") + to_string(room) + " us).";
    findings.push_back({level, servoLabel(robot, servo), msg,
                        "Re-seat the horn mechanically instead of adding more trim; check for a stripped spline or loose screw."});
  }
}

// a pulse range much narrower than similar joints limits how far the leg can move
void MockAnomalyDetector::checkPulseRanges(Hexapod& robot, vector<Finding>& findings) const {
  for (int joint = 0; joint < SERVOS_PER_LEG; joint++) {
    vector<Servo *> group;
    vector<double> spans;
    for (int leg = 0; leg < NUM_LEGS; leg++) {
      Servo * servo = robot.findServo(leg * SERVOS_PER_LEG + joint);
      if (servo != nullptr) {
        group.push_back(servo);
        spans.push_back(servo->getMaxPulse() - servo->getMinPulse());
      }
    }
    double typical = median(spans);
    for (size_t i = 0; i < group.size(); i++) {
      if (spans[i] < typical * NARROW_RANGE) {
        string msg = "Pulse range is " + to_string(static_cast<int>(spans[i])) + " us wide; other "
                     + JOINTS[joint] + "s have about " + to_string(static_cast<int>(typical)) + " us.";
        findings.push_back({Severity::INFO, servoLabel(robot, group[i]), msg,
                            "If this was not on purpose (to avoid a collision), widen the range with menu option 5."});
      }
    }
  }
}

// the robot is only stable if a full tripod of feet is on the ground
void MockAnomalyDetector::checkStance(Hexapod& robot, vector<Finding>& findings) const {
  bool down[NUM_LEGS];
  int feetDown = 0;
  for (int leg = 0; leg < NUM_LEGS; leg++) {
    Footswitch * foot = robot.findSwitch(leg);
    down[leg] = foot != nullptr && foot->isPressed();
    if (down[leg]) {
      feetDown++;
    }
  }

  bool tripodA = down[TRIPOD_A[0]] && down[TRIPOD_A[1]] && down[TRIPOD_A[2]];
  bool tripodB = down[TRIPOD_B[0]] && down[TRIPOD_B[1]] && down[TRIPOD_B[2]];

  if (feetDown < 3) {
    findings.push_back({Severity::ALERT, "Foot switches",
                        "Only " + to_string(feetDown) + " of 6 feet report ground contact; the robot cannot stand.",
                        "Check the robot is resting on the ground and test each foot switch with menu option 6."});
  } else if (!tripodA && !tripodB) {
    findings.push_back({Severity::WARNING, "Foot switches",
                        to_string(feetDown) + " feet are down, but they do not form a tripod "
                        "(" + robot.getLegName(TRIPOD_A[0]) + "/" + robot.getLegName(TRIPOD_A[1]) + "/" + robot.getLegName(TRIPOD_A[2])
                        + " or " + robot.getLegName(TRIPOD_B[0]) + "/" + robot.getLegName(TRIPOD_B[1]) + "/" + robot.getLegName(TRIPOD_B[2]) + ").",
                        "The stance may tip over; a switch may be stuck or not wired."});
  }
}

HealthReport MockAnomalyDetector::analyze(Hexapod& robot) {
  HealthReport report;
  report.source = getName();

  checkStance(robot, report.findings);
  checkTrimHeadroom(robot, report.findings);
  checkTrimOutliers(robot, report.findings);
  checkPulseRanges(robot, report.findings);

  // most severe first; stable_sort keeps the check order within a level
  stable_sort(report.findings.begin(), report.findings.end(),
              [](const Finding& a, const Finding& b) { return a.severity > b.severity; });

  int alerts = 0, warnings = 0, infos = 0;
  for (const Finding& f : report.findings) {
    if (f.severity == Severity::ALERT) alerts++;
    else if (f.severity == Severity::WARNING) warnings++;
    else infos++;
  }
  report.score = max(0, 100 - 25 * alerts - 10 * warnings - 2 * infos);

  // summarization: a live AI would write this; the mock uses a template
  if (report.findings.empty()) {
    report.summary = "All 18 servos and 6 foot switches look normal. Calibration is consistent across legs.";
  } else {
    report.summary = "Found " + to_string(alerts) + " alert(s), " + to_string(warnings) + " warning(s), and "
                     + to_string(infos) + " note(s). Most urgent: " + report.findings[0].part + " - "
                     + report.findings[0].message;
  }
  return report;
}

// --- shared functions ---

string buildCalibrationSnapshot(Hexapod& robot) {
  ostringstream out;
  out << "{\n  \"servos\": [\n";
  for (int id = 0; id < NUM_SERVOS; id++) {
    Servo * s = robot.findServo(id);
    if (s == nullptr) {
      continue;
    }
    out << "    {\"id\": " << id
        << ", \"leg\": \"" << robot.getLegName(id / SERVOS_PER_LEG) << "\""
        << ", \"joint\": \"" << s->getName() << "\""
        << ", \"min\": " << s->getMinPulse()
        << ", \"max\": " << s->getMaxPulse()
        << ", \"center\": " << s->getCenterPulse()
        << ", \"trim\": " << s->getTrim() << "}"
        << (id < NUM_SERVOS - 1 ? "," : "") << "\n";
  }
  out << "  ],\n  \"foot_switches\": [\n";
  for (int id = 0; id < NUM_LEGS; id++) {
    Footswitch * f = robot.findSwitch(id);
    if (f == nullptr) {
      continue;
    }
    out << "    {\"id\": " << id
        << ", \"leg\": \"" << robot.getLegName(id) << "\""
        << ", \"pressed\": " << (f->isPressed() ? "true" : "false") << "}"
        << (id < NUM_LEGS - 1 ? "," : "") << "\n";
  }
  out << "  ]\n}\n";
  return out.str();
}

void displayReport(const HealthReport& report) {
  cout << "=== Calibration Health Check ===" << endl;
  cout << "Analyzed by: " << report.source << endl;
  cout << "Health score: " << report.score << " / 100" << endl;
  cout << endl;
  cout << "Summary: " << report.summary << endl;

  if (report.findings.empty()) {
    cout << endl;
    return;
  }

  cout << endl << "Findings:" << endl;
  int number = 1;
  for (const Finding& f : report.findings) {
    cout << " " << number++ << ". [" << severityText(f.severity) << "] " << f.part << endl;
    cout << "    " << f.message << endl;
    cout << "    Suggestion: " << f.suggestion << endl;
  }
  cout << endl;
}
