cmake_minimum_required(VERSION 3.0)
project(RTSPServer)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -pthread" )
# find_package(FFmpeg REQUIRED)
find_package(Live555 REQUIRED)

set(FFMPEG_STATIC_LIB_PATH /home/bql/project/workspace/lib/)
set(FFMPEG_STATIC_INCLUDE_PATH /home/bql/project/workspace/include/)

include_directories("inc")


add_executable(
        ${PROJECT_NAME}
        ${SOURCE_FILES}
        src/main.cpp
        src/FFmpegDecoder.cpp
        src/FFmpegH264Encoder.cpp
        src/FFmpegH264Source.cpp
        src/LiveRTSPServer.cpp
        src/LiveServerMediaSubsession.cpp
)


# FFmpeg
#if (FFMPEG_FOUND)
#    include_directories(${FFMPEG_INCLUDE_DIR})
#    message("${FFMPEG_LIBRARIES}")
#   # target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIBRARIES})
#else (FFMPEG_FOUND)
#    message( "Can't find FFmpeg libs!")
#endif (FFMPEG_FOUND)


include_directories(${FFMPEG_STATIC_INCLUDE_PATH})


add_library(swresample STATIC IMPORTED)
set_property(TARGET swresample PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libswresample.a)
#target_link_libraries(${PROJECT_NAME}  swresample )




add_library(avcodec STATIC IMPORTED)
set_property(TARGET avcodec PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libavcodec.a)
#target_link_libraries(${PROJECT_NAME}  avcodec )

add_library(avformat STATIC IMPORTED)
set_property(TARGET avformat PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libavformat.a)
#target_link_libraries(${PROJECT_NAME}  avformat )

add_library(avdevice STATIC IMPORTED)
set_property(TARGET avdevice PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libavdevice.a)
#target_link_libraries(${PROJECT_NAME}  avdevice )

add_library(avutil STATIC IMPORTED)
set_property(TARGET avutil PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libavutil.a)
#target_link_libraries(${PROJECT_NAME}  avutil )

add_library(swscale STATIC IMPORTED)
set_property(TARGET swscale PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libswscale.a)
#target_link_libraries(${PROJECT_NAME}  swscale )

add_library(avfilter STATIC IMPORTED)
set_property(TARGET avfilter PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libavfilter.a)
#target_link_libraries(${PROJECT_NAME}  avfilter )

add_library(postproc STATIC IMPORTED)
set_property(TARGET postproc PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libpostproc.a)
#target_link_libraries(${PROJECT_NAME}  postproc )

add_library(x264 STATIC IMPORTED)
set_property(TARGET x264 PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libx264.a)
#target_link_libraries(${PROJECT_NAME}  x264 )

add_library(x265 STATIC IMPORTED)
set_property(TARGET x265 PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libx265.a)
#target_link_libraries(${PROJECT_NAME}  x265 )

add_library(vidstab STATIC IMPORTED)
set_property(TARGET vidstab PROPERTY IMPORTED_LOCATION ${FFMPEG_STATIC_LIB_PATH}/libvidstab.a)
#target_link_libraries(${PROJECT_NAME}  vidstab )


target_link_libraries(${PROJECT_NAME}
 -Wl,--start-group
 avcodec
 avdevice
 avfilter
 avformat
 avutil
 postproc
 swresample
 swscale
 x264
 x265
 vidstab
 -Wl,--end-group
 )


# Live555 Framework
if (Live555_FOUND)
    message("Found Live555")
    foreach (Live555_module IN LISTS Live555_INCLUDE_DIRS)
        include_directories(${Live555_module})
    endforeach ()
    target_link_libraries(${PROJECT_NAME} ${Live555_LIBRARIES})
else (Live555_FOUND)
    message(FATAL_ERROR "Can't find Live555 libs!")
endif (Live555_FOUND)



target_link_libraries(${PROJECT_NAME} -lpthread)
target_link_libraries(${PROJECT_NAME} -lz)
target_link_libraries(${PROJECT_NAME} -ldl)
target_link_libraries(${PROJECT_NAME} -lm)
target_link_libraries(${PROJECT_NAME} -lrt)
target_link_libraries(${PROJECT_NAME} -llzma)

