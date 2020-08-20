#include <vector>
#include <iostream>
#include "openvr.h"
#include "main.h"

namespace vtp
{
    // Returns indices of valid tracked devices of class @lookForClass
    void FindTrackedDevicesOfClass(vr::IVRSystem* system, const vr::TrackedDeviceClass& lookForClass, std::vector<vr::TrackedDeviceIndex_t>& validIndices)
    {
        for (vr::TrackedDeviceIndex_t idx = 0; idx < vr::k_unMaxTrackedDeviceCount; idx++)
        {
            auto trackedDeviceClass = system->GetTrackedDeviceClass(idx);
            if (trackedDeviceClass == lookForClass)
            {
                validIndices.push_back(idx);
                std::cout << "Found valid device at index " << idx << std::endl;
            }
        }
    }
    // Returns a std::string of a trackedDevice property
    std::string GetTrackedPropString(vr::IVRSystem* system, vr::TrackedDeviceIndex_t unDeviceIndex, vr::TrackedDeviceProperty prop)
    {
        if (!system) 
            return std::string();

        const uint32_t bufSize = vr::k_unMaxPropertyStringSize;
//        char* pchValue = new char[bufSize];
        std::unique_ptr<char*> pchValue = std::make_unique<char*>(new char[bufSize]);
        std::string propString = "";
        vr::TrackedPropertyError pError = vr::TrackedPropertyError::TrackedProp_NotYetAvailable;

        system->GetStringTrackedDeviceProperty(unDeviceIndex, prop, *pchValue.get(), bufSize, &pError);

        if (pError != vr::TrackedPropertyError::TrackedProp_Success)
        {
            propString = "Error";
        }
        else
        {
            propString = std::string(*pchValue.get());
        }
        return propString;
    }

    // +y is up, +x is right, -z is forward, units are meters
    // According to https://github.com/ValveSoftware/openvr/issues/689 ,
    // [X1 Y1 Z1 P1]
    // [X2 Y2 Z2 P2]
    // [X3 Y3 Z3 P3]
    // Where P is position vector
    std::string HmdMatrix34ToString(vr::HmdMatrix34_t& m)
    {
        // Returns m.m[col][row] followed by a comma
        auto mn = [&](auto col, auto row) { return std::to_string(m.m[col][row]) + ","; };
        return
          "[" + mn(0, 0) + mn(0, 1) + mn(0, 2) + mn(0, 3) + "\n" +
                mn(1, 0) + mn(1, 1) + mn(1, 2) + mn(1, 3) + "\n" +
                mn(2, 0) + mn(2, 1) + mn(2, 2) + mn(2, 3) + "\n" +
                "0.0, 0.0, 0.0, 1.0]";
    }

    // Returns TrackedDevicePose_t struct of device# trackerIdx
    vr::TrackedDevicePose_t GetTrackedDevicePose(vr::IVRSystem* system_, vr::TrackedDeviceIndex_t& trackerIdx)
    {
        vr::VRControllerState_t state;
        vr::TrackedDevicePose_t pose;
        bool gotPose = system_->GetControllerStateWithPose(
            vr::ETrackingUniverseOrigin::TrackingUniverseStanding,
            trackerIdx,
            &state,
            1,
            &pose
        );
        if (!gotPose)
        {
            std::cerr << "Failed to get pose";
        }
        return pose;
    }

    // Prints information about TrackedDevice pose
    void PrintTrackedDevicePose(vr::TrackedDevicePose_t& pose)
    {
        bool& poseValid = pose.bPoseIsValid;
        std::cout << "Pose: ";
        if (poseValid)
        {
            // Print matrix
            std::cout << "\n" + vtp::HmdMatrix34ToString(pose.mDeviceToAbsoluteTracking) << std::endl;
        }
        else
        {
            std::cout << "Invalid" << std::endl;
        }
    }
}

int main(int argc, char* argv)
{
    using namespace vr;

    //INIT -- Taken from OpenVR sample
    vr::EVRInitError eError = vr::VRInitError_None;
    vr::IVRSystem* system_ = vr::VR_Init( &eError, vr::VRApplication_Other);
    if ( eError != vr::VRInitError_None )
    {
        system_ = NULL;
        char buf[1024];
        sprintf_s( buf, sizeof( buf ), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription( eError ) );
        return false;
    }

    // Find generic trackers only
    vr::TrackedDeviceClass lookForClass = TrackedDeviceClass::TrackedDeviceClass_GenericTracker;

    // Store found trackers in vector
    std::vector<vr::TrackedDeviceIndex_t> genericTrackerIndices;
    vtp::FindTrackedDevicesOfClass(system_, lookForClass, genericTrackerIndices);
    if (genericTrackerIndices.empty())
    {
        std::cerr << "No tracked devices found!";
        return -1;
    }

    // Loop through trackers and get more info
    for (auto trackerIdxIt = genericTrackerIndices.begin(); trackerIdxIt != genericTrackerIndices.end(); trackerIdxIt++)
    {
        vr::TrackedDeviceIndex_t& trackerIdx = *trackerIdxIt;
        bool isConnected = system_->IsTrackedDeviceConnected(trackerIdx);
        std::cout << "Device Index: " << trackerIdx << std::endl;
        if (isConnected)
        {
            // Print property strings
            auto getPropString = [&](auto desc, auto prop) { 
                return desc + vtp::GetTrackedPropString(system_, trackerIdx, prop) + "\n"; 
            };
            std::cout << "Connected!" << std::endl;
            std::cout << getPropString("TrackingSystemName: ", Prop_TrackingSystemName_String);
            std::cout << getPropString("ModelNumber: ",        Prop_ModelNumber_String );
            std::cout << getPropString("SerialNumber: ",       Prop_SerialNumber_String);

            // Print device pose info
            auto pose = vtp::GetTrackedDevicePose(system_, trackerIdx);
            vtp::PrintTrackedDevicePose(pose);
        }
        else
        {
            std::cout << "Error:Not connected!" << std::endl;
        }
        std::cout << "--------------" << std::endl;
    }

// SHUTDOWN
    vr::VR_Shutdown();
    system_ = NULL;
    return 0;
}

