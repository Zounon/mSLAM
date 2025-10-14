# mSLAM

### a monocular SLAM implementation built to learn SLAM engineering, modern C++, and hardware constrained programming.


### Intention of this project
1. Build a strong understanding for how (monocular) SLAM systems are engineered
2. Improve my proficiency in modern C++
3. Create a SLAM system efficent enough to run on a Raspberry Pi


### Tech stack
  - C++23
  - OpenCV2 (common CV functions)
  - Eigen3 (matrix operations and linear algebra)
  - Sophus (Lie groups for 3D geometric calculations)
  - Pangolin (2D & 3D Visualization)

### System architecture

- **Frontend**: Handles feature detection, tracking, and initial pose estimation
- **Map**: Manages frames and 3D map points with efficient data structures
- **Frame**: Encapsulates image data and associated features
- **Feature**: Represents 2D image features with connections to 3D map points
- **MapPoint**: Represents 3D points in the world coordinate system
- **Viewer**: Provides visualization of camera trajectory and map points

### Current status
Frontend
- [x] Feature detection: Good‑Features‑to‑Track (OpenCV)
- [x] Feature tracking: Lucas–Kanade optical flow
- [~] Pose estimation: Essential matrix + robust inliers

Backend
- [ ] Keyframe selection policy & Map points
- [ ] Loop closure
- [ ] simple Bundle Adjustment



## Build Instructions

The project uses CMake (3.25+) with C++23 standard requirements and depends on packages available through vcpkg.








