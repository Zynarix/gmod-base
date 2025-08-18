#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <cstdint>
#include "sdk.h"

bool GetModuleInfo(DWORD pid, const wchar_t* moduleName, uintptr_t& base, uintptr_t& size);

namespace patterns {
    extern const char* entlist_pattern;
    extern const char* entlist_mask;
    bool b_connected();
    //std::string s_serverip();

    // Offsests and sizes
    extern SIZE_T entsz;
    extern SIZE_T hp_offs;
    extern SIZE_T armor_offs;
    extern SIZE_T pos_offs;
    extern SIZE_T ang_offs;

    uintptr_t FindPattern(HANDLE hProcess, uintptr_t start, uintptr_t end, const char* pattern, const char* mask);

    uintptr_t GetEntityList(HANDLE hProcess, uintptr_t moduleBase, uintptr_t moduleSize);

    std::vector<ply> GetPlayers(HANDLE hProcess, uintptr_t entityListAddr, int maxEntities);
}