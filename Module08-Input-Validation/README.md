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
* Menu was renumbered to make room for option 5