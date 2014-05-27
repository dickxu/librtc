add_definitions(
    -fno-rtti
    -DPOSIX
    -DIOS
    -DWEBRTC_IOS
)


if (${TARGET} STREQUAL "IOS")
    set (IOS_PLATFORM "iPhoneOS.platform")
    set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos")
    set (CMAKE_OSX_ARCHITECTURES "armv6 armv7")
else ()
    set (IOS_PLATFORM "iPhoneSimulator.platform")
    set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphonesimulator")
    set (CMAKE_OSX_ARCHITECTURES "i386")
endif ()

set(XCODE_ROOT1 "/Applications/Xcode2.app/Contents/Developer/Platforms/${IOS_PLATFORM}/Developer")
set(XCODE_ROOT2 "/Applications/Xcode 2.app/Contents/Developer/Platforms/${IOS_PLATFORM}/Developer")
set(XCODE_ROOT3 "/Applications/Xcode.app/Contents/Developer/Platforms/${IOS_PLATFORM}/Developer")
if (EXISTS ${XCODE_ROOT1})
    set(XCODE_ROOT ${XCODE_ROOT1})
elseif (EXISTS ${XCODE_ROOT2})
    set(XCODE_ROOT ${XCODE_ROOT2})
elseif (EXISTS ${XCODE_ROOT3})
    set(XCODE_ROOT ${XCODE_ROOT3})
endif()

file (GLOB _XCODE_SDKS "${XCODE_ROOT}/SDKs/*")
if (_XCODE_SDKS) 
    list (SORT _XCODE_SDKS)
    list (REVERSE _XCODE_SDKS)
    list (GET _XCODE_SDKS 0 XCODE_SDK_ROOT)
else ()
    message (FATAL_ERROR "No iOS SDK's found in default search path ${XCODE_ROOT}. Please check iOS SDK.")
endif ()
message (STATUS "Toolchain using default iOS SDK: ${XCODE_SDK_ROOT}")
set(CMAKE_OSX_SYSROOT "${XCODE_SDK_ROOT}")


#find_library(FWCORESERVICES CoreServices)
find_library(FWCOREAUDIO CoreAudio)
find_library(FWCOREVIDEO CoreVideo)
#find_library(FWQTKIT QTKit)
#find_library(FWOPENGL OpenGL)
find_library(FWAUDIOTOOLBOX AudioToolbox)
#find_library(FWAPPLICATIONSERVICES ApplicationServices)
find_library(FWFOUNDATION Foundation)
#find_library(FWAPPKIT AppKit)
find_library(FWSECURITY Security)
find_library(FWIOKIT IOKit)

if (${TARGET} STREQUAL "IOS")
find_library(LIBWEBRTC libwebrtc_${CMAKE_BUILD_TYPE}.a third_party/webrtc/trunk/out_ios/${CMAKE_BUILD_TYPE}-iphoneos)
else ()
find_library(LIBWEBRTC libwebrtc_${CMAKE_BUILD_TYPE}.a third_party/webrtc/trunk/out_sim/${CMAKE_BUILD_TYPE})
endif ()

set(all_libs
    ${LIBWEBRTC}
    ${FWCOREAUDIO}
    ${FWCOREVIDEO}
    ${FWAUDIOTOOLBOX}
    ${FWFOUNDATION}
    ${FWSECURITY}
    ${FWIOKIT}
    -lcrypto
    -lssl
)
