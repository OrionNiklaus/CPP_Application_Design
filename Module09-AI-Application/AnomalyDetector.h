#pragma once
#include <string>
#include <vector>
#include "Hexapod.h"

// how serious a finding is
enum class Severity { INFO, WARNING, ALERT };

// one problem the detector noticed
struct Finding {
  Severity severity;
  std::string part;        // ex. "Servo 4 (femur, Middle Left)"
  std::string message;     // what looks wrong
  std::string suggestion;  // what the user should do about it
};

// everything the user sees after a health check
struct HealthReport {
  std::string source;               // which detector made the report
  int score;                        // 0-100, 100 = nothing unusual
  std::string summary;              // short plain-English overview
  std::vector<Finding> findings;    // most severe first
};

// abstract interface: the rest of the app only talks to this,
// so a live AI service can replace the mock without changing main.cpp
class AnomalyDetector {
public:
  virtual ~AnomalyDetector() {}
  virtual std::string getName() const = 0;
  virtual HealthReport analyze(Hexapod& robot) = 0;
};

// offline prototype: uses robust statistics and robot-specific rules
// in place of a trained model or hosted AI service
class MockAnomalyDetector : public AnomalyDetector {
public:
  std::string getName() const override;
  HealthReport analyze(Hexapod& robot) override;

private:
  void checkTrimOutliers(Hexapod& robot, std::vector<Finding>& findings) const;
  void checkTrimHeadroom(Hexapod& robot, std::vector<Finding>& findings) const;
  void checkPulseRanges(Hexapod& robot, std::vector<Finding>& findings) const;
  void checkStance(Hexapod& robot, std::vector<Finding>& findings) const;
};

// calibration data formatted as JSON: what would be sent to a live AI service
std::string buildCalibrationSnapshot(Hexapod& robot);

// print a report for the user
void displayReport(const HealthReport& report);
