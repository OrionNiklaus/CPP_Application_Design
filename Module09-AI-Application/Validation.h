#pragma once
#include <string>
#include <stdexcept>

// thrown when the input stream closes (Ctrl+D) so main can exit cleanly
// instead of looping forever on a dead stream
class InputClosed : public std::runtime_error {
public:
  InputClosed() : std::runtime_error("input closed") {}
};

// remove leading and trailing whitespace
std::string trim(const std::string& text);

// --- checks ---
// each returns true if the value is valid; otherwise fills error
// with a message that tells the user what was wrong
bool parseInt(const std::string& text, int& value, std::string& error);
bool checkRange(int value, int min, int max, std::string& error);
bool checkName(const std::string& name, std::string& error);
bool checkDate(const std::string& date, std::string& error);

// --- prompts ---
// each keeps asking until the input passes its checks
std::string readLine(const std::string& prompt);
int readIntInRange(const std::string& prompt, int min, int max);
std::string readName(const std::string& prompt);
std::string readDate(const std::string& prompt);
bool readYesNo(const std::string& prompt);
