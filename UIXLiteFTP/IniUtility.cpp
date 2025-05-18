#include "IniUtility.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include "pointerVector.h"

using namespace std;

namespace {
    string trim(const string& str)
	{
        size_t first = str.find_first_not_of(" \t");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t");
        return str.substr(first, last - first + 1);
    }

    bool isSection(const string& line, string& sectionName)
	{
        if (!line.empty() && line[0] == '[' && line[line.length() - 1] == ']')
		{
            sectionName = line.substr(1, line.size() - 2);
            return true;
        }
        return false;
    }

    bool isKeyValuePair(const string& line, string& key, string& value, bool& hasSpaces)
	{
        size_t equalPos = line.find('=');
        if (equalPos != string::npos)
		{
            key = trim(line.substr(0, equalPos));
            value = trim(line.substr(equalPos + 1));
            hasSpaces = (line.find(" = ") != string::npos);
            return true;
        }
        return false;
    }
}

char* IniUtility::GetValue(const char* filename, const char* section, const char* key)
{
    ifstream file(filename);
    if (!file)
	{
        return strdup("");
    }

    string line, currentSection;
    string keyStr = key;
    bool inGlobalSection = (section == NULL || strlen(section) == 0);

    while (getline(file, line))
	{
        line = trim(line);
        if (line.empty() || line[0] == ';') continue;

        if (isSection(line, currentSection))
		{
            inGlobalSection = false;
            if (currentSection == section)
			{
                inGlobalSection = true;
            }
            continue;
        }

        string foundKey, foundValue;
        bool hasSpaces;
        if (isKeyValuePair(line, foundKey, foundValue, hasSpaces))
		{
            if ((inGlobalSection || currentSection == section) && foundKey == key)
			{
                char* result = new char[foundValue.length() + 1];
                strcpy(result, foundValue.c_str());
                return result;
            }
        }
    }
    return strdup("");
}

bool IniUtility::SetValue(const char* filename, const char* section, const char* key, const char* value)
{
    ifstream file(filename);
    if (!file.is_open())
	{
        return false;
    }

    ostringstream newContent;
    string line, currentSection, previousSection;
    string keyStr = key;
    string valueStr = value;
    bool keyUpdated = false;
    bool inGlobalSection = true;
	bool firstFormatDetected = false;
    bool useSpaces = false; // Determines if spaces should be used around '='

    while (getline(file, line))
	{
        string trimmedLine = trim(line);
        if (trimmedLine.empty() || trimmedLine[0] == ';')
		{
            newContent << line << "\n";
            continue;
        }

        string foundKey, foundValue;
        bool lineHasSpaces;
        if (isSection(trimmedLine, currentSection))
		{
			if (!keyUpdated && previousSection == section) {
				// Update the existing key-value pair
                newContent << keyStr << (useSpaces ? " = " : "=") << valueStr << "\n";
                keyUpdated = true;
			}
			previousSection = currentSection;
			newContent << line << "\n";
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

            if ((inGlobalSection || currentSection == section) && foundKey == keyStr)
			{
                // Update the existing key-value pair
                newContent << keyStr << (useSpaces ? " = " : "=") << valueStr << "\n";
                keyUpdated = true;
                continue;
            }
        }

        newContent << line << "\n";
    }

    if (!keyUpdated)
	{
        if (!inGlobalSection && currentSection != section)
		{
            newContent << "[" << section << "]\n";
        }
        newContent << keyStr << (useSpaces ? " = " : "=") << valueStr << "\n";
    }

    file.close();

    ofstream outFile(filename);
    if (!outFile)
	{
        return false;
    }
    outFile << newContent.str();
    outFile.close();

    return true;
}

pointerVector<char*>* IniUtility::GetSectionNames(const char* filename)
{
    ifstream file(filename);
    if (!file)
	{
        return new pointerVector<char*>(false);
    }

    string line;
    pointerVector<char*>* sections = new pointerVector<char*>(false);

    while (getline(file, line))
	{
        string sectionName;
        if (isSection(trim(line), sectionName))
		{
            char* section = new char[sectionName.length() + 1];
            strcpy(section, sectionName.c_str());
            sections->add(section);
        }
    }

    file.close();
    return sections;
}

pointerVector<char*>* IniUtility::GetSectionKeys(const char* filename, const char* section)
{
    ifstream file(filename);
    if (!file)
	{
        return new pointerVector<char*>(false);
    }

    string line, currentSection;
    pointerVector<char*>* keys = new pointerVector<char*>(false);
	bool inGlobalSection = (section == NULL || strlen(section) == 0);

    while (getline(file, line))
	{
        string trimmedLine = trim(line);
        if (trimmedLine.empty() || trimmedLine[0] == ';') continue;

        if (isSection(trimmedLine, currentSection))
		{
            inGlobalSection = false;
            if (currentSection == section)
			{
                inGlobalSection = true;
            }
            continue;
        }

        if (inGlobalSection || currentSection == section)
		{
            string key, value;
            bool hasSpaces;
            if (isKeyValuePair(trimmedLine, key, value, hasSpaces))
			{
                char* keyCStr = new char[key.length() + 1];
                strcpy(keyCStr, key.c_str());
                keys->add(keyCStr);
            }
        }
    }

    file.close();
    return keys;
}

bool IniUtility::DeleteKey(const char* filename, const char* section, const char* key)
{
    ifstream file(filename);
    if (!file.is_open())
	{
        return false;
    }

    ostringstream newContent;
    string line, currentSection;
    string keyStr = key;
    bool keyDeleted = false;
    bool inGlobalSection = (section == NULL || strlen(section) == 0);

    while (getline(file, line))
	{
        string trimmedLine = trim(line);
        if (trimmedLine.empty() || trimmedLine[0] == ';')
		{
            newContent << line << "\n";
            continue;
        }

        string foundKey, foundValue;
        bool hasSpaces;
        if (isSection(trimmedLine, currentSection))
		{
            newContent << line << "\n";
            inGlobalSection = false;
            continue;
        }

        if ((inGlobalSection || currentSection == section) && isKeyValuePair(trimmedLine, foundKey, foundValue, hasSpaces))
		{
            if (foundKey == keyStr)
			{
                keyDeleted = true;
                continue;
            }
        }

        newContent << line << "\n";
    }

    file.close();

    if (!keyDeleted)
	{
        return false;
    }

    ofstream outFile(filename);
    if (!outFile)
	{
        return false;
    }
    outFile << newContent.str();
    outFile.close();

    return true;
}

bool IniUtility::DeleteSection(const char* filename, const char* section)
{
    ifstream file(filename);
    if (!file.is_open())
	{
        return false;
    }

    ostringstream newContent;
    string line, currentSection;
    bool sectionDeleted = false;
    bool inTargetSection = false;
    bool deleteGlobal = (section == NULL || strlen(section) == 0);

    while (getline(file, line))
	{
        string trimmedLine = trim(line);

        if (trimmedLine.empty() || trimmedLine[0] == ';')
		{
            if (!inTargetSection)
			{
                newContent << line << "\n";
            }
            continue;
        }

        string sectionName;
        if (isSection(trimmedLine, sectionName))
		{
            if (inTargetSection || (deleteGlobal && !sectionDeleted))
			{
                inTargetSection = false;
                sectionDeleted = true; // Mark global deletion complete
            }
            if (!deleteGlobal && sectionName == section)
			{
                inTargetSection = true;
                sectionDeleted = true;
                continue;
            }
        }

        if (!inTargetSection && !(deleteGlobal && sectionDeleted))
		{
            newContent << line << "\n";
        }
    }

    file.close();

    if (!sectionDeleted && !deleteGlobal)
	{
        return false;
    }

    ofstream outFile(filename);
    if (!outFile)
	{
        return false;
    }
    outFile << newContent.str();
    outFile.close();

    return true;
}