## How to set up a project using RealEngine

This document explains how to set up a project using [RealEngine](https://github.com/ZADNE/RealEngine). RealEngine is a simple engine-like library written in C++23. It uses Vulkan as rendering backed so it integrates shaders written in GLSL the best, though it can be fed SPIR-V directly. RealEngine is built using CMake.

### File structure setup

A project using RealEngine should be created as a **direct subfolder of a freshly cloned [Real](https://github.com/ZADNE/Real) repo**. Real is a very lightweight repo (it does not contain RealEngine). If you need to build RealEngine from source, clone it next to your project. If you do not want build it from source, [get a prebuilt release](https://github.com/ZADNE/RealEngine/releases) and make sure the package can be found using the standard CMake `find_package mechanism`. Given project name `MyProject` and building RealEngine from source, the folder structure should look like this:

```
Real/
├─ MyProject/
│  ├─ CMakeLists.txt
│  ├─ ...
├─ RealEngine/
│  ├─ CMakeLists.txt
│  ├─ ...
├─ CMakeLists.txt
```

The top level `CMakeLists.txt` must stay intact. Real also contains RealEngine-default configuration files for various tools (clangformat etc.) but these are not strictly necessary. There can be multiple RealProjects next to each other.

### CMake setup

RealProjects shall be created with `add_real_executable` and `add_real_library` functions which take the same arguments as their standard versions. It is also advised to set up the main source directory - eg. `real_target_init_file_sets(MyProject BASE_DIR "src")` allows to `#include` files relative to `src` directory.

Individual files can be added to project using `real_target_sources`. This function works similar to `target_sources`. The important difference is that automatically incorporates shaders based on file extensions. By default it expects the same file extensions as Real Naming Convention recommends, though it can be changed via CMake cache variables.

See `cmake/RealProject/RealProject.cmake` for full list and documentation of public functions and configuration options.

### Incorporation of GLSL shaders

Adding files via `real_target_sources` ensures that for a shader stage, C++ wrappers holding SPIR-V of the shader is generated. For example, given shader file `drawTiles.frag`, C++ header named `drawTiles_frag.hpp` is generated. This header holds the SPIR-V as a C++ constant named `drawTiles_frag`. The wrapper can be included as if it was placed in the same folder where the original shader was. The system also ensures that the SPIR-V is regenerated whenever the shader is changed.

But there are cases when more than the SPIR-V is needed from shaders:

* It is convenient to have a single definition of structs used in uniform and storage buffers to ensure the same memory layout in C++ and GLSL.
* In some cases, it might be needed to sample the same mathematical both on CPU and GPU. Then again, it comes handy to have only one definition of such a function, instead of one written in C++ and analogical one in GLSL.

To serve these use cases, wrappers for GLSL headers are generated too. Wrapper for headers are very different from wrappers of stages. They contain (`#include`) the original shader wrapped in a code that brings C++ and GLSL closer. Most notable changes:

* GLSL keywords not relevant for C++ are removed. This includes `layout`, `coherent`, `restrict`, `readonly` and `writeonly`.
* `buffer` and `uniform` keywords are replaced with `struct`.
* `glm` namespace is used and swizzling can be used without parenthesis.

**This allows to include standard GLSL into C++ and reuse the mathematical functions and structures** written there. However, there are a few limitations on what shader headers can be included into C++:

* The mathematical functions must be separated from its input and outputs. Thus, the headers must not use any textures, samplers and stage code, ie. stage inputs and outputs, the main function, shared variables etc.
* `scalar` layout should be used for interface blocks to ensure that the memory layout is the same in both languages.
* `out` and `inout` keywords cannot be used.

You can also include a special GLSL header via `#include <RealShaders/CppIntegration.glsl>` to:

* Introduce macro `RE_GLSL_ONLY(...)` that preserves the contents only for GLSL compiler.
* Introduce macro `RE_CPP_ONLY(...)` that preserves the contents only for C++ compiler.
* Remove `inline` keyword from GLSL.

As an example, the following snippet:

```
layout (set = 0, binding = 1, scalar)
restrict buffer PlayerHitboxSB {
    vec2 botLeftPx[2];
    vec2 dimsPx;
    vec2 velocityPx;
} RE_GLSL_ONLY(b_player);
```

Translates to almost the same GLSL:

```
layout (set = 0, binding = 1, scalar)
restrict buffer PlayerHitboxSB {
    vec2 botLeftPx[2];
    vec2 dimsPx;
    vec2 velocityPx;
} b_player;
```

But in C++, the global variable is avoided and the compiler sees this:

```
struct PlayerHitboxSB {
    vec2 botLeftPx[2];
    vec2 dimsPx;
    vec2 velocityPx;
};
```

See `cmake/RealProject/include/RealShaders/` for details on the integration.
