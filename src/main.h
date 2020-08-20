#pragma once

void FindTrackedDevicesOfClass(vr::IVRSystem* system_, const vr::TrackedDeviceClass& lookForClass, std::vector<vr::TrackedDeviceIndex_t>& validIndices);
