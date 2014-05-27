set(testrtc_EXEC_SRCS
    defaults.cc
    pclient.cpp
    testrtc.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
    ${PROJECT_SOURCE_DIR}/ubase
    ${PROJECT_SOURCE_DIR}/third_party/webrtc/trunk
)

link_directories(
    ${PROJECT_BINARY_DIR}/lib
)
link_libraries(testrtc ubase rtc ${all_libs})

add_executable(testrtc ${testrtc_EXEC_SRCS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

install(TARGETS testrtc RUNTIME DESTINATION bin)
