// Utils.h
// Small user-defined helper functions for strings and input parsing.
// They are written by hand (instead of library calls) so that every
// validation rule is visible and easy to explain.

#ifndef UTILS_H
#define UTILS_H

#include <string>

// Character checks (ASCII based).
inline bool isDigitChar(char c)  { return c >= '0' && c <= '9'; }
inline bool isLetterChar(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
inline bool isSpaceChar(char c)  { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }

// Returns a copy without leading/trailing spaces. Parameter is passed
// by const reference: no copy is made on the way in, and the caller's
// string cannot be changed.
std::string trim(const std::string& text);

// Converts the string IN PLACE to uppercase. Parameter is passed by
// (non-const) reference, so the caller's own string is modified.
void toUpperCase(std::string& text);

// Parses a whole number. Returns true and stores the value in "value"
// (call by reference) only if the full text is a valid integer that
// fits between minValue and maxValue.
bool parseInt(const std::string& text, int minValue, int maxValue, int& value);

// Parses a money amount like "45", "45.5" or "45.50" (at most 2 decimals).
bool parseMoney(const std::string& text, double& value);

#endif
