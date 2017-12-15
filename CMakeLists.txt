#1.cmake verson，指定cmake的最小版本号 
cmake_minimum_required(VERSION 2.8)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
project(mtcnn_ncnn C CXX)

#3.set environment variable，设置环境变量
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#4.include头文件目录 
include_directories(${CMAKE_CURRENT_LIST_DIR}/3rdparty/include/Opencv
					${CMAKE_CURRENT_LIST_DIR}/3rdparty/include/Opencv/opencv
					${CMAKE_CURRENT_LIST_DIR}/3rdparty/include/Opencv/opencv2
					${CMAKE_CURRENT_LIST_DIR}/3rdparty/include/ncnn
                    ${CMAKE_CURRENT_LIST_DIR}/src)

#5.library目录及name名称
link_directories(${CMAKE_CURRENT_LIST_DIR}/3rdparty/lib)
list(APPEND MTCNN_LINKER_LIBS opencv_world320 ncnn)

#6.source directory源文件目录
file(GLOB MTCNN_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.h
                    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
set(MTCNN_COMPILE_CODE ${MTCNN_SRC})

#7.1.add executable file，编译为可执行文件
add_executable(mtcnn_ncnn ${MTCNN_COMPILE_CODE})
#7.2.add library file，编译为动态库
# add_library(mtcnn_ncnn SHARED ${MTCNN_COMPILE_CODE})

#8.add link library，添加工程所依赖的库
target_link_libraries(mtcnn_ncnn ${MTCNN_LINKER_LIBS})