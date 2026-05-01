// Compatibility stub for NFD (file dialog library) - not used on Linux
#ifndef NFD_NFD_H
#define NFD_NFD_H

// Stub types for Linux compatibility
typedef int nfdresult_t;
typedef const char nfdchar_t;

#define NFD_OKAY 0
#define NFD_CANCEL 1
#define NFD_ERROR 2

// Stub functions for Linux compatibility
static inline nfdresult_t NFD_OpenDialog(const char* filterList, const char* defaultPath, nfdchar_t** outPath) {
    return NFD_CANCEL;
}

static inline void NFD_Free(void* ptr) {
}

#endif
