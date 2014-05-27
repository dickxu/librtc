# CMAKE_C_FLAGS CMAKE_CXX_FLAGS
add_definitions(-O2 -Wall)

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
    ${PROJECT_SOURCE_DIR}/ubase
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/third_party/webrtc/trunk
)

# For librtc
set(librtc_LIB_SRCS
    mainx.cpp
    media.cpp
    peer.cpp
    observer.cpp
    stream.cpp
    track.cpp
)

set(a_NAME rtc_static)
add_library(${a_NAME} STATIC ${librtc_LIB_SRCS})
set_target_properties(${a_NAME} PROPERTIES OUTPUT_NAME rtc)
set_target_properties(${a_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/)

#set(CMAKE_INSTALL_PREFIX /tmp/librtc)
install(TARGETS ${a_NAME}  ARCHIVE DESTINATION lib)
install(FILES ${PROJECT_SOURCE_DIR}/inc/xrtc_api.h DESTINATION inc)

