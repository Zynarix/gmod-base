#include "framework.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>


int main() {
    patterns::Initialize();


    patterns::Shutdown();
    return 0;
}