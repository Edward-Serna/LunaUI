# C++ Game Engine
> Creating a game engine from scratch to learn the fundamentals of game development. This is going to be a playground for me to learn low-level graphics programming and refine my coding style. The goal is to have a formal format for my internal system. 

### Prerequisites
**Required:**
- **CMake 3.20 or higher** – [Download](https://cmake.org/download/)
- **C/C++ Compiler**:
   - Windows: Visual Studio 2013+ (MSVC)
   - Linux: GCC or Clang
   - macOS: Xcode Command Line Tools
- **Git** – For fetching dependencies
- **OpenGL 3.2+** compatible graphics driver

 Check if CMake is installed: (_Should show version 3.14 or higher._)
```bash
cmake --version
```
Check if Git is installed:
```bash
git --version
```

### Installing/ Building
Download your IDE. (My recommendation is [CLion](https://www.jetbrains.com/clion/download/#section=windows))\
Clone the repository
```bash
git clone git@github.com:Edward-Serna/LunaUI.git
```
Enter repo and initialize and update the submodules.\
_These are the third party libraries that are used in the project._
```bash
git submodule update --init --recursive
```
**Windows (Visual Studio):**
```bash
mkdir build && cd build
cmake ..
cmake --build . --target Simulator -j 14
```
**Linux / macOS:**
```bash
mkdir build && cd build
cmake ..
make
```
**_Running the Program_**
```bash
.\build\bin\Simulator.exe -l .\bin\Debug\SDL3.dll
```

### Third-Party Libraries
With the third-party folder you can find external repositories that have been added via git submodules. The full list can be found in the .gitmodules file.
- [Assimp](https://github.com/assimp/assimp)
- [fmt](https://github.com/fmtlib/fmt)
- [glad Branch-c](https://github.com/Dav1dde/glad/tree/c)
- [glm](https://github.com/g-truc/glm)
- [json](https://github.com/nlohmann/json)
- [SDL3](https://github.com/libsdl-org/SDL)

### Other Resources
- [OpenGL](https://learnopengl.com/)
- Standard DirectMedia Layer 3 ([SDL3](https://wiki.libsdl.org/SDL3/FrontPage))
