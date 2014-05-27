add_definitions(
    -fno-rtti
    -DPOSIX
    -DOSX
    -DWEBRTC_MAC
)

set(XCODE_ROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer")
file (GLOB _XCODE_SDKS "${XCODE_ROOT}/SDKs/*")
if (_XCODE_SDKS)
    list (SORT _XCODE_SDKS)
    #list (REVERSE _XCODE_SDKS)
    list (GET _XCODE_SDKS 0 XCODE_SDK_ROOT)
else ()
    message (FATAL_ERROR "No Mac SDK's found in default search path ${XCODE_ROOT}.")
endif ()
message (STATUS "Toolchain using default SDK: ${XCODE_SDK_ROOT}")
set(CMAKE_OSX_SYSROOT "${XCODE_SDK_ROOT}")


find_library(FWCORESERVICES CoreServices)
find_library(FWCOREAUDIO CoreAudio)
find_library(FWCOREVIDEO CoreVideo)
find_library(FWQTKIT QTKit)
find_library(FWOPENGL OpenGL)
find_library(FWAUDIOTOOLBOX AudioToolbox)
find_library(FWAPPLICATIONSERVICES ApplicationServices)
find_library(FWFOUNDATION Foundation)
find_library(FWAPPKIT AppKit)
find_library(FWSECURITY Security)
find_library(FWIOKIT IOKit)

find_library(LIBWEBRTC libwebrtc_${CMAKE_BUILD_TYPE}.a third_party/webrtc/trunk/out_mac/${CMAKE_BUILD_TYPE})

set(all_libs
    ${LIBWEBRTC}
    ${FWCORESERVICES}
    ${FWCOREAUDIO}
    ${FWCOREVIDEO}
    ${FWQTKIT}
    ${FWOPENGL}
    ${FWAUDIOTOOLBOX}
    ${FWAPPLICATIONSERVICES}
    ${FWFOUNDATION}
    ${FWAPPKIT}
    ${FWSECURITY}
    ${FWIOKIT}
    -lcrypto
    -lssl
)
