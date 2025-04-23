## Real Naming Convention and Style Guide

This document explains code style rules of the [RealEngine](https://github.com/ZADNE/RealEngine) and some projects made with it (most notably [RealWorld](https://github.com/ZADNE/RealWorld)). The document mentions the most important rules.

### Structure – files and folders

Real naming convention follows [C++ Canonical Project Structure](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html) proposal. The only notable addition to the document is that folders are named strictly with underscore_case, while files are named with PascalCase. The following file extensions are used:

* `.hpp` for C++ headers (e.g. `TileDrawer.hpp`)
* `.cpp` for C++ source files (e.g. `TileDrawer.cpp`)
* `.glsl` for GLSL headers (e.g. `WorldDrawingPC.glsl`)
* `.vert`, `.tesc`, `.tese`, `.geom`, `.frag`, `.comp` for GLSL vertex, tessellation control, tessellation evaluation, geometry, fragment and compute shaders respectively (e.g. `drawFullscreen.vert`, `drawTiles.frag`)

Related C++ and GLSL source codes may be mixed inside a folder or the shaders may be placed inside a directly nested folder.

### C++ Naming Convention

Indentation and alignment should be enforced by the **usage of ClangFormat** so this section describes only identifier naming conventions:

### Types
Types (structs, classes, unions) and concepts use **PascalCase**. A class is expected to be placed in a file of the same name so, e.g., `TileDrawer` in `TileDrawer.hpp`.

A notable exception from this rule are namespaces which use (usually very short) underscore_case identifiers.

### Variables
Identifiers of a variables consist of two parts: **scope identifier followed by camelCase** given name. The identifier **does not encode the type** of the variable. Scope (or nature) identifiers include:

* `k_` for compile-time constants of any kind
* `m_` for a member variable of a class (both private and protected)
* `s_` for static member variables (both private and protected)
* `g_` for (rare) global variables
* Other variables (any locals and public struct members) do not use any prefix.

### Functions

Both free-standing and member functions use **camelCase**.

Getters and setters (where necessary) use implicit get and explicit set. So, e.g., property `m_color` is gotten via `color()` and set via `setColor(...)`.

### Example

A code sample demonstrating some of the rules follows:
```
class TileDrawer {
public:
    void drawTiles(float deltaTime);
    int tileCount() const { return m_tileCount; }
private:
   static inline constexpr int k_tilesPerChunk{256};
   int m_tileCount{};
};
```

### GLSL Naming Convention

Naming convention in GLSL is the same **as in C++ with the following adjustments**:

### Interface blocks

Name of an interface block uses **PascalCase** and it is suffixed with identifier of its type:

* `PC` for push constants
* `UB` for uniform buffers
* `SB` for storage buffers

Layout specifiers are on a seperate line above the type name. Interface blocks should be placed in files of the same name.

### Variables

Scope identifiers include:

* `k_` for compile-time and specialization constants
* `i_` for input attributes from the previous shader stage
* `o_` for output attributes to the next shader stage
* `p_` for push constants
* `u_` for uniform buffers
* `b_` for storage buffers
* `s_` for shared variables
* Other variables (such as locals) do not use any prefix.

### Example

A sample push constant interface block, which would be placed in file `WorldDrawingPC.glsl`, follows:

```
layout (push_constant, scalar)
uniform WorldDrawingPC {
    mat4 p_viewMat;
    vec2 p_minimapOffset;
};
```

### Including GLSL code into C++

See documentation of RealShaders which integrate shaders into C++.
