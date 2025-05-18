#include "xbeParse.h"
#include <xtl.h>
#include <stdio.h>
#include <string.h>

bool parseXBE(const char* filepath, char* titleNameOut, uint32_t* titleIdOut) {
    FILE* file = fopen(filepath, "rb");
    if (!file)
        return false;

    uint8_t xbeData[8192];
    size_t bytesRead = fread(xbeData, 1, sizeof(xbeData), file);
    if (bytesRead < sizeof(XBEHeader)) {
        fclose(file);
        return false;
    }

    XBEHeader* header = (XBEHeader*)xbeData;
    if (header->magic != XBE_MAGIC) {
        fclose(file);
        return false;
    }

    uint32_t certOffset = header->certificate_address - header->base_address;
    if (certOffset + sizeof(XBECertificate) > bytesRead) {
        fclose(file);
        return false;
    }

    XBECertificate* cert = (XBECertificate*)(xbeData + certOffset);

    if (titleIdOut)
        *titleIdOut = cert->title_id;

    if (titleNameOut) {
        for (int i = 0; i < 40; ++i) {
            uint16_t wc = cert->title_name[i];
            if (wc == 0) {
                titleNameOut[i] = '\0';
                break;
            }

            titleNameOut[i] = (wc >= 0x20 && wc <= 0x7E) ? (char)wc : '?';
            titleNameOut[i + 1] = '\0';
        }
    }

    fclose(file);
    return true;
}
