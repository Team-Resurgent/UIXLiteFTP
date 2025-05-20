#include "IniUtility.h"
#include <stdio.h>
#include <string.h>
#include "pointerVector.h"

namespace {
    char* trim(char* str)
    {
        if (str == NULL) return NULL;

        // Trim leading spaces
        char* start = str;
        while (*start == ' ' || *start == '\t') start++;

        // Trim trailing spaces
        char* end = start + strlen(start) - 1;
        while (end > start && (*end == ' ' || *end == '\t')) end--;
        *(end + 1) = '\0';

        return start;
    }

    bool isSection(char* line, char* sectionName)
    {
        if (line && line[0] == '[')
        {
            char* end = strchr(line, ']');
            if (end)
            {
                strncpy(sectionName, line + 1, end - line - 1);
                sectionName[end - line - 1] = '\0';
                return true;
            }
        }
        return false;
    }

    bool isKeyValuePair(char* line, char* key, char* value, bool& hasSpaces)
    {
        char* equalPos = strchr(line, '=');
        if (equalPos)
        {
            strncpy(key, line, equalPos - line);
            key[equalPos - line] = '\0';
            strcpy(value, equalPos + 1);

            // Check if spaces are around '='
            hasSpaces = (strstr(line, " = ") != NULL);
            trim(key);
            trim(value);
            return true;
        }
        return false;
    }
}

char* IniUtility::GetValue(const char* filename, const char* section, const char* key)
{
    FILE* file = fopen(filename, "r");
    if (!file) return strdup("");

    char line[MAX_LINE_SIZE], currentSection[256];
    char keyStr[256];
    strcpy(keyStr, key);
    bool inGlobalSection = (section == NULL || strlen(section) == 0);

    while (fgets(line, sizeof(line), file))
    {
        char* trimmedLine = trim(line);
        if (*trimmedLine == '\0' || *trimmedLine == ';') continue;

        if (isSection(trimmedLine, currentSection))
        {
            inGlobalSection = false;
            if (strcmp(currentSection, section) == 0)
            {
                inGlobalSection = true;
            }
            continue;
        }

        char foundKey[256], foundValue[MAX_VALUE_SIZE];
        bool hasSpaces;
        if (isKeyValuePair(trimmedLine, foundKey, foundValue, hasSpaces))
        {
            if ((inGlobalSection || strcmp(currentSection, section) == 0) && strcmp(foundKey, key) == 0)
            {
                char* result = strdup(foundValue);
                fclose(file);
                return result;
            }
        }
    }
    fclose(file);
    return strdup("");
}

bool IniUtility::SetValue(const char* filename, const char* section, const char* key, const char* value)
{
    FILE* file = fopen(filename, "r");
    if (!file) return false;

    FILE* tempFile = tmpfile();
    if (!tempFile)
    {
        fclose(file);
        return false;
    }

    char line[MAX_LINE_SIZE], currentSection[256], previousSection[256] = "";
    char keyStr[256], valueStr[MAX_VALUE_SIZE];
    strcpy(keyStr, key);
    strcpy(valueStr, value);
    bool keyUpdated = false;
    bool inGlobalSection = true;
    bool firstFormatDetected = false;
    bool useSpaces = false; // Determines if spaces should be used around '='

    while (fgets(line, sizeof(line), file))
    {
        char* trimmedLine = trim(line);
        if (*trimmedLine == '\0' || *trimmedLine == ';')
        {
            fprintf(tempFile, "%s\n", line);
            continue;
        }

        char foundKey[256], foundValue[MAX_VALUE_SIZE];
        bool lineHasSpaces;
        if (isSection(trimmedLine, currentSection))
        {
            if (!keyUpdated && strcmp(previousSection, section) == 0) {
                fprintf(tempFile, "%s%s%s\n", keyStr, useSpaces ? " = " : "=", valueStr);
                keyUpdated = true;
            }
            strcpy(previousSection, currentSection);
            fprintf(tempFile, "%s\n", line);
            inGlobalSection = false;
            continue;
        }

        if (isKeyValuePair(trimmedLine, foundKey, foundValue, lineHasSpaces))
        {
            if (!firstFormatDetected)
            {
                useSpaces = lineHasSpaces;
                firstFormatDetected = true;
            }

            if ((inGlobalSection || strcmp(currentSection, section) == 0) && strcmp(foundKey, keyStr) == 0)
            {
                fprintf(tempFile, "%s%s%s\n", keyStr, useSpaces ? " = " : "=", valueStr);
                keyUpdated = true;
                continue;
            }
        }

        fprintf(tempFile, "%s\n", line);
    }

    if (!keyUpdated)
    {
        if (!inGlobalSection && strcmp(currentSection, section) != 0)
        {
            fprintf(tempFile, "[%s]\n", section);
        }
        fprintf(tempFile, "%s%s%s\n", keyStr, useSpaces ? " = " : "=", valueStr);
    }

    fclose(file);

    file = fopen(filename, "w");
    if (!file)
    {
        fclose(tempFile);
        return false;
    }

    // Copy temp file content to original file
    rewind(tempFile);
    while (fgets(line, sizeof(line), tempFile))
    {
        fprintf(file, "%s", line);
    }

    fclose(tempFile);
    fclose(file);

    return true;
}

pointerVector<char*>* IniUtility::GetSectionNames(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        return new pointerVector<char*>(false);
    }

    char line[MAX_LINE_SIZE];
    pointerVector<char*>* sections = new pointerVector<char*>(false);

    while (fgets(line, sizeof(line), file))
    {
        char sectionName[256];
        if (isSection(trim(line), sectionName))
        {
            char* section = strdup(sectionName);
            sections->add(section);
        }
    }

    fclose(file);
    return sections;
}

pointerVector<char*>* IniUtility::GetSectionKeys(const char* filename, const char* section)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        return new pointerVector<char*>(false);
    }

    char line[MAX_LINE_SIZE], currentSection[256];
    pointerVector<char*>* keys = new pointerVector<char*>(false);
    bool inGlobalSection = (section == NULL || strlen(section) == 0);

    while (fgets(line, sizeof(line), file))
    {
        char* trimmedLine = trim(line);
        if (*trimmedLine == '\0' || *trimmedLine == ';') continue;

        if (isSection(trimmedLine, currentSection))
        {
            inGlobalSection = false;
            if (strcmp(currentSection, section) == 0)
            {
                inGlobalSection = true;
            }
            continue;
        }

        if (inGlobalSection || strcmp(currentSection, section) == 0)
        {
            char key[256], value[MAX_VALUE_SIZE];
            bool hasSpaces;
            if (isKeyValuePair(trimmedLine, key, value, hasSpaces))
            {
                char* keyCStr = strdup(key);
                keys->add(keyCStr);
            }
        }
    }

    fclose(file);
    return keys;
}

bool IniUtility::DeleteKey(const char* filename, const char* section, const char* key)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        return false;
    }

    FILE* tempFile = tmpfile();
    if (!tempFile)
    {
        fclose(file);
        return false;
    }

    char line[MAX_LINE_SIZE], currentSection[256];
    bool keyDeleted = false;
    bool inGlobalSection = (section == NULL || strlen(section) == 0);

    while (fgets(line, sizeof(line), file))
    {
        char* trimmedLine = trim(line);
        if (*trimmedLine == '\0' || *trimmedLine == ';')
        {
            fprintf(tempFile, "%s\n", line);
            continue;
        }

        char foundKey[256], foundValue[MAX_VALUE_SIZE];
        bool hasSpaces;
        if (isSection(trimmedLine, currentSection))
        {
            fprintf(tempFile, "%s\n", line);
            inGlobalSection = false;
            continue;
        }

        if ((inGlobalSection || strcmp(currentSection, section) == 0) && isKeyValuePair(trimmedLine, foundKey, foundValue, hasSpaces))
        {
            if (strcmp(foundKey, key) == 0)
            {
                keyDeleted = true;
                continue; // Skip writing this line to delete the key
            }
        }

        fprintf(tempFile, "%s\n", line);
    }

    fclose(file);

    if (!keyDeleted)
    {
        fclose(tempFile);
        return false;
    }

    file = fopen(filename, "w");
    if (!file)
    {
        fclose(tempFile);
        return false;
    }

    rewind(tempFile);
    while (fgets(line, sizeof(line), tempFile))
    {
        fprintf(file, "%s", line);
    }

    fclose(tempFile);
    fclose(file);

    return true;
}

bool IniUtility::DeleteSection(const char* filename, const char* section)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        return false;
    }

    FILE* tempFile = tmpfile();
    if (!tempFile)
    {
        fclose(file);
        return false;
    }

    char line[MAX_LINE_SIZE], currentSection[256];
    bool sectionDeleted = false;
    bool inTargetSection = false;
    bool deleteGlobal = (section == NULL || strlen(section) == 0);

    while (fgets(line, sizeof(line), file))
    {
        char* trimmedLine = trim(line);

        if (*trimmedLine == '\0' || *trimmedLine == ';')
        {
            if (!inTargetSection)
            {
                fprintf(tempFile, "%s\n", line);
            }
            continue;
        }

        char sectionName[256];
        if (isSection(trimmedLine, sectionName))
        {
            if (inTargetSection || (deleteGlobal && !sectionDeleted))
            {
                inTargetSection = false;
                sectionDeleted = true; // Mark global deletion complete
            }
            if (!deleteGlobal && strcmp(sectionName, section) == 0)
            {
                inTargetSection = true;
                sectionDeleted = true;
                continue; // Skip writing this section
            }
        }

        if (!inTargetSection && !(deleteGlobal && sectionDeleted))
        {
            fprintf(tempFile, "%s\n", line);
        }
    }

    fclose(file);

    if (!sectionDeleted && !deleteGlobal)
    {
        fclose(tempFile);
        return false;
    }

    file = fopen(filename, "w");
    if (!file)
    {
        fclose(tempFile);
        return false;
    }

    rewind(tempFile);
    while (fgets(line, sizeof(line), tempFile))
    {
        fprintf(file, "%s", line);
    }

    fclose(tempFile);
    fclose(file);

    return true;
}

pointerVector<char*>* IniUtility::GetSectionNames(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        return new pointerVector<char*>(false);
    }

    char line[MAX_LINE_SIZE];
    pointerVector<char*>* sections = new pointerVector<char*>(false);

    while (fgets(line, sizeof(line), file))
    {
        char sectionName[256];
        if (isSection(trim(line), sectionName))
        {
            char* section = strdup(sectionName);
            sections->add(section);
        }
    }

    fclose(file);
    return sections;
}

pointerVector<char*>* IniUtility::GetSectionKeys(const char* filename, const char* section)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        return new pointerVector<char*>(false);
    }

    char line[MAX_LINE_SIZE], currentSection[256];
    pointerVector<char*>* keys = new pointerVector<char*>(false);
    bool inGlobalSection = (section == NULL || strlen(section) == 0);

    while (fgets(line, sizeof(line), file))
    {
        char* trimmedLine = trim(line);
        if (*trimmedLine == '\0' || *trimmedLine == ';') continue;

        if (isSection(trimmedLine, currentSection))
        {
            inGlobalSection = false;
            if (strcmp(currentSection, section) == 0)
            {
                inGlobalSection = true;
            }
            continue;
        }

        if (inGlobalSection || strcmp(currentSection, section) == 0)
        {
            char key[256], value[MAX_VALUE_SIZE];
            bool hasSpaces;
            if (isKeyValuePair(trimmedLine, key, value, hasSpaces))
            {
                char* keyCStr = strdup(key);
                keys->add(keyCStr);
            }
        }
    }

    fclose(file);
    return keys;
}