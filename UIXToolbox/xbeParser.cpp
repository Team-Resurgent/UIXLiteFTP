#include "xbeParser.h"
#include <cstdio>
#include <cstring>

XBEParser::XBEParser() : filepath(NULL), xbe_size(0), sections(true) {}

XBEParser::~XBEParser() {
	sections.clear();
	if (filepath) {
        FreePath();
    }
}

bool XBEParser::LoadXBE(const char* path) {
	// Free any previously allocated filepath
    if (filepath) {
        FreePath(); // Reset state
    }
	
    filepath = strdup(path);
	if (!filepath) {
        return false;  // strdup failed
    }
	
	FILE* file = fopen(filepath, "rb");
    if (!file) {
		FreePath(); // Reset state
        return false;
    }
	
    // Move to the end of the file to determine the size
    fseek(file, 0, SEEK_END);
    xbe_size = static_cast<size_t>(ftell(file));
    rewind(file);

    if (xbe_size < sizeof(XBE_HEADER)) {
		fclose(file);
        FreePath(); // Reset state
        return false;
    }
	
    if (fread(&header, 1, sizeof(XBE_HEADER), file) != sizeof(XBE_HEADER)) {
		fclose(file);
        FreePath(); // Reset state
        return false;
    }

    // Verify XBE magic number
    if (header.magic != 0x48454258) { // "XBEH" in hexadecimal
		fclose(file);
        FreePath(); // Reset state
        return false;
    }
	
	if (Offset(header.certificate_address) >= xbe_size) {
		fclose(file);
        FreePath();  // Reset state
        return false;
    }
	
	fseek(file,Offset(header.certificate_address),SEEK_SET);
    if (fread(&certificate, 1, sizeof(XBE_CERTIFICATE), file) != sizeof(XBE_CERTIFICATE)) {
		fclose(file);
        FreePath(); // Reset state
        return false;
    }
	
    if (Offset(header.section_headers_address) >= xbe_size) {
		fclose(file);
        FreePath(); // Reset state
        return false;
    }
	
	fseek(file,Offset(header.section_headers_address),SEEK_SET);
	sections.clear();

    for (uint32_t i = 0; i < header.number_of_sections; ++i) {
        uint32_t section_offset = Offset(header.section_headers_address) + (i * sizeof(XBE_SECTION));

        if (section_offset + sizeof(XBE_SECTION) > xbe_size) {
			fclose(file);
            FreePath(); // Reset state
			return false;
        }

        // Dynamically allocate a new XBE_SECTION
        XBE_SECTION* section = new XBE_SECTION;
        if (!section) {
            fclose(file);
            FreePath(); // Reset state
			return false; // Handle memory allocation failure
        }
		fseek(file, section_offset, SEEK_SET);
        if (fread(section, 1, sizeof(XBE_SECTION), file) != sizeof(XBE_SECTION)) {
			delete section; // Clean up the allocated memory
			sections.clear();
            fclose(file);
			FreePath(); // Reset state
			return false;
		}
        // Add the section to the pointerVector
        sections.add(section);
    }
	
    fclose(file);
    return true;
}

bool XBEParser::GetSectionByName(const char* name, XBE_SECTION& section) {
    if (!filepath) {
        return false;  // Ensure LoadXBE was successful
    }
	
	FILE* file = fopen(filepath, "rb");
    if (!file) {
        return false;
    }
	
	for (size_t i = 0; i < sections.count(); ++i) {
        XBE_SECTION* sec = sections.get(i);
        char section_name[256];
		if (Offset(sec->section_name_address) >= xbe_size) {
            continue;
        }

		fseek(file,Offset(sec->section_name_address),SEEK_SET);
		
		// Read the section name from the file
        if (fread(section_name, 1, sizeof(section_name), file) != sizeof(section_name)) {
			fclose(file);
			return false;
		}
		
        section_name[sizeof(section_name) - 1] = '\0';  // Ensure null-termination
		
        // Compare the read section name with the provided name
        if (strcmp(section_name, name) == 0) {
            section = *sec; // Copy the section data to the output parameter
            fclose(file);
			return true;
        }
    }
	fclose(file);
    return false;
}

bool XBEParser::GetTitleID(uint32_t& title_id) {
    if (!filepath) {
        return false;  // Ensure LoadXBE was successful
    }
    
    title_id = certificate.title_id;
    return true;
}

bool XBEParser::GetTitleName(char*& titlename) {
    if (!filepath) {
        return false;  // Ensure LoadXBE was successful
    }

    titlename = new char[TITLE_LEN + 1];
    for (int i = 0; i < TITLE_LEN; ++i) {
        titlename[i] = (char)(certificate.title_name[i] & 0xFF);
    }
    titlename[TITLE_LEN] = '\0';

    return true;
}

bool XBEParser::HasTitleImage(){
    XBE_SECTION section = {0};
    return GetSectionByName("$$XTIMAGE", section);
}

bool XBEParser::GetTitleImage(uint8_t*& image_data, size_t& image_size) {
    if (!filepath) {
        return false;  // Ensure LoadXBE was successful
    }

    XBE_SECTION section = {0};

    if (!GetSectionByName("$$XTIMAGE", section)) {
        return false;
    }

    // Check if the raw address is within bounds
    if (section.raw_address > xbe_size || section.raw_size > xbe_size - section.raw_address) {
        return false;
    }

    // Free any previously allocated image_data
    if (image_data != NULL) {
        delete[] image_data;
        image_data = NULL;
    }

    // Allocate new memory for image_data
    image_data = new uint8_t[section.raw_size];
    if (!image_data) {
        return false; // Allocation failed
    }

    // Set the returned image size
	image_size = section.raw_size;
    
	FILE* file = fopen(filepath, "rb");
    if (!file) {
		delete[] image_data;
		image_data = NULL;
        return false;

    }
	
    fseek(file,section.raw_address,SEEK_SET);
		
	// Read the image data from the section's raw address
	if (fread(image_data, 1, section.raw_size, file) != section.raw_size) {
		delete[] image_data;
		image_data = NULL;
		fclose(file);
		return false;
	}

    fclose(file);
    return true;
}

bool XBEParser::SaveTitleImage(const char* path) {
    uint8_t* image_data = NULL;
    size_t image_size = 0;

    if (!GetTitleImage(image_data, image_size)) {
        return false;
    }

    if (!path || !image_data || image_size == 0) {
        delete[] image_data;
        return false;
    }

    FILE* file = fopen(path, "wb");
    if (!file) {
        delete[] image_data;
        return false;
    }

    size_t written = fwrite(image_data, 1, image_size, file);
    fclose(file);
    delete[] image_data;
    return written == image_size;
}
