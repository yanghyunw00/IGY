# Memory Leak Detection Guide

This project includes multiple memory leak detection options for Windows.

## 1. CRT Debug Heap (Built-in, Windows only)

### Description
Uses Microsoft's C Runtime Debug Heap to detect memory leaks. This is automatically enabled in Debug builds on Windows.

### How to Use
1. Build in **Debug** mode:
   ```bash
   cmake -B build/windows-debug -DCMAKE_BUILD_TYPE=Debug
   cmake --build build/windows-debug --config Debug
   ```

2. Run the executable:
   ```bash
   ./build/windows-debug/Debug/cs200_fun.exe
   ```

3. Check the **Output Window** in Visual Studio or the console for memory leak reports at program exit.

### Output Example
```
Detected memory leaks!
Dumping objects ->
{156} normal block at 0x00000123456789AB, 64 bytes long.
 Data: <                > CD CD CD CD CD CD CD CD
Object dump complete.
```

### Setting a Breakpoint on Allocation
If you see a leak at allocation `{156}`, uncomment this line in `main.cpp`:
```cpp
_CrtSetBreakAlloc(156); // Replace 156 with your allocation number
```

---

## 2. Visual Leak Detector (VLD) - Optional

### Description
VLD is a more powerful and user-friendly memory leak detector with detailed reports.

### Installation
1. Download VLD from: https://github.com/KindDragon/vld/releases
2. Install to default location: `C:\Program Files (x86)\Visual Leak Detector`

### How to Use
1. Configure CMake with VLD enabled:
   ```bash
   cmake -B build/windows-debug -DCMAKE_BUILD_TYPE=Debug -DUSE_VLD=ON
   cmake --build build/windows-debug --config Debug
   ```

2. Run the executable. VLD will automatically detect and report leaks.

### Output Example
```
Visual Leak Detector Version 2.5.1 installed.
    Aggregating duplicate leaks.
    Leak sizes are in bytes.

WARNING: Visual Leak Detector detected memory leaks!
---------- Block 1 at 0x00000123456789AB: 64 bytes ----------
  Leak Hash: 0xABCDEF01, Count: 1, Total 64 bytes
  Call Stack (TID 1234):
    ucrtbased.dll!malloc()
    cs200_fun.exe!operator new() Line 25
    cs200_fun.exe!MyClass::MyClass() Line 42 (c:\path\to\myclass.cpp)
    cs200_fun.exe!main() Line 98 (c:\path\to\main.cpp)
  Data:
    CD CD CD CD    CD CD CD CD    CD CD CD CD    CD CD CD CD
```

---

## 3. AddressSanitizer (Compiler-based) - Alternative

### Description
Clang/GCC's AddressSanitizer can detect memory errors, but **not supported** with MSVC in this project setup.

If using MinGW/Clang on Windows, add to CMakeLists.txt:
```cmake
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(project_options INTERFACE -fsanitize=address)
    target_link_options(project_options INTERFACE -fsanitize=address)
endif()
```

---

## Comparison

| Tool | Ease of Use | Detail Level | Platform |
|------|-------------|--------------|----------|
| CRT Debug Heap | ⭐⭐⭐⭐ | ⭐⭐⭐ | Windows only |
| VLD | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | Windows only |
| AddressSanitizer | ⭐⭐⭐ | ⭐⭐⭐⭐ | Cross-platform (not MSVC) |

---

## Recommended Workflow

1. **During Development**: Use **CRT Debug Heap** (always enabled in Debug builds)
2. **For Deep Analysis**: Install and use **VLD** when you find leaks
3. **Cross-Platform**: Use **AddressSanitizer** with GCC/Clang

---

## Notes

- Memory leak detection only works in **Debug** builds
- VLD must be installed separately
- CRT Debug Heap reports appear in the Output window when debugging in Visual Studio
- Both methods work together - you can use CRT Debug Heap as a first pass, then VLD for details
