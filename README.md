# CMGEngine

CMGEngine is a C++ game engine developed by David Jordan (created in 2015). Uses OpenGL for graphics.
Uses CMake to build. Currently being tested on Windows 10.

## Libraries

- CMGCore: Core engine functionality and framework
- CMGMath: Math types such as Vector2f and Matrix4f
- CMGGraphics: Graphics library for drawing and rendering 2D and 3D using OpenGL
- CMGApplication: High level application level code including resource management.
- CMGInput: Keyboard, mouse, and joystick input system using DirectInput8.
- CMGPhysics: 3D physics engine.

## Dependencies

- FreeType: Vectorized fonts
- Glew
- OpenGL
- lodepng
- SOIL: Image loading
- DirectInput8
- WinSDK
- Catch: unit tests
- Googletest: unit tests

## TODO
- Add event-based code
	- window: 
	- input: key, mouse, joystick, plug-in, unplug, physics
- Keyboard events/mouse events (might need to use Win32 instead of directinput)
- Mutex and Thread wrapper
- Better 'Error' handling, that show proper traceback
- Start using smart pointers where necessary, like Graphics2D
- Start using FreeType
- Rework DecodedImageData interface (at least for memory management)
- Stop using struct/enum pairings, use enum class
- decide on standard for enum names, k_enum or enum_value, and apply to code
- Audio engine
- Start using cmg namespace everywhere
- remove LodePNG (using SOIL now)

## FIXME
- bug where app closes if alt-tabbed during startup
