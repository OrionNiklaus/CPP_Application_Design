#include <iostream>
#include <cctype>
#include <ctime>
#include "Validation.h"
using namespace std;

const size_t MAX_NAME_LENGTH = 40;
const int MIN_YEAR = 2000;

string trim(const string& text) {
  size_t start = text.find_first_not_of(" \t\r\n");
  if (start == string::npos) {
    return "";
  }
  size_t end = text.find_last_not_of(" \t\r\n");
  return text.substr(start, end - start + 1);
}

// the whole string must be a whole number: "12" ok; "12abc", "1.5", "" rejected
bool parseInt(const string& rawText, int& value, string& error) {
  string text = trim(rawText);
  if (text.empty()) {
    error = "A value is required.";
    return false;
  }

  try {
    size_t used = 0;
    int result = stoi(text, &used);
    if (used != text.size()) { // characters left over after the number
      if (text.find('.') != string::npos) {
        error = "\"" + text + "\" is not a whole number (no decimals).";
      } else {
        error = "\"" + text + "\" is not a whole number.";
      }
      return false;
    }
    value = result;
    return true;
  } catch (const invalid_argument&) {
    error = "\"" + text + "\" is not a whole number.";
  } catch (const out_of_range&) {
    error = "\"" + text + "\" is too large.";
  }
  return false;
}

bool checkRange(int value, int min, int max, string& error) {
  if (value >= min && value <= max) {
    return true;
  }
  string allowed = "(allowed: " + to_string(min) + " to " + to_string(max) + ")";
  if (value < 0 && min >= 0) {
    error = "Value cannot be negative " + allowed + ".";
  } else {
    error = to_string(value) + " is out of range " + allowed + ".";
  }
  return false;
}

// required, letters plus spaces, hyphens, apostrophes, periods
bool checkName(const string& rawName, string& error) {
  string name = trim(rawName);
  if (name.empty()) {
    error = "Name cannot be empty.";
    return false;
  }
  if (name.size() > MAX_NAME_LENGTH) {
    error = "Name must be " + to_string(MAX_NAME_LENGTH) + " characters or fewer.";
    return false;
  }

  bool hasLetter = false;
  for (char c : name) {
    unsigned char uc = static_cast<unsigned char>(c);
    if (isalpha(uc)) {
      hasLetter = true;
    } else if (c != ' ' && c != '-' && c != '\'' && c != '.') {
      error = "Name can only contain letters, spaces, hyphens (-), apostrophes ('), and periods (.).";
      return false;
    }
  }
  if (!hasLetter) {
    error = "Name must contain at least one letter.";
    return false;
  }
  return true;
}

static bool isLeapYear(int year) {
  return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

static int daysInMonth(int year, int month) {
  const int DAYS[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 2 && isLeapYear(year)) {
    return 29;
  }
  return DAYS[month - 1];
}

// today's date as YYYY-MM-DD
static string todayString() {
  time_t now = time(nullptr);
  char buffer[11];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d", localtime(&now));
  return string(buffer);
}

// format YYYY-MM-DD, a real calendar date, not in the future
bool checkDate(const string& rawDate, string& error) {
  string date = trim(rawDate);
  if (date.empty()) {
    error = "Date cannot be empty.";
    return false;
  }

  // check format: 4 digits, dash, 2 digits, dash, 2 digits
  bool formatOk = date.size() == 10 && date[4] == '-' && date[7] == '-';
  for (size_t i = 0; formatOk && i < date.size(); i++) {
    if (i != 4 && i != 7 && !isdigit(static_cast<unsigned char>(date[i]))) {
      formatOk = false;
    }
  }
  if (!formatOk) {
    error = "Date must be in YYYY-MM-DD format, e.g. 2026-11-09.";
    return false;
  }

  int year = stoi(date.substr(0, 4));
  int month = stoi(date.substr(5, 2));
  int day = stoi(date.substr(8, 2));

  if (year < MIN_YEAR) {
    error = "Year must be " + to_string(MIN_YEAR) + " or later.";
    return false;
  }
  if (month < 1 || month > 12) {
    error = "Month must be between 01 and 12.";
    return false;
  }
  int maxDay = daysInMonth(year, month);
  if (day < 1 || day > maxDay) {
    error = date.substr(0, 7) + " only has " + to_string(maxDay) + " days.";
    return false;
  }

  // YYYY-MM-DD strings compare in date order
  string today = todayString();
  if (date > today) {
    error = "Date cannot be in the future (today is " + today + ").";
    return false;
  }
  return true;
}

// read a full line; getline everywhere means no leftover input between prompts
string readLine(const string& prompt) {
  cout << prompt;
  string line;
  if (!getline(cin, line)) {
    throw InputClosed();
  }
  return line;
}

static void showError(const string& error) {
  cout << "  Invalid input: " << error << " Please try again." << endl;
}

int readIntInRange(const string& prompt, int min, int max) {
  while (true) {
    string line = readLine(prompt);
    int value;
    string error;
    if (parseInt(line, value, error) && checkRange(value, min, max, error)) {
      return value;
    }
    showError(error);
  }
}

string readName(const string& prompt) {
  while (true) {
    string line = readLine(prompt);
    string error;
    if (checkName(line, error)) {
      return trim(line);
    }
    showError(error);
  }
}

string readDate(const string& prompt) {
  while (true) {
    string line = readLine(prompt);
    string error;
    if (checkDate(line, error)) {
      return trim(line);
    }
    showError(error);
  }
}

bool readYesNo(const string& prompt) {
  while (true) {
    string answer = trim(readLine(prompt));
    for (char& c : answer) {
      c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
    if (answer == "y" || answer == "yes") {
      return true;
    }
    if (answer == "n" || answer == "no") {
      return false;
    }
    showError("Answer must be y or n.");
  }
}
