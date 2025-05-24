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
        while (end > start && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
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
    bool inGlobalSection = isNullOrEmpty(section);

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
    FILE* file = fopen(filename, "rb");
    if (!file) return false;

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory to read the entire file
    char* fileContent = new char[fileSize + 1]; // +1 for null terminator
    if (!fileContent)
    {
        fclose(file);
        return false;
    }

    // Read the entire file into memory
    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0'; // Null terminate the string
    fclose(file);

    // Prepare buffer for the new content
    char* newContent = new char[fileSize + 1280]; // Allocate some extra space for new content
    if (!newContent)
    {
        delete[] fileContent;
        return false;
    }
    newContent[0] = '\0'; // Start with an empty string

    char line[MAX_LINE_SIZE], currentSection[256], previousSection[256] = "";
    char keyStr[256], valueStr[MAX_VALUE_SIZE];
    strcpy(keyStr, key);
    strcpy(valueStr, value);
    bool keyUpdated = false;
    bool inGlobalSection = true;
    bool firstFormatDetected = false;
    bool useSpaces = false; // Determines if spaces should be used around '='
    char* currentLine = strtok(fileContent, "\n");
	
    while (currentLine)
    {
        strncpy(line, currentLine, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0'; // Ensure null-termination
        char* trimmedLine = trim(line);

        if (*trimmedLine == '\0' || *trimmedLine == ';')
        {
            strcat(newContent, line);
            strcat(newContent, "\n");
            currentLine = strtok(NULL, "\n");
            continue;
        }

        char foundKey[256], foundValue[MAX_VALUE_SIZE];
        bool lineHasSpaces;
        if (isSection(trimmedLine, currentSection))
        {
            if (!keyUpdated && strcmp(previousSection, section) == 0)
            {
                strcat(newContent, keyStr);
                strcat(newContent, useSpaces ? " = " : "=");
                strcat(newContent, valueStr);
                strcat(newContent, "\n");
                keyUpdated = true;
            }
            strcpy(previousSection, currentSection);
            strcat(newContent, line);
            strcat(newContent, "\n");
            inGlobalSection = false;
            currentLine = strtok(NULL, "\n");
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
                strcat(newContent, keyStr);
                strcat(newContent, useSpaces ? " = " : "=");
                strcat(newContent, valueStr);
                strcat(newContent, "\n");
                keyUpdated = true;
                currentLine = strtok(NULL, "\n");
                continue;
            }
        }

        strcat(newContent, line);
        strcat(newContent, "\n");
        currentLine = strtok(NULL, "\n");
    }
    if (!keyUpdated)
    {
        // Add the new section and key-value if it wasn't found and updated
        if ((!inGlobalSection && strcmp(currentSection, section) != 0) 
			|| (inGlobalSection && !isNullOrEmpty(section)))
        {
            strcat(newContent, "[");
            strcat(newContent, section);
            strcat(newContent, "]\n");
        }
        strcat(newContent, keyStr);
        strcat(newContent, useSpaces ? " = " : "=");
        strcat(newContent, valueStr);
        strcat(newContent, "\n");
    }

    // Open the file again for writing
    file = fopen(filename, "wb");
    if (!file)
    {
        delete[] fileContent;
        delete[] newContent;
        return false;
    }

    // Write the updated content to the file
    fwrite(newContent, sizeof(char), strlen(newContent), file);

    // Clean up
    fclose(file);
    delete[] fileContent;
    delete[] newContent;

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
    bool inGlobalSection = isNullOrEmpty(section);

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
    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        return false;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory to read the entire file
    char* fileContent = new char[fileSize + 1];
    if (!fileContent)
    {
        fclose(file);
        return false;
    }

    // Read the entire file into memory
    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0'; // Null terminate the string
    fclose(file);

    // Prepare buffer for the new content
    char* newContent = new char[fileSize + 1];
    if (!newContent)
    {
        delete[] fileContent;
        return false;
    }
    newContent[0] = '\0'; // Start with an empty string

    char line[MAX_LINE_SIZE], currentSection[256];
    bool keyDeleted = false;
    bool inGlobalSection = isNullOrEmpty(section);

    char* currentLine = strtok(fileContent, "\n");
    while (currentLine)
    {
        strncpy(line, currentLine, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0'; // Ensure null-termination
        char* trimmedLine = trim(line);

        if (*trimmedLine == '\0' || *trimmedLine == ';')
        {
            strcat(newContent, line);
            strcat(newContent, "\n");
            currentLine = strtok(NULL, "\n");
            continue;
        }

        char foundKey[256], foundValue[MAX_VALUE_SIZE];
        bool hasSpaces;
        if (isSection(trimmedLine, currentSection))
        {
            strcat(newContent, line);
            strcat(newContent, "\n");
            inGlobalSection = false;
            currentLine = strtok(NULL, "\n");
            continue;
        }

        if ((inGlobalSection || strcmp(currentSection, section) == 0) && isKeyValuePair(trimmedLine, foundKey, foundValue, hasSpaces))
        {
            if (strcmp(foundKey, key) == 0)
            {
                keyDeleted = true;
                currentLine = strtok(NULL, "\n");
                continue; // Skip writing this line to delete the key
            }
        }

        strcat(newContent, line);
        strcat(newContent, "\n");
        currentLine = strtok(NULL, "\n");
    }

    if (!keyDeleted)
    {
        delete[] fileContent;
        delete[] newContent;
        return false;
    }

    // Open the file again for writing
    file = fopen(filename, "wb");
    if (!file)
    {
        delete[] fileContent;
        delete[] newContent;
        return false;
    }

    // Write the updated content to the file
    fwrite(newContent, sizeof(char), strlen(newContent), file);

    // Clean up
    fclose(file);
    delete[] fileContent;
    delete[] newContent;

    return true;
}

bool IniUtility::DeleteSection(const char* filename, const char* section)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        return false;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory to read the entire file
    char* fileContent = new char[fileSize + 1];
    if (!fileContent)
    {
        fclose(file);
        return false;
    }

    // Read the entire file into memory
    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0'; // Null terminate the string
    fclose(file);

    // Prepare buffer for the new content
    char* newContent = new char[fileSize + 1];
    if (!newContent)
    {
        delete[] fileContent;
        return false;
    }
    newContent[0] = '\0'; // Start with an empty string

    char line[MAX_LINE_SIZE], currentSection[256];
    bool sectionDeleted = false;
    bool inTargetSection = false;
    bool deleteGlobal = isNullOrEmpty(section);

    char* currentLine = strtok(fileContent, "\n");
    while (currentLine)
    {
        strncpy(line, currentLine, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0'; // Ensure null-termination
        char* trimmedLine = trim(line);

        if (*trimmedLine == '\0' || *trimmedLine == ';')
        {
            if (!inTargetSection)
            {
                strcat(newContent, line);
                strcat(newContent, "\n");
            }
            currentLine = strtok(NULL, "\n");
            continue;
        }

        char sectionName[256];
        if (isSection(trimmedLine, sectionName))
        {
            if (inTargetSection)
            {
                inTargetSection = false;
                sectionDeleted = true; // Mark section deletion complete
            }
            if (!deleteGlobal && strcmp(sectionName, section) == 0)
            {
                inTargetSection = true;
                sectionDeleted = true;
                currentLine = strtok(NULL, "\n");
                continue; // Skip writing this section
            }
        }

        if (!inTargetSection)
        {
            strcat(newContent, line);
            strcat(newContent, "\n");
        }
        currentLine = strtok(NULL, "\n");
    }

    if (!sectionDeleted && !deleteGlobal)
    {
        delete[] fileContent;
        delete[] newContent;
        return false;
    }

    // Open the file again for writing
    file = fopen(filename, "wb");
    if (!file)
    {
        delete[] fileContent;
        delete[] newContent;
        return false;
    }

    // Write the updated content to the file
    fwrite(newContent, sizeof(char), strlen(newContent), file);

    // Clean up
    fclose(file);
    delete[] fileContent;
    delete[] newContent;

    return true;
}