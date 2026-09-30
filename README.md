# Magnet Searcher Robot

University of Bristol Robotic Systems (EMATM0054) assessment project (Arduino C++). A robot that searches a bounded grid for a hidden magnet and returns to its start position.

## Features
- Line sensor and magnetometer calibration by spinning in place
- Pose estimation (x, y, heading) from wheel encoders using kinematics
- Grid search that turns by a random angle when it reaches the boundary line
- Magnet detection from calibrated magnetometer readings
- State machine: calibrate, leave start, search, magnet found, return to start
