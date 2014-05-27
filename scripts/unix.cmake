
add_definitions(
    -DPOSIX
    -DWEBRTC_LINUX
    -Wunused-but-set-variable
)

find_package(GTK2 REQUIRED)
if (GTK2_FOUND)
    include_directories(${GTK2_INCLUDE_DIR})
    add_definitions(-DHAVE_GTK2)
    message("GTK 2.x found and used as GUI\n")
else (GTK2_FOUND)
    message(FATAL_ERROR "GTK2 is required to build this project.")
endif (GTK2_FOUND)

find_library(LIBWEBRTC libwebrtc_${CMAKE_BUILD_TYPE}.a third_party/webrtc/trunk/out_unix/${CMAKE_BUILD_TYPE})

#${GTK2_LIBRARIES}
set(all_libs
    ${LIBWEBRTC}
    -lX11
    -lXext
    -lXrender
    -lXcomposite
    -lGL
    -lexpat
    -ldl
    -lrt
    -lpthread
    -lssl
    -lcrypto
)

