# ==============================================
# Try to find FFmpeg libraries:
# - avcodec
# - avformat
# - avdevice
# - avutil
# - swscale
# - avfilter
# - swresample
#
# FFMPEG_FOUND - system has FFmpeg
# FFMPEG_INCLUDE_DIR - the FFmpeg inc directory
# FFMPEG_LIBRARIES - Link these to use FFmpeg
# ==============================================

set(FFMPEG_LIBRARY_PATH /home/bql/project/workspace/lib)
set(FFMPEG_INCLUDEPATH /home/bql/project/workspace/include)


set(CMAKE_LIBRARY_PATH ${FFMPEG_LIBRARY_PATH})
set(CMAKE_INCLUDE_PATH ${FFMPEG_INCLUDEPATH})

message("===============")

message( "${FFMPEG_AVCODEC_INCLUDE_DIR}")
message( "${FFMPEG_LIBAVCODEC}")
message( "${FFMPEG_LIBAVFORMAT}")
message( "${FFMPEG_LIBSWSCALE}")
message( "${FFMPEG_LIBAVDEVICE}")
message( "${FFMPEG_LIBAVFILTER}")
message( "${FFMPEG_LIBSWRESAMPLE}")
message( "${FFMPEG_LIBPOSTPROC}")

message("===============")

if (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIR)
    # in cache already
    set(FFMPEG_FOUND TRUE)
else (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIR)

    find_path(
            FFMPEG_AVCODEC_INCLUDE_DIR
            NAMES libavcodec/avcodec.h
            PATHS ${_FFMPEG_AVCODEC_INCLUDE_DIRS}
            /usr/include
            /usr/local/include
            /opt/local/include
			/usr/include/x86_64-linux-gnu            
    )

    find_library(
            FFMPEG_LIBAVCODEC
            NAMES avcodec
            PATHS ${_FFMPEG_AVCODEC_LIBRARY_DIRS}
            /usr/lib
            /usr/local/lib
            /opt/local/lib
			/usr/lib/x86_64-linux-gnu
    )

    find_library(
            FFMPEG_LIBAVFORMAT
            NAMES avformat
            PATHS ${_FFMPEG_AVFORMAT_LIBRARY_DIRS}
            /usr/lib
            /usr/local/lib
            /opt/local/lib
			/usr/lib/x86_64-linux-gnu
    )

    find_library(
            FFMPEG_LIBAVDEVICE
            NAMES avdevice
            PATHS ${_FFMPEG_AVDEVICE_LIBRARY_DIRS}
            /usr/lib
            /usr/local/lib
            /opt/local/lib
			/usr/lib/x86_64-linux-gnu
    )

    find_library(
            FFMPEG_LIBAVUTIL
            NAMES avutil
            PATHS ${_FFMPEG_AVUTIL_LIBRARY_DIRS}
            /usr/lib
            /usr/local/lib
            /opt/local/lib
			/usr/lib/x86_64-linux-gnu
    )

    find_library(
            FFMPEG_LIBSWSCALE
            NAMES swscale
            PATHS ${_FFMPEG_SWSCALE_LIBRARY_DIRS}
            /usr/lib
            /usr/local/lib
            /opt/local/lib
			/usr/lib/x86_64-linux-gnu
    )

    find_library(
            FFMPEG_LIBAVFILTER
            NAMES avfilter
            PATHS ${_FFMPEG_AVFILTER_LIBRARY_DIRS}
            /usr/lib
            /usr/local/lib
            /opt/local/lib
			/usr/lib/x86_64-linux-gnu
    )


    find_library(
            FFMPEG_LIBSWRESAMPLE
            NAMES swresample
            PATHS ${_FFMPEG_SWRESAMPLE_LIBRARY_DIRS}
            /usr/lib
            /usr/local/lib
            /opt/local/lib
			/usr/lib/x86_64-linux-gnu
    )

    find_library(
            FFMPEG_LIBPOSTPROC
            NAMES postproc
            PATHS ${_FFMPEG_POSTPROC_LIBRARY_DIRS}
            /usr/lib
            /usr/local/lib
            /opt/local/lib
			/usr/lib/x86_64-linux-gnu
    )
    message("===============")

    message( "${FFMPEG_AVCODEC_INCLUDE_DIR}")
    message( "${FFMPEG_LIBAVCODEC}")
    message( "${FFMPEG_LIBAVFORMAT}")
    message( "${FFMPEG_LIBSWSCALE}")
    message( "${FFMPEG_LIBAVDEVICE}")
    message( "${FFMPEG_LIBAVFILTER}")
    message( "${FFMPEG_LIBSWRESAMPLE}")
    message( "${FFMPEG_LIBPOSTPROC}")

    message("===============")

    if (FFMPEG_LIBAVCODEC AND FFMPEG_LIBAVFORMAT AND FFMPEG_LIBSWSCALE AND FFMPEG_LIBAVDEVICE AND FFMPEG_LIBAVFILTER AND FFMPEG_LIBSWRESAMPLE)
        set(FFMPEG_FOUND TRUE)
    endif ()

    if (FFMPEG_FOUND)
        set(FFMPEG_INCLUDE_DIR ${FFMPEG_AVCODEC_INCLUDE_DIR})
        set(FFMPEG_LIBRARIES
                ${FFMPEG_LIBPOSTPROC}
                ${FFMPEG_LIBAVCODEC}
                ${FFMPEG_LIBAVFORMAT}
                ${FFMPEG_LIBAVUTIL}
                ${FFMPEG_LIBSWSCALE}
                #${FFMPEG_LIBAVDEVICE}
                ${FFMPEG_LIBSWRESAMPLE}
                ${FFMPEG_LIBAVFILER}

                )
    else (FFMPEG_FOUND)
        message(FATAL_ERROR "Could not find FFmpeg libraries!")
    endif (FFMPEG_FOUND)

endif (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIR)