// Utils.cpp - helper function definitions.

#include "Utils.h"

std::string trim(const std::string& text) {
    std::string result = text;          // work on a copy
    // erase() is a string modifier: remove spaces from the front...
    while (!result.empty() && isSpaceChar(result[0]))
        result.erase(0, 1);
    // ...and from the back.
    while (!result.empty() && isSpaceChar(result[result.length() - 1]))
        result.erase(result.length() - 1, 1);
    return result;
}

void toUpperCase(std::string& text) {
    int n = text.length();
    for (int i = 0; i < n; ++i) {
        if (text[i] >= 'a' && text[i] <= 'z')
            text[i] = text[i] - 'a' + 'A';   // ASCII shift: 'a' (97) -> 'A' (65)
    }
}

bool parseInt(const std::string& text, int minValue, int maxValue, int& value) {
    std::string t = trim(text);
    if (t.empty()) return false;

    bool negative = false;
    int n = t.length();
    int i = 0;
    if (t[0] == '-' || t[0] == '+') {
        negative = (t[0] == '-');
        i = 1;
        if (n == 1) return false;            // just a sign, no digits
    }

    long result = 0;                          // long, so we can detect overflow
    for (; i < n; ++i) {
        if (!isDigitChar(t[i])) return false; // rejects "12abc", "1.5", "1 2"
        result = result * 10 + (t[i] - '0');
        if (result > 1000000000L) return false;
    }
    if (negative) result = -result;
    if (result < minValue || result > maxValue) return false;

    value = result;                           // write back through the reference
    return true;
}

bool parseMoney(const std::string& text, double& value) {
    std::string t = trim(text);
    if (t.empty()) return false;

    long rupees = 0;
    long paise = 0;
    int digitsBeforeDot = 0;
    int digitsAfterDot = 0;
    bool seenDot = false;

    int n = t.length();
    for (int i = 0; i < n; ++i) {
        char c = t[i];
        if (c == '.') {
            if (seenDot) return false;       // "1.2.3"
            seenDot = true;
        } else if (isDigitChar(c)) {
            if (!seenDot) {
                rupees = rupees * 10 + (c - '0');
                if (++digitsBeforeDot > 7) return false;   // too large
            } else {
                paise = paise * 10 + (c - '0');
                if (++digitsAfterDot > 2) return false;    // "4.555"
            }
        } else {
            return false;                    // letters, minus sign, spaces...
        }
    }
    if (digitsBeforeDot == 0) return false;  // ".5" is not accepted
    if (seenDot && digitsAfterDot == 0) return false;   // "5."
    if (digitsAfterDot == 1) paise *= 10;    // "4.5" means 4 rupees 50 paise

    value = rupees + paise / 100.0;
    return true;
}
