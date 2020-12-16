# Visual Computing Project by Juan Pablo Stumpf, Philip Wille and Andrea Portscher

## Short description

This project contains a simple simulated robot arm made with OpenGL and written in C++. 

## How to run the project:
- Clone
- Execute:
    - cd build
    - cmake ..
    - make
    - ./assign_3

## Keyboard controls:

First select which object you want to control:

- 0 - camera
- 1 - First limb
- 2 - Second limb
- 3 - Third limb
- 4 - Ambient light effect
- 5 - Diffuse light effect
- 6 - specular light effect

When controlling the camera:

- W - zoom in
- S - zoom out
- A - left
- D - right

When controlling limbs:

- W - up
- S - down
- A - left
- D - right

Additional keyboard controls:

- R - reset to initial position
- Q - closes the window

Mouse wheel:

- Also controls the zoom

Controlling light:

- Plus at Numpad - Amplifies selected light effect
- Minus at Numpad - Damplifies selected light effect

## Mesh files

The meshes for the base and the banana were downloaded from www.free3d.com, the rest was created using Blender.
All of them were edited, and in particular triangluated and smoothened, using Blender.

Sources:

- Banana: https://free3d.com/3d-model/banana-23586.html
- Base: https://free3d.com/3d-model/baseball-base--838565.html