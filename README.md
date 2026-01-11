# Raspberry Pi Orb Slam 2
## Description

This is a fork of ORB-SLAM2.  This was to test offloading of the ORB feature extraction step to an FPGA and running the rest on a Raspberry Pi 4.  Even with the features extraction moved, The raspberry pi got something like 5 FPS, which is still only marginally usable.

## Testing Steps
1. I modified the Orb SLAM 2 repo to save orb descriptors for every image in each of the test videos.  This was to provide a method of verifying the FPGA implementation of the orb feature extraction.
2. I modified Orb SLAM 2 to use the saved files instead of it's own feature extraction.  This way we could measure the performance with the feature extraction removed and see how much of a speedup we'd get.
3. Teammates implemented Orb feature extraction on FPGA Hardware.


## Test Videos

We used two test videos to test our new feature extraction, one from the TUM Dataset and one from a simulation I made in Unity.  The simulation moved a camera through a room and recorded the pose of the camera as it moved.
