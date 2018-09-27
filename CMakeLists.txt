cmake_minimum_required(VERSION 3.10)

project(mtcnn_ncnn C CXX)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

################# External Config ###############################
include(ExternalProject)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/3rdparty)
set(EXTERNAL_SOURCE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
################################################################

if (NOT ANDROID AND NOT IOS)

##################### Ncnn ######################################
ExternalProject_Add(Ncnn
        GIT_REPOSITORY https://github.com/Tencent/ncnn.git
        SOURCE_DIR ${EXTERNAL_SOURCE_LOCATION}/ncnn
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        )
list(APPEND LINKS ncnn)
list(APPEND DEPS Ncnn)
#################################################################

################### Protobuf ####################################
ExternalProject_Add(Protobuf
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
        SOURCE_DIR ${EXTERNAL_SOURCE_LOCATION}/protobuf
        SOURCE_SUBDIR cmake
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        )
list(APPEND LINKS protobuf)
list(APPEND DEPS Protobuf)
#################################################################

##################### GTest ######################################
ExternalProject_Add(GTest
        GIT_REPOSITORY https://github.com/google/googletest.git
        SOURCE_DIR ${EXTERNAL_SOURCE_LOCATION}/gtest
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        )
list(APPEND LINKS gtest)
list(APPEND DEPS GTest)
#################################################################

###################### GTK2 ###########################
if (NOT ANDROID AND NOT IOS)
find_package(GTK2 REQUIRED)
if (GTK2_FOUND)
else()
    MESSAGE(FATAL_ERROR "GTK2 isn't found !!")
endif()
endif()
########################################################

###################### OpenCV ##########################
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
#######################################################

###################### Dlib ##########################
ExternalProject_Add(Dlib
        GIT_REPOSITORY https://github.com/davisking/dlib.git
        SOURCE_DIR ${EXTERNAL_SOURCE_LOCATION}/dlib
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        )
list(APPEND LINKS dlib)
list(APPEND DEPS Dlib)
#######################################################

#################### OpenMP ############################
if (UNIX AND NOT APPLE)
    find_package(OpenMP REQUIRED)
    if (OpenMP_FOUND)
        list(APPEND LINKS gomp)
    else()
        MESSAGE(FATAL_ERROR "OpenMP isn't found !!")
    endif()
endif()
#########################################################
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

file(GLOB MTCNN_IMAGE_DIR_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.h
                              ${CMAKE_CURRENT_LIST_DIR}/src/mtcnn.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/mtcnn_image_dir.cpp)
file(GLOB MTCNN_DLIB_IMAGE_DIR_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.h
                              ${CMAKE_CURRENT_LIST_DIR}/src/mtcnn.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/mtcnn_dlib_image_dir.cpp)
file(GLOB MTCNN_VIDEO_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.h
                          ${CMAKE_CURRENT_LIST_DIR}/src/mtcnn.cpp
                          ${CMAKE_CURRENT_LIST_DIR}/src/mtcnn_video.cpp)

add_executable(mtcnn_image_dir ${MTCNN_IMAGE_DIR_SRC})
#add_dependencies(mtcnn_image_dir ${DEPS})

target_link_libraries(mtcnn_image_dir ${LINKS} gomp)

add_executable(mtcnn_dlib_image_dir ${MTCNN_DLIB_IMAGE_DIR_SRC})
#add_dependencies(mtcnn_dlib_image_dir ${DEPS})

target_link_libraries(mtcnn_dlib_image_dir ${LINKS} gomp pthread png jpeg)
#target_link_libraries(mtcnn_dlib_image_dir ${LINKS} gomp pthread png jpeg X11)

add_executable(mtcnn_video ${MTCNN_VIDEO_SRC})
#add_dependencies(mtcnn_video ${DEPS})

target_link_libraries(mtcnn_video ${LINKS} gomp)

else()

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

file(GLOB MTCNN_IMAGE_DIR_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.h
                              ${CMAKE_CURRENT_LIST_DIR}/src/mtcnn.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/mtcnn_image_dir.cpp)
add_executable(mtcnn_image_dir ${MTCNN_IMAGE_DIR_SRC})
#add_dependencies(mtcnn_image_dir ${DEPS})

list(APPEND LINKS ncnn)
list(APPEND LINKS opencv_core)
list(APPEND LINKS opencv_imgproc)
list(APPEND LINKS opencv_video)
list(APPEND LINKS opencv_videoio)
list(APPEND LINKS opencv_imgcodecs)
list(APPEND LINKS opencv_highgui)
list(APPEND LINKS omp)

target_link_libraries(mtcnn_image_dir ${LINKS})

endif()
