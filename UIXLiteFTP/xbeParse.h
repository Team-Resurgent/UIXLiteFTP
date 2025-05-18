#ifndef XBE_PARSE_H
#define XBE_PARSE_H

#include "xboxinternals.h"

#define XBE_MAGIC 0x48454258  // 'XBEH' 

#pragma pack(push, 1)

typedef struct {
    uint32_t magic;
    uint8_t signature[256];
    uint32_t base_address;
    uint32_t size_of_headers;
    uint32_t size_of_image;
    uint32_t size_of_image_header;
    uint32_t timestamp;
    uint32_t certificate_address;
    uint32_t number_of_sections;
    uint32_t section_headers_address;
    uint32_t init_flags;
    uint32_t entry_point;
    uint32_t tls_address;
    uint32_t pe_stack_commit;
    uint32_t pe_heap_reserve;
    uint32_t pe_heap_commit;
    uint32_t pe_base_address;
    uint32_t pe_size_of_image;
    uint32_t pe_checksum;
    uint32_t pe_timestamp;
    uint32_t debug_pathname_address;
    uint32_t debug_filename_address;
    uint32_t debug_unicode_filename_address;
    uint32_t kernel_image_thunk_address;
    uint32_t nonkernel_import_dir_address;
    uint32_t library_versions_count;
    uint32_t library_versions_address;
    uint32_t kernel_library_version_address;
    uint32_t xapi_library_version_address;
    uint32_t logo_bitmap_address;
    uint32_t logo_bitmap_size;
} XBEHeader;

typedef struct {
    uint32_t size;
    uint32_t timestamp;
    uint32_t title_id;
    uint16_t title_name[40];
    uint32_t alt_title_ids[16];
    uint32_t allowed_media;
    uint32_t game_region;
    uint32_t game_ratings;
    uint32_t disk_number;
    uint32_t version;
    uint8_t lan_key[16];
    uint8_t signature_key[16];
    uint8_t alt_signature_keys[16][16];
} XBECertificate;

#pragma pack(pop)

bool parseXBE(const char* filepath, char* titleNameOut, uint32_t* titleIdOut);

#endif // XBE_PARSE_H
