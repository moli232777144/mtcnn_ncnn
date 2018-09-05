cmake_minimum_required(VERSION 2.8)

project(mtcnn_ncnn C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# External Config
include(ExternalProject)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/3rdparty)
set(EXTERNAL_SOURCE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
###

# Protobuf
ExternalProject_Add(Protobuf
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
        SOURCE_DIR ${EXTERNAL_SOURCE_LOCATION}/protobuf
        SOURCE_SUBDIR cmake
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        )
list(APPEND LINKS protobuf)
list(APPEND DEPS Protobuf)
######

# Ncnn
ExternalProject_Add(Ncnn
        GIT_REPOSITORY https://github.com/Tencent/ncnn.git
        SOURCE_DIR ${EXTERNAL_SOURCE_LOCATION}/ncnn
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        )
list(APPEND LINKS ncnn)
list(APPEND DEPS Ncnn)
######

# OpenCV
ExternalProject_Add(OpenCV
        GIT_REPOSITORY https://github.com/opencv/opencv.git
        SOURCE_DIR ${EXTERNAL_SOURCE_LOCATION}/opencv
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        )
list(APPEND LINKS opencv_core)
list(APPEND LINKS opencv_imgproc)
list(APPEND LINKS opencv_video)
list(APPEND LINKS opencv_videoio)
list(APPEND LINKS opencv_imgcodecs)
list(APPEND LINKS opencv_highgui)
list(APPEND DEPS OpenCV)
######

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

file(GLOB MTCNN_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.h
                    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

add_executable(mtcnn_ncnn ${MTCNN_SRC})
add_dependencies(mtcnn_ncnn ${DEPS})

target_link_libraries(mtcnn_ncnn ${LINKS})