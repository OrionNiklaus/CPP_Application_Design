## Description:
---

* Module 8 Input Validation Assignment.
* Add validation to the application so it does not accept bad input. Check types, ranges, required fields, and formats (negative values, empty names, incorrectly formatted dates). Give the user a useful error message and let them try again.
* Builds on the Midterm hexapod calibration tool. All input checks live in a new `Validation.h` / `Validation.cpp` module so every prompt and the config file loader use the same rules.

## Changes from the Midterm:
---

### Problems fixed
1. **Partial numbers accepted** - midterm `readInt` used `cin >> value`, so `12abc` was read as `12` and `abc` was left behind for the next prompt. Now every prompt reads a full line with `getline` and `parseInt` requires the *whole* line to be a number.
2. **No retry after a rejected value** - midterm printed "Trim rejected" or "Servo not found" and went back to the menu. Now every prompt repeats with an error message until the value is valid.
3. **Negative values wrapped around** - a negative id stored into a `uint8_t` became 255, and negative pulse widths were never checked. All values are range checked as `int` *before* being stored, with a specific "cannot be negative" message.
4. **No names or dates** - the midterm never asked for any text. Saving now asks for a technician name and a calibration date.
5. **Config file was trusted** - `loadConfig` now validates every line and reports each problem with its line number instead of loading bad data.
6. **Infinite loop on end of input** - pressing Ctrl+D made `readInt` loop forever. Now an `InputClosed` exception is thrown and `main` exits cleanly.

### New / changed features
* **`Validation.h` / `Validation.cpp`** (new)
  * Checks: `parseInt`, `checkRange`, `checkName`, `checkDate` - each returns `true`/`false` and an error message.
  * Prompts: `readIntInRange`, `readName`, `readDate`, `readYesNo` - keep asking until the input passes.
* **Menu option 5: Edit servo pulse range** (new) - min, max, and center pulse must be 500-2500 us and `min < center < max`. Each prompt's allowed range is based on the previous answer. If the old trim no longer fits the new range it is reset to 0 and the user is told.
* **Adjust servo trim** - the allowed range shown is calculated from that servo's own pulse limits (`getMinTrim()` / `getMaxTrim()`), not just +/-200.
* **Save calibration** - asks for technician name, date, and a y/n confirmation, then writes `# calibrated by NAME on DATE` at the top of `servo_config.txt`.
* **`Servo::validatePulseRange`** - one static check shared by the menu and the config loader.
* Menu was renumbered to make room for option 5 (toggle switch is now 6, save is now 9).

## Validation rules:
---

| Input | Rules | Example error |
|---|---|---|
| Any number | whole line must be an integer, required | `"1.5" is not a whole number (no decimals).` |
| Menu choice | 0-9 | `42 is out of range (allowed: 0 to 9).` |
| Servo ID / switch ID | 0-17 / 0-5 | `Value cannot be negative (allowed: 0 to 17).` |
| Trim | within +/-200 **and** keeps center inside the servo's range | `500 is out of range (allowed: -200 to 200).` |
| Pulse range | 500-2500 us, min < center < max | `900 is out of range (allowed: 1002 to 2500).` |
| Technician name | required, max 40 characters, letters / spaces / `-` `'` `.` only | `Name cannot be empty.` |
| Date | `YYYY-MM-DD`, real calendar date (leap years checked), year 2000+, not in the future | `2026-02 only has 28 days.` |
| Overwrite confirm | y / yes / n / no (any case) | `Answer must be y or n.` |

**Config file (`servo_config.txt`) checks:** correct number of values per line, every number is a valid integer, ids/channels/pins/pulses/trim in range, `pressed` is 0 or 1, joint name matches the servo's position (coxa/femur/tibia), no duplicate servo ids, channels, switch ids, or GPIO pins, and no unknown line types. Example:
```
Error - servo_config.txt line 3: servo id: Value cannot be negative (allowed: 0 to 17).
Error - servo_config.txt line 5: servo 3 must be a coxa, not "tibia"
Error - servo_config.txt line 10: channel 7 is used by more than one servo
3 problem(s) found in servo_config.txt. Fix the file and try again.
```

## Instructions for running:
---

```
g++ -std=c++17 -Wall -o hexapod *.cpp
./hexapod
```
Run from this folder so `servo_config.txt` is found. Try entering `abc`, `1.5`, `-5`, `99`, an empty name, `11/09/2026`, or `2026-02-30` to see the validation messages.
