#include <stdio.h>
#include <Windows.h>


int main() {
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);

    // 1.
    DWORD pageSize = systemInfo.dwPageSize;
    printf("Page Size: %u bytes\n", pageSize);

    // 2.
    LPVOID memoryBlock = VirtualAlloc(NULL, 1 << 20, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    // 3.
    MEMORY_BASIC_INFORMATION memoryInfo;
    VirtualQuery(memoryBlock, &memoryInfo, sizeof(memoryInfo));
    DWORD memoryState = memoryInfo.State;

    // 4.
    if (memoryState == MEM_COMMIT) {
        printf("Memory State: Commited\n");
    }
    else if (memoryState == MEM_RESERVE) {
        printf("Memory State: Reserved\n");
    }
    else if (memoryState == MEM_FREE) {
        printf("Memory State: Free\n");
    }
    else {
        printf("Memory State: Undetermined\n");
    }

    // 5.
    VirtualFree(memoryBlock, 0, MEM_RELEASE);

    // 6.
    VirtualQuery(memoryBlock, &memoryInfo, sizeof(memoryInfo));
    memoryState = memoryInfo.State;
    if (memoryState == MEM_COMMIT) {
        printf("Memory State: Commited\n");
    }
    else if (memoryState == MEM_RESERVE) {
        printf("Memory State: Reserved\n");
    }
    else if (memoryState == MEM_FREE) {
        printf("Memory State: Free\n");
    }
    else {
        printf("Memory State: Undetermined\n");
    }

    return 0;
}
