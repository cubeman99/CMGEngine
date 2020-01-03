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
- OpenGL & Glew: Graphics library
- SOIL: Image loading and saving
- lodepng: Image saving for PNG
- DirectInput8: user input
- WinSDK
- Catch: unit tests
- Googletest: unit tests

## Trello Page
Use for issue and change tracking.
https://trello.com/b/wGRJTuHn/cmg-engine

## TODO
- General
	- Create an audio engine
	- Start using cmg namespace everywhere
	- Create CMake example project
	- Stop using struct/enum pairings, use enum class
		- decide on standard for enum names, k_enum or enum_value, and apply to code
	- Start using smart pointers where necessary, like Graphics2D
		- remove sptr where not necessary like in SetShaderUniform
	- Add event-based code
		- window
		- input: key, mouse, joystick, plug-in, unplug
			- Keyboard events/mouse events (might need to use Win32 instead of directinput)
		- physics
- Core
	- Mutex and Thread wrapper
	- Better 'Error' handling, that show proper traceback
- Graphics
	- delete Renderer class
	- Make FreeType/SOIL compiled into cmgGraphics lib
	- 3D Model importing
		- OBJ & Materials
	- ECS systems for rendering
	- fix attribute location mismatches (use constant index or varying per mesh?)
	- let shader uniforms be referenced as 'array' and 'array[0]'
	- Rework DecodedImageData interface (at least for memory management)
	- generic vs specific material class

## FIXME
-


ECS plannig:
parent/child of entities
transform built-in to entity
1 component type per entity
components inheriting other components?
Update() in component
systems?


