# PhysicsServerBox2D [![Physics Server Box2D Builds](https://github.com/Ughuuu/physics_server_box2d/actions/workflows/build.yml/badge.svg)](https://github.com/Ughuuu/physics_server_box2d/actions/workflows/build.yml)

The Box2D one is on the left. Overall it increases stability of larger simulations.

![testing](output.gif)

An unofficial [**Box2D**](https://github.com/erincatto/box2d) physics server for [**Godot Engine**](https://github.com/godotengine/godot) 4.0, implemented as a GDExtension. This is a fork from [rburing/physics_server_box2d](https://github.com/rburing/physics_server_box2d), on top of which I continued implementing missing features.

The goal of the project is to be a drop-in solution for 2D physics in Godot 4.0. In your Godot project you can load the GDExtension, change the (advanced) project setting `physics/2d/physics_engine` to `Box2D`, and it will work with Godot's original 2D physics nodes such as `RigidBody2D` and `StaticBody2D`.

## Current state

[x] - Rigidbody/Staticbody
- missing lock rotation
- missing contact monitor
- missing excepted body

[x] - Areas
- just default area
- area point gravity not done

[x] - Shapes
- skewed circles and capsules not impl
- edge shape works only as static
- polygon with more than 8 edges is made with lines instead of polygon(which only works static for now)

[x] - Joints
- Pin joint doesn't have softness impl

## Installing from builds

Download from latest actions and put the `bin` folder and the `physics_server_box2d.gdextension` file.

## Building from source

1. Clone the git repository https://github.com/ughuuu/physics_server_box2d, including its `box2d` and `godot-cpp` submodules.

2. Open a terminal application and change its working directory to the `physics_server_box2d` git repository.

3. Compile `godot-cpp` for the desired `target` (`template_debug` or `template_release`):

       cd godot-cpp
       scons target=template_debug generate_bindings=yes

4. Compile the GDExtension for the same `target` as above:

       cd ..
       scons target=template_debug generate_bindings=no

*Note*: The `template_debug` target can also be loaded in the Godot editor.

For more info on how to build, view actions.

## Demo

The Godot project in the `demo` subdirectory is an example of how to load the GDExtension.
