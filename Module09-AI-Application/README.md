## Description:
---

* Module 9 AI Application Assignment.
* Choose one AI-powered feature for the application. Describe what it does, what data it needs, what the user sees, and where AI fits into the current architecture. Build a mock version or prototype if a live AI service is not available.
* Builds on the Module 8 hexapod calibration tool. The AI feature chosen is **anomaly detection** with a plain-English **summary**: an *AI Calibration Health Check* (menu option 10).

## The feature: AI Calibration Health Check
---

### What it does
Hand-calibrating 18 servos is easy to get wrong, and a bad value usually looks fine until the robot walks badly or a servo strains against its limit. The health check looks at the whole robot at once and flags anything that looks wrong compared with the rest of the robot, then explains each problem and what to do about it.

The mock version checks four things:

| Check | Idea | Severity |
|---|---|---|
| **Trim outliers** | All six coxas (and all six femurs, all six tibias) should need similar trims. A trim far from its group usually means the servo horn is on the wrong spline tooth. Uses a *modified z-score* (median and median absolute deviation), which is not thrown off by the outlier itself. | WARNING |
| **Trim headroom** | A trim using most of its available room (e.g. -195 of -200 us) means the joint has drifted and there is little room left to correct it. | WARNING at 60%, ALERT at 90% |
| **Narrow pulse range** | A pulse range much narrower than similar joints limits how far that leg can move. | INFO |
| **Stance stability** | A hexapod walking with a tripod gait needs one full tripod (Front Left / Middle Right / Rear Left, or Front Right / Middle Left / Rear Right) on the ground. Fewer than 3 feet down = cannot stand. | WARNING / ALERT |

### What data it needs
* **Available now (used by the mock):** each servo's id, leg, joint, min / max / center pulse, and trim, plus each foot switch's pressed state. This is the same data already loaded from `servo_config.txt`.
* **A live AI version would also want:**
  * Calibration history (trim values over time, saved each time the technician saves) to spot slow drift.
  * Servo telemetry from the Pimoroni Servo 2040 board: current draw per servo (a straining servo draws more current), and IMU readings while standing to check the body is level.
  * Example calibrations labeled "good" or "bad" to train or prompt the model.

### What the user sees
```
=== Calibration Health Check ===
Analyzed by: Mock AI (offline statistical model)
Health score: 55 / 100

Summary: Found 1 alert(s), 2 warning(s), and 0 note(s). Most urgent: Foot switches - Only 2 of 6 feet report ground contact; the robot cannot stand.

Findings:
 1. [ALERT] Foot switches
    Only 2 of 6 feet report ground contact; the robot cannot stand.
    Suggestion: Check the robot is resting on the ground and test each foot switch with menu option 6.
 2. [WARNING] Servo 4 (femur, Middle Left)
    Trim uses 75% of the adjustment room it has (150 of +200 us).
    Suggestion: Re-seat the horn mechanically instead of adding more trim; check for a stripped spline or loose screw.
 3. [WARNING] Servo 4 (femur, Middle Left)
    Trim of 150 us is unusual for a femur (femurs on this robot are typically near 4 us, score 10.9).
    Suggestion: Check the servo horn is on the correct spline tooth, re-center it, then re-trim.

Show the data a live AI service would receive? (y/n):
```
* A **health score** (0-100), a one-line **summary**, and a list of **findings**, most severe first, each with a **suggestion**.
* Optionally, the **JSON snapshot** of calibration data that would be sent to a live AI service.

### Where AI fits into the architecture
```
                     main.cpp (menu)
                          |
         option 10 -> runHealthCheck(robot, detector)
                          |
                          v
              AnomalyDetector  (abstract interface)
               /                        \
   MockAnomalyDetector             LiveAIDetector  (future)
   - runs offline                  - buildCalibrationSnapshot() -> JSON
   - statistics + robot rules      - send JSON to AI service / on-board model
               \                        /
                          v
                    HealthReport  -> displayReport()
                          ^
                          |
        reads Hexapod data (findServo, findSwitch, getLegName)
```
* `AnomalyDetector` is an abstract base class, like `Component` from the midterm. `main.cpp` only uses the interface, so the mock can be replaced by a live AI detector by changing **one line** in `main()`. The menu and display code do not change.
* The detector only **reads** the robot. It never changes calibration values; the technician decides what to fix.
* **Live options:** a hosted LLM (send the JSON snapshot and ask for findings plus a summary), or a small anomaly model running on the Raspberry Pi 5 / AI Pi hat so it works without internet. The mock keeps working as an offline fallback.

### Why a mock
There is no live AI service connected to this project, and the robot has no telemetry history yet to train a model on. The mock uses robust statistics and robot-specific rules to produce the same `HealthReport` a real model would, which proves the interface and the user experience. The thresholds are constants at the top of `AnomalyDetector.cpp`.

## Changes from Module 8:
---

* **`AnomalyDetector.h` / `AnomalyDetector.cpp`** (new) - `Severity`, `Finding`, `HealthReport`, the abstract `AnomalyDetector` class, `MockAnomalyDetector`, `buildCalibrationSnapshot()`, and `displayReport()`.
* **`main.cpp`** - new menu option **10. AI calibration health check**; menu range is now 0-10; a `MockAnomalyDetector` is created in `main()` and passed to `runHealthCheck()` as an `AnomalyDetector&`.
* **`Hexapod.h` / `Hexapod.cpp`** - added `getLegName(int)` so findings can say which leg a servo is on.
* All Module 8 input validation is unchanged and still used (menu choice, y/n prompt).

## Instructions for running:
---

```
g++ -std=c++17 -Wall -o hexapod *.cpp
./hexapod
```
Run from this folder so `servo_config.txt` is found.

To see the health check find problems:
1. Option **10** on the default config: score 100, no findings.
2. Option **4**, servo **4**, trim **150**; then option **10**: trim outlier + headroom warnings.
3. Option **5**, servo **2**, pulses **1000 / 1300 / 1150**; then option **10**: narrow range note.
4. Option **6** to lift feet **0** and **1**; then option **10**: no tripod warning. Lift **4** and **5** too: cannot stand alert.
