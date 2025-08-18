#include "framework.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>

void print(const std::string& text) {
    std::cout << text << std::endl;
}

DWORD FindProcessId(const std::wstring& processName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    DWORD pid = 0;

    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName.c_str()) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32NextW(hSnap, &pe));
    }

    CloseHandle(hSnap);
    return pid;
}

int main() {
    print("Searching game process...");
    std::wstring target = L"gmod.exe";
    DWORD pid = FindProcessId(target);

    while (pid == 0) {
        print("Waiting for game...");
        std::this_thread::sleep_for(std::chrono::seconds(10));
        DWORD pid = FindProcessId(target);
        if (pid != 0) {
            break;
        }
    }
    print("Process found! PID: " + std::to_string(pid));

    HANDLE game = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if (game == NULL) {
        print("Failed to open process.");
        system("pause");
        return 1;
    }

    print("Process opened successfully.");

    print("\n[Memory Test] Allocating memory in remote process...");
    SIZE_T sizeToAllocate = 8192; // two pages
    LPVOID remoteAddr = VirtualAllocEx(game, nullptr, sizeToAllocate, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remoteAddr) {
        std::string testString = "All is working!";
        print("[Memory Test] Writing payload...");
        WriteProcessMemory(game, remoteAddr, testString.c_str(), testString.size() + 1, nullptr);

        char buffer[256] = { 0 };
        ReadProcessMemory(game, remoteAddr, buffer, sizeof(buffer) - 1, nullptr);
        print("[Memory Test] Read back: " + std::string(buffer));

        VirtualFreeEx(game, remoteAddr, 0, MEM_RELEASE);
    }
    else {
        print("[Memory Test] Failed to allocate memory.");
    }

    print("\n--- Starting Pattern Scan ---");

    uintptr_t moduleBase = 0;
    uintptr_t moduleSize = 0;

    if (!GetModuleInfo(pid, L"client.dll", moduleBase, moduleSize)) {
        std::cerr << "Failed to find module client.dll" << std::endl;
        CloseHandle(game);
        system("pause");
        return 1;
    }
    std::cout << "Module client.dll found at: 0x" << std::hex << moduleBase << " with size: " << moduleSize << std::dec << std::endl;

    if (patterns::b_connected() == false) {
        print("Server not found");
        return 1;
    }

    uintptr_t entityListAddr = patterns::GetEntityList(game, moduleBase, moduleSize);
    if (!entityListAddr) {
        std::cerr << "EntityList not found via pattern scan." << std::endl;
        CloseHandle(game);
        system("pause");
        return 1;
    }

    std::cout << "EntityList found at address: 0x" << std::hex << entityListAddr << std::dec << std::endl;

    print("\n--- Reading Player Data ---");
    auto players = patterns::GetPlayers(game, entityListAddr, 131);

    if (players.empty()) {
        print("No players found or failed to read data.");
    }
    else {
        print("Players found: " + std::to_string(players.size()));
        for (const auto& p : players) {
            std::cout << "Player -> "
                << "HP: " << p.hp
                << ", Armor: " << p.armor
                << ", Pos: (" << p.pos.x << ", " << p.pos.y << ", " << p.pos.z << ")"
                << std::endl;
        }
    }

    print("\n--- Finished ---");
    CloseHandle(game);
    //system("pause");
    return 0;
}