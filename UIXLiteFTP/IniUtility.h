#ifndef INIUTILITY_H
#define INIUTILITY_H

#include "pointerVector.h"

class IniUtility {
public:
    // Returns the value for a given section and key
    static char* GetValue(const char* filename, const char* section, const char* key);

    // Sets the value for a given section and key
    static bool SetValue(const char* filename, const char* section, const char* key, const char* value);

    // Returns all section names
    static pointerVector<char*>* GetSectionNames(const char* filename);

    // Returns all key names in a section
    static pointerVector<char*>* GetSectionKeys(const char* filename, const char* section);

    // Deletes a specific key from a section
    static bool DeleteKey(const char* filename, const char* section, const char* key);

    // Deletes a section and all its keys
    static bool DeleteSection(const char* filename, const char* section);
};

#endif // INIUTILITY_H