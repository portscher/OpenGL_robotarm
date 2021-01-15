# Visual Computing Project by Juan Pablo Stumpf, Philip Wille and Andrea Portscher

## Short description

This project contains a simple simulated robot arm using the OpenGL library and written in C++. 

## How to run the project

- Clone
- Execute:
  - cd build
  - cmake ..
  - make
  - ./assign_5

## Keyboard controls

First select which object you want to control:

- 0 - camera
- 1 - First limb
- 2 - Second limb
- 3 - Third limb
- 4 - Ambient light effect
- 5 - Diffuse light effect
- 6 - specular light effect
- 7 - light source

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

Selected one of the light effects:

- J - Increase selected light effect
- N - Decrease selected light effect

When controlling light:

- W - moves the light right along the z-Axis.
- S - moves the light left along the z-Axis.
- A - moves the light right along the x-Axis.
- D - moves the light left along the x-Axis.
- J - selects the next light color.
- N - selects the previous light color.
- Available colors
  - Red
  - Green
  - Blue
  - Orange
  - Yellow

## Mesh files

The meshes for the base and the banana were downloaded from www.free3d.com, the rest was created using Blender.
All of them were edited, and in particular triangluated and smoothened, using Blender.

Sources:

- Banana: https://free3d.com/3d-model/banana-23586.html
- Base: https://free3d.com/3d-model/baseball-base--838565.html
