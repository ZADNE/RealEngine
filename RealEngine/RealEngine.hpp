#pragma once
/*
RealEngine v1.2.0 (11/24 2018)
- Geometry Batch
- default shaders
- std objects
- uniform auto-update
RealEngine v1.2.1 (11/25 2018)
- Uniform Manager (shared uniform blocks)
RealEngine v1.3.1 (11/26 2018)
- support for mouse double-clicks, triple-clicks etc.
- changeable window title
RealEngine v1.4.1 (11/29 2018)
- CursorDrawer (changeable cursor)
RealEngine v1.5.1 (11/29 2018)
- KeyBinder
- LanguageManager
RealEngine v1.6.1 (1/13 2019)
- Menu revamp: loading from ini files, string names etc.
RealEngine v1.6.2 (1/15 2019)
- MenuParser: refactoring, optimalization, additive path support
- LanguageManager: automatically loads last used language
RealEngine v1.6.3 (1/18 2019)
- KeyBinder: key change by listening for a key
RealEngine v1.6.4 (1/24 2019)
- InputManager: support for horizontal side wheel, isDown() length upgrade
RealEngine v1.6.5 (3/12 2019)
- MainProgram, RoomVector, Room fixes and/or improvements
RealEngine v1.6.6 (3/12 2019)
- MainProgram, Window improvemments and refactoring
- Window can switch fullscreen/borderless/vsync while running
- MainProgram can query info about all displays
RealEngine v1.7.0 (1/2 2021)
- Revamped textures (texture images/parameters, new rti format, rti inside png)
- Shader programs with all OpenGL stages
*/

#define RE_VERSION_MAJOR 1
#define RE_VERSION_MINOR 7
#define RE_VERSION_PATCH 0


namespace RE {

int initRE();

}