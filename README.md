DropBoneImu
===========

Team 4774's Inertial Measurement Unit.

This project runs on a Beaglebone Black (BBB) and talks over i2c to an MPU-6050.
It uses the sample code from Invensense so the MPU-6050 is able to return a 3D
orientation solution (ie roll, pitch, yaw).
All processing is done on board - no need for the BBB/cRIO/roboRIO to do any
integration.

The BBB broadcasts the accelerometer, gyro and orientation information over UDP.

Documentation is available here:
http://thedropbears.org.au/mediawiki-1.23.2/index.php/DropBone_IMU
