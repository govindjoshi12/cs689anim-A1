Govind Joshi
gvj84
gvj84@tamu.edu

tmax: Number of Keyframes * 2

The number of keyframes can be changed in my animation, and I thought it was best to keep the time proportional to the number of keyframes. Not multiplying by two can cause the helicopter to move unnaturally fast.

Drew my spline surface with triangles for the bonus.

NEW CONTROLS AND FEATURES:
- K: Toggle keyframes and spline
  - Default: On.

- S: Cycle through time control functions
  1. Default: No arc length parametrization
  2. Arc length parametrization
  3. Ease-in and Ease-out
  4. Quadrate speed up
  5. Custom Multiple Cubics function 

- Spacebar: Toggle helicopter camera. 
  - Default: Off.

- H: While in helicopter mode, toggle whether the camera rotates with the helicopter. 
  - Default: Camera rotation on.

- C: Reset Camera to original position. Useful when switching to helicopter mode to reset camera. I have the option to not apply camera transforms to the MV matrix when in helicopter mode, but I like being able to move the camera in Helicopter Cam mode too.

- Keyframe Amount Controls
  - \-: Remove Keyframe
  - =: Add keyframe
  - Range [5, 27], Default: 10
  - Keyframes are randomly sampled from a 3x3x3 grid of positions. Adding and removing keyframes does not re-randomize the keyframe generation.

- Keyframe Positioning Controls:
  - <: Decrease interval between keyframes by 1.0f.
  - \>: Increase interval between keyframes
  - Range [2.0f, 6.0f], Default: 5.0f
  - Increasing the length between keyframes also alters the size of the spline surface by changing the number of control points (within a range).

- G: Toggle using Gaussian Quadrature
  - Default: GQ on.

- M: Toggle spline surface control points
  - Default: Off.