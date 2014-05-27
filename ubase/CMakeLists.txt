# CMAKE_C_FLAGS CMAKE_CXX_FLAGS
add_definitions(-O2 -Wall
    -DHAS_ATOMICS
    -DHAVE_PTHREAD_H
)

include_directories(
    ${PROJECT_SOURCE_DIR}/ubase
)

# For libubase
set(libubase_LIB_SRCS
    atomic.cpp      
    misc.cpp        
    mutex.cpp
)

set(a_NAME ubase_static)
add_library(${a_NAME} STATIC ${libubase_LIB_SRCS})
set_target_properties(${a_NAME} PROPERTIES OUTPUT_NAME ubase)
set_target_properties(${a_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/)

#set(CMAKE_INSTALL_PREFIX /tmp/librtc)
install(TARGETS ${a_NAME}  ARCHIVE DESTINATION lib)
install(FILES ${PROJECT_SOURCE_DIR}/ubase/misc.h DESTINATION inc)
install(FILES ${PROJECT_SOURCE_DIR}/ubase/atomic.h DESTINATION inc)
install(FILES ${PROJECT_SOURCE_DIR}/ubase/error.h DESTINATION inc)
install(FILES ${PROJECT_SOURCE_DIR}/ubase/mutex.h DESTINATION inc)
install(FILES ${PROJECT_SOURCE_DIR}/ubase/refcount.h DESTINATION inc)
install(FILES ${PROJECT_SOURCE_DIR}/ubase/types.h DESTINATION inc)
install(FILES ${PROJECT_SOURCE_DIR}/ubase/zeroptr.h DESTINATION inc)

