#include "framework.h"
#include <vector>
#include <iostream>
#include <psapi.h>
#include <cstring>

HANDLE gmodHandle = nullptr;
uintptr_t clientBase = 0;
uintptr_t clientSize = 0;
uintptr_t m_bConnected_addr = 0;

bool GetModuleInfo(DWORD pid, const wchar_t* moduleName, uintptr_t& base, uintptr_t& size) {
    base = 0;
    size = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnap == INVALID_HANDLE_VALUE)
        return false;

    MODULEENTRY32W modEntry;
    modEntry.dwSize = sizeof(modEntry);

    if (Module32FirstW(hSnap, &modEntry)) {
        do {
            if (_wcsicmp(modEntry.szModule, moduleName) == 0) {
                base = reinterpret_cast<uintptr_t>(modEntry.modBaseAddr);
                size = static_cast<uintptr_t>(modEntry.modBaseSize);
                CloseHandle(hSnap);
                return true;
            }
        } while (Module32NextW(hSnap, &modEntry));
    }

    CloseHandle(hSnap);
    return false;
}

namespace patterns {

    const char* entlist_pattern = "\x48\x03\x3D\x00\x00\x00\x00";
    const char* entlist_mask = "xxx????";
    SIZE_T entsz = 0x10;
    SIZE_T hp_offs = 0xC8;
    SIZE_T armor_offs = 0xCC;
    SIZE_T pos_offs = 0x308;
    SIZE_T ang_offs = 0x1E4;

    const char* m_bconnected_pattern = "\xB8\xDC\x20\x00\x00\x48\x8D\x15\x00\x00\x00\x00\x48\x8D\x4D\x00\xE8\x00\x00\x00\x00";
    const char* m_bconnected_mask = "xxxxxxx????xxx?x????";
}

bool DataCompare(const BYTE* data, const BYTE* pattern, const char* mask) {
    for (; *mask; ++mask, ++data, ++pattern) {
        if (*mask == 'x' && *data != *pattern) {
            return false;
        }
    }
    return (*mask) == '\0';
}

uintptr_t patterns::FindPattern(HANDLE hProcess, uintptr_t start, uintptr_t end, const char* pattern, const char* mask) {
    size_t regionSize = end - start;
    std::vector<BYTE> buffer(regionSize);

    SIZE_T bytesRead;
    if (!ReadProcessMemory(hProcess, (LPCVOID)start, buffer.data(), regionSize, &bytesRead) || bytesRead == 0) {
        return 0;
    }

    size_t patternLen = strlen(mask);
    for (size_t i = 0; i < bytesRead - patternLen; ++i) {
        if (DataCompare(buffer.data() + i, reinterpret_cast<const BYTE*>(pattern), mask)) {
            uintptr_t instruction_address = start + i;
            DWORD offset = 0;
            if (ReadProcessMemory(hProcess, (LPCVOID)(instruction_address + 3), &offset, sizeof(offset), nullptr)) {
                return instruction_address + offset + 7;
            }
            return 0;
        }
    }
    return 0;
}

uintptr_t patterns::GetEntityList(HANDLE hProcess, uintptr_t moduleBase, uintptr_t moduleSize) {
    return FindPattern(hProcess, moduleBase, moduleBase + moduleSize, entlist_pattern, entlist_mask);
}

std::vector<ply> patterns::GetPlayers(HANDLE hProcess, uintptr_t entityListAddr, int maxEntities) {
    std::vector<ply> players;

    uintptr_t entityListBase = 0;
    if (!ReadProcessMemory(hProcess, (LPCVOID)entityListAddr, &entityListBase, sizeof(entityListBase), nullptr) || entityListBase == 0) {
        return players;
    }

    for (int i = 0; i < maxEntities; i++) {
        uintptr_t entAddr = 0;
        uintptr_t currentEntPtr = entityListBase + i * entsz;

        if (!ReadProcessMemory(hProcess, (LPCVOID)currentEntPtr, &entAddr, sizeof(entAddr), nullptr) || entAddr == 0) {
            continue;
        }

        int hp = 0;
        int armor = 0;
        Vector3 pos, ang;

        ReadProcessMemory(hProcess, (LPCVOID)(entAddr + hp_offs), &hp, sizeof(hp), nullptr);
        ReadProcessMemory(hProcess, (LPCVOID)(entAddr + armor_offs), &armor, sizeof(armor), nullptr);
        ReadProcessMemory(hProcess, (LPCVOID)(entAddr + pos_offs), &pos, sizeof(pos), nullptr);
        ReadProcessMemory(hProcess, (LPCVOID)(entAddr + ang_offs), &ang, sizeof(ang), nullptr);

        if (hp > 0) {
            ply p(
                "",
                false,
                hp,
                armor,
                "",
                pos,
                ang,
                {},
                {}
            );
            players.push_back(p);
        }
    }

    return players;
}

bool patterns::b_connected() {
    if (!gmodHandle || !m_bConnected_addr)
        return false;

    bool result = false;
    if (ReadProcessMemory(gmodHandle, (LPCVOID)m_bConnected_addr, &result, sizeof(result), nullptr)) {
        return result;
    }
    return false;
}