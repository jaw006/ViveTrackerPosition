#include <vector>
#include <iostream>
#include "openvr.h"
#include "main.h"

#define PRINTER(name) printer(#name, (name))
namespace vtp
{
    void FindTrackedDevicesOfClass(vr::IVRSystem* system_, const vr::TrackedDeviceClass& lookForClass, std::vector<vr::TrackedDeviceIndex_t>& validIndices)
    {
        for (vr::TrackedDeviceIndex_t idx = 0; idx < vr::k_unMaxTrackedDeviceCount; idx++)
        {
            auto trackedDeviceClass = system_->GetTrackedDeviceClass(idx);
            if (trackedDeviceClass == lookForClass)
            {
                validIndices.push_back(idx);
                std::cout << "Found valid device at index " << idx << std::endl;
            }
        }
    }
    // Returns a string 
    std::string GetTrackedPropString(vr::IVRSystem* system_, vr::TrackedDeviceIndex_t unDeviceIndex, vr::TrackedDeviceProperty prop)
    {
        if (!system_) 
            return std::string();

        const uint32_t bufSize = vr::k_unMaxPropertyStringSize;
//        char* pchValue = new char[bufSize];
        std::unique_ptr<char*> pchValue = std::make_unique<char*>(new char[bufSize]);
        std::string propString = "";
        vr::TrackedPropertyError pError = vr::TrackedPropertyError::TrackedProp_NotYetAvailable;

        system_->GetStringTrackedDeviceProperty(unDeviceIndex, prop, *pchValue.get(), bufSize, &pError);

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
}

int main(int argc, char* argv)
{
    using namespace vr;
    //INIT Taken from OpenVR sample
    vr::EVRInitError eError = vr::VRInitError_None;
    vr::IVRSystem* system_ = vr::VR_Init( &eError, vr::VRApplication_Other);

    if ( eError != vr::VRInitError_None )
    {
        system_ = NULL;
        char buf[1024];
        sprintf_s( buf, sizeof( buf ), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription( eError ) );
        return false;
    }

    // Find generic trackers
    std::vector<vr::TrackedDeviceIndex_t> genericTrackerIndices;
    vr::TrackedDeviceClass lookForClass = TrackedDeviceClass::TrackedDeviceClass_GenericTracker;
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
            auto getPropString = [&](auto desc, auto prop) { 
                return desc + vtp::GetTrackedPropString(system_, trackerIdx, prop) + "\n"; 
            };
            std::cout << "Connected!" << std::endl;
            std::cout << getPropString("TrackingSystemName: ", Prop_TrackingSystemName_String);
            std::cout << getPropString("ModelNumber: ",        Prop_ModelNumber_String );
            std::cout << getPropString("SerialNumber: ",       Prop_SerialNumber_String);
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
