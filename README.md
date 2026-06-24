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
.\bin\Debug\Simulator.exe 
```
_↑ This runs within .\build. Use_ `cd..` _to exit dir._

### Third-Party Libraries
With the third-party folder you can find external repositories that have been added via git submodules. The full list can be found in the .gitmodules file.
- [Assimp](https://github.com/assimp/assimp): Open **Asset Import** Library is a library that loads various 3D file 
  formats into a shared, in-memory format. It supports more than 40 file formats for import and a growing selection of file formats for export.
- [fmt](https://github.com/fmtlib/fmt): Is an open-source **formatting** library providing a fast and safe alternative 
  to C stdio and C++ iostreams.
- [glad Branch-c](https://github.com/Dav1dde/glad/tree/c): (**OpenGL Loader Generator**) is an open-source, 
  multi-language library primarily used in C and C++ to manage and load OpenGL function pointers. Cross-Platform 
  Compatibility.
- [glm](https://github.com/g-truc/glm): **OpenGL Mathematics** is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
- [json](https://github.com/nlohmann/json): JSON for Modern C++
- [SDL3](https://github.com/libsdl-org/SDL): **Simple DirectMedia Layer** is a cross-platform library designed to make it easy to write multi-media software, such as games and emulators.

### Other Resources
- [OpenGL](https://learnopengl.com/): (**Open Graphics Library**) is a cross-language, cross-platform programming interface (API) used for rendering 2D and 3D vector graphics. It serves as a universal translator, allowing software and game engines to directly communicate with a device's Graphics Processing Unit (GPU) to achieve hardware-accelerated rendering.
- [SDL3](https://wiki.libsdl.org/SDL3/FrontPage): **Simple DirectMedia Layer** is a cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL/Direct3D/Metal/Vulkan. It is used by video playback software, emulators, and popular games including Valve's award winning catalog and many Humble Bundle games.
