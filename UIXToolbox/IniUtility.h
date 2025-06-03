#ifndef INIUTILITY_H
#define INIUTILITY_H

#include "pointerVector.h"
#include <cstring>

// Maximum sizes for line and value buffers
const int MAX_LINE_SIZE = 1280;
const int MAX_VALUE_SIZE = 1024;

// IniUtility class declaration
class IniUtility {
public:
    // Retrieves the value associated with a key in a specific section
    static char* GetValue(const char* filename, const char* section, const char* key);

    // Sets the value associated with a key in a specific section
    static bool SetValue(const char* filename, const char* section, const char* key, const char* value);

    // Retrieves all section names in the INI file
    static pointerVector<char*>* GetSectionNames(const char* filename);

    // Retrieves all keys within a specific section
    static pointerVector<char*>* GetSectionKeys(const char* filename, const char* section);

    // Deletes a specific key in a section
    static bool DeleteKey(const char* filename, const char* section, const char* key);

    // Deletes a specific section
    static bool DeleteSection(const char* filename, const char* section);

private:
	// Helpers
	static bool isNullOrEmpty(const char* strParam) {
		return (strParam == NULL || strcmp(strParam, "") == 0);
    }
};

#endif // INIUTILITY_H