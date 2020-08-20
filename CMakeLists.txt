cmake_minimum_required(VERSION 3.0.2)
project(ViveTrackerPosition)

set(OPENVR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/openvr")
set(OPENVR_LIB_DIR "${OPENVR_DIR}/lib" "${OPENVR_DIR}/bin")
set(OPENVR_INCLUDE_DIR "${OPENVR_DIR}/headers")

# -----------------------------------------------------------------------------
# Taken from OpenVR CMakeLists.txt
## SYSTEM PROPERTIES ##

# If not set, determines the running platform architecture.
if(NOT PLATFORM)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PLATFORM 64)
  else()
    set(PLATFORM 32)
  endif()
endif()
message(STATUS "Compilation set for ${PLATFORM}bits architectures.")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX -DPOSIX)
  set(ARCH_TARGET linux64)

  if(${PLATFORM} MATCHES 32)
    message(WARNING "OpenVR x86 binaries not provided on GNU/Linux.")
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_MACOSX_RPATH 0)
  add_definitions(-DOSX -DPOSIX)
  set(ARCH_TARGET osx32)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions(-D_WIN32)
  set(ARCH_TARGET win${PLATFORM})
  # Binaries path for thirdparties are not generics so we try to guess their suffixes.
  set(WINDOWS_PATH_SUFFIXES win${PLATFORM} Win${PLATFORM} x${PLATFORM})
endif()


## OpenVR API path
find_library(OPENVR_LIBRARIES 
	NAMES 
	  openvr_api 
	PATHS	
	  ${OPENVR_DIR}/bin
	  ${OPENVR_DIR}/lib
	PATH_SUFFIXES
	  osx32
	  linux64
	  ${WINDOWS_PATH_SUFFIXES}
	NO_DEFAULT_PATH
	NO_CMAKE_FIND_ROOT_PATH
)

# -----------------------------------------------------------------------------

add_executable(ViveTrackerPos src/main.cpp)
target_include_directories(ViveTrackerPos PUBLIC ${OPENVR_INCLUDE_DIR})
target_link_libraries(ViveTrackerPos PUBLIC ${OPENVR_LIBRARIES})
