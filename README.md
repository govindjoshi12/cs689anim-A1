Govind Joshi
gvj84
gvj84@tamu.edu

tmax: Number of Keyframes * 2

The number of keyframes can be changed in my animation, and I thought it was best to keep the time proportional to the number of keyframes. Not multiplying by two can cause the helicopter to move unnaturally fast.

Drew my spline surface with triangles for the bonus.

Side notes: 
- The camera begins underneath the spline surface so you need to shift up and zoom out to see the action.
- Added an int cast to data.size() in GLSL.cpp:167 to prevent compiler warning

#### NEW CONTROLS
Many of the parameters listed below are defined at the top of the main.cpp file.

- K: Toggle keyframes and spline
  - Default: On.

- S: Cycle through time control functions
  1. **Default**: No arc length parametrization
  2. Arc length parametrization
  3. Ease-in and Ease-out
  4. Quadrate speed up
  5. Custom Multiple Cubics function 

- Spacebar: Toggle helicopter camera. Resets camera transforms applied earlier so when you return to the regular camera, you start at the camera's initial position.
  - Default: Off.

- H: While in helicopter mode, toggle whether the camera rotates with the helicopter. 
  - Default: Camera rotation on.

- C: Reset Camera to original position. Useful when switching to helicopter mode to reset camera. I could chose to not apply camera transforms to the MV matrix when in helicopter mode, but I like being able to move the camera in Helicopter Cam mode too.

- Keyframe Amount Controls
  - \-: Remove Keyframe
  - =: Add keyframe
  - Range [5, 27], Default: 10
  - Keyframes are randomly sampled from a 3x3x3 grid of positions. Adding and removing keyframes does not re-randomize the keyframe generation.

- Keyframe Positioning Controls:
  - <: Decrease interval between keyframes by 1.0f.
  - \>: Increase interval between keyframes
  - Range [2.0f, 6.0f], Default: 5.0f
  - The range is arbitrary. You can change the default (or max) keyframe interval length and increase it. I have tested up to 10.0f, but at the point you have to zoom out really far. You can also increase the size of the spline surface separately in the code. (splineSurfaceCps, represents the number of control points on one boundary of the patch). 
  - Increasing the length between keyframes also alters the size of the spline surface by changing the number of control points (within a range).

- G: Toggle using Gaussian Quadrature
  - Default: GQ on.

- M: Toggle spline surface control points
  - Default: Off.

#### Some implementation notes
- The 3x3x3 randomized keyframes generated with a random seed that stays constant between adding/subtracting keyframes or changing the keyframe interval. This allows for the position/rotation configuration of all possible keyframes to remain the same for a given seed. This seed is also used to randomize heights of control points for the tensor product patch. The seed can be changed by pressing R which resets the seed, animation, and tensor product patch.
- The Spline curve uses the Catmull-rom basis, and the spline surface uses the B-spline basis.