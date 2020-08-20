# Minimal Vive Tracker Position Example
## Description
There are many libraries which get positional/rotational data from the Vive
Tracker, but none that I could find which do only that.  
This is a minimal working example that gets the Vive Trackers connected to
SteamVR and prints the pose as 4x4 matrix.

## Requirements
CMake 3.0.2  
OpenVR (included as submodule)   
SteamVR

## Building
Only tested on Win10 x64
```
git clone --recurse-submodules git@github.com:jaw006/ViveTrackerPosition.git
cd ViveTrackerPosition
mkdir build
cd build
cmake .. .
```
## Example Usage
```
$ ./ViveTrackerPos.exe
Found valid device at index 5
Device Index: 5
Connected!
TrackingSystemName: lighthouse
ModelNumber: VIVE Tracker Pro MV
SerialNumber: LHR-XXXXXXXX
Pose:
[-0.996883,-0.076283,-0.020117,1.465377,
0.027700,-0.099685,-0.994633,0.746515,
0.073868,-0.992091,0.101487,-0.389518,
0.0, 0.0, 0.0, 1.0]
--------------
```

## License
MIT



