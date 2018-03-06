---
2018.3.6更新：mtcnn_AS更新至ncnn最新的1.29版本；


附录：MTCNN的ARM端时间测试
（1000次测试图640*480最小人脸40三层网络阈值｛0.8，0.8，0.6｝）

	高通625 | Max| Min| Avg
	---|---|---|---
	单线程 | 157.65| 137.11|144.90
	双线程 | 101.68| 83.29| 87.29
	四线程 | 75.21| 59.58| 63.14
	八线程 | 277.33| 47.83| 54.29


---
# 前言
  ncnn是腾讯优图在七月份开源的，一款手机端极致优化的前向计算框架；开源有几个月了，仍然是开源里的扛把子（给nihui大佬递茶）。之前也测试移植过，这次主要做个整理，鉴于很多人只想在window下搭建并调试，本次主要基于MTCNN的人脸检测例子，进行一次该框架的搭建，构建流程主要采用脚本编写，目的在于简单演示下流程操作；
  

---
##    主要环境：
- git
- cmake
- vs2015
- android studio

---

#  PC端调试：
使用windows的pc端调试，繁琐的是依赖库的生成和引用。依赖库中Protobuf是caffe的模型序列化存储的规则库，将caffe框架转ncnn框架模型用到，另外opencv库主要用于范例的图像读取操作，可自己配置，或直接使用个人在3rdparty文件夹下编译好的库；

1. 下载源码并更新子模块,protobuf源码库比较大，更新会比较慢

```
git clone https://github.com/moli232777144/mtcnn_ncnn.git
git submodule update --init
```

2. 编译protobuf
调用tools下的protobuf脚本编译:

```
cd ../3rdparty/src/protobuf/cmake
mkdir build
cd build
cmake .. -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_MSVC_STATIC_RUNTIME=OFF -G "Visual Studio 14 2015 Win64"
pause
```

 vs2015打开./3rdparty/src/protobuf/cmake/build下的protobuf.sln工程，编译Debug及Release版本；
 
 调用tools下的copyProtobuf脚本，生成protobuf的依赖库到第三方公共文件夹3rdparty下。
 
3.  编译ncnn

修改./3rdparty/src/ncnn目录下的CMakeLists.txt中的add_definitions相关注释，修改后结果如下：

```
add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
#add_definitions(-Wall -Wextra)
#add_definitions(-fPIC)
#add_definitions(-Ofast)
#add_definitions(-ffast-math)
#add_definitions(-march=native)
#add_definitions(-flto)
#add_definitions(-fvisibility=hidden -fvisibility-inlines-hidden)
```


因为 VS2015 只支持 openmp 2.0，所以修改 src/layer/convolution_depthwise.cpp

```

#pragma omp parallel for collapse(2)
```

去掉 collapse(2)

```
#pragma omp parallel for
```

接着修改tools下的ncnn.bat工具，将DProtobuf几个参数替换为自己编译后的protobuf相关目录

```
cd ../3rdparty/src/ncnn
mkdir build
cd build
cmake .. -G"Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%cd%/install -DProtobuf_INCLUDE_DIR=F:\mtcnn_ncnn\3rdparty\include -DProtobuf_LIBRARIES=F:\mtcnn_ncnn\3rdparty\lib\libprotobuf.lib -DProtobuf_PROTOC_EXECUTABLE=F:\mtcnn_ncnn\3rdparty\bin\protoc.exe
pause
```

运行后生成ncnn.sln工程, 生成解决方案，编译Release版本；
编译后，build目录下生成的src中包含ncnn.lib库，tools里有caffe以及mxnet的转换工具；
为了以后方便使用，调用下copyNcnn.bat统一放到公共目录3rdparty目录下，主要移动了caffe2ncnn的exe文件，ncnn的lib库及.h头文件；


```
cd ..
set path=%cd%
copy %path%\3rdparty\src\ncnn\build\tools\caffe\Release\caffe2ncnn.exe %path%\3rdparty\bin\caffe2ncnn.exe
copy %path%\3rdparty\src\ncnn\build\src\Release\ncnn.lib %path%\3rdparty\lib\ncnn.lib

mkdir %path%\3rdparty\include\ncnn
copy %path%\3rdparty\src\ncnn\build\src\layer_type_enum.h %path%\3rdparty\include\ncnn\layer_type_enum.h
copy %path%\3rdparty\src\ncnn\build\src\layer_registry.h %path%\3rdparty\include\ncnn\layer_registry.h
copy %path%\3rdparty\src\ncnn\build\src\layer_declaration.h %path%\3rdparty\include\ncnn\layer_declaration.h
copy %path%\3rdparty\src\ncnn\build\src\platform.h %path%\3rdparty\include\ncnn\platform.h

copy %path%\3rdparty\src\ncnn\src\layer.h %path%\3rdparty\include\ncnn\layer.h
copy %path%\3rdparty\src\ncnn\src\blob.h %path%\3rdparty\include\ncnn\blob.h
copy %path%\3rdparty\src\ncnn\src\cpu.h %path%\3rdparty\include\ncnn\cpu.h
copy %path%\3rdparty\src\ncnn\src\mat.h %path%\3rdparty\include\ncnn\mat.h
copy %path%\3rdparty\src\ncnn\src\net.h %path%\3rdparty\include\ncnn\net.h
copy %path%\3rdparty\src\ncnn\src\opencv.h %path%\3rdparty\include\ncnn\opencv.h
copy %path%\3rdparty\src\ncnn\src\paramdict.h %path%\3rdparty\include\ncnn\paramdict.h

pause
```
4.  编译调试MTCNN

接下来首先可以开始转换MTCNN的caffe模型，调用格式为：

caffe2ncnn.exe  xx.prototxt xx.caffemodel xx.param xx.bin
启动mtcnn2ncnn.bat脚本，即可将mtcnn目录下的model文件都转化为ncnn模型存储方式。
```
cd ..
set path=%cd%
%path%\3rdparty\bin\caffe2ncnn.exe %path%/mtcnn/model/det1.prototxt %path%/mtcnn/model/det1.caffemodel %path%/mtcnn/model/det1.param %path%/mtcnn/model/det1.bin
%path%\3rdparty\bin\caffe2ncnn.exe %path%/mtcnn/model/det2.prototxt %path%/mtcnn/model/det2.caffemodel %path%/mtcnn/model/det2.param %path%/mtcnn/model/det2.bin
%path%\3rdparty\bin\caffe2ncnn.exe %path%/mtcnn/model/det3.prototxt %path%/mtcnn/model/det3.caffemodel %path%/mtcnn/model/det3.param %path%/mtcnn/model/det3.bin

```

一切看似很顺利，麻烦的是，mtcnn模型是caffe+matlab训练的，生成的是col-major模型，与ncnn模型默认的row-major不匹配，参考
[ElegantGod的ncnn](https://github.com/ElegantGod/ncnn)的ncnn改进，提取了其中转化准则文件，放tools目录下的caffe2ncnn.cpp文件，接着替换ncnn的tools/caffe同文件，重新生成caffe2ncnn.exe，并依次执行一次以上模型转换步骤。（ps:Android工程里有转换好的模型，懒的朋友直接拷贝）


生成正确的ncnn模型后，主要就是建立vs工程进行调试，可以vs新建工程，添加包含目录导入3rdparty的opencv及ncnn头文件目录，接着在链接器里添加两者的lib库引用；

当然为了更好学习脚本，个人仍采用cmake的构建方式，MTCNN源码主要基于[Longqi-S](https://github.com/Longqi-S/ncnn-mtcnn)的linux版本就行修改，ncnn的使用方法也可以多参考下ncnn源码目录的example例子；

CmakeList.txt编译介绍如下：
```
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
```

同样的，为了构造工程，执行ncnnBuild.bat的脚本创建vs2015工程

```
cd ..
mkdir vs2015
cd vs2015
cmake .. -G"Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Release
pause
```

创建打开vs2015目录下可生成mtcnn—ncnn.sln,编译Rlease版本，缺少的dll文件可在3rdpatry的bin目录找到；

对应自己的模型，结合ncnn的example的范例，多熟悉下ncnn的模型导入及前置计算，pc端的调试测试就大概完成了。

附粗略实测时间（战神z7）：


pc端速度  | 时间
---|---
squeezenet（原始例子）| 298ms
mtcnn（最小人脸40）| 32ms


#  安卓端调试：
ncnn的安卓端源码范例主要采用的mk文件构造，win开发安卓端大家通常使用AS的cmake来构造工程，下面主要简单介绍相关流程，具体细节参考mtcnnn-AS工程；


1. 新建工程
参考网上配置andorid studio的c++混编环境，新建一个mtcnn—AS的工程；

2. 配置相关文件位置（ps：最新的lib会更快）
	- 下载ncnn的release里的[安卓端lib](https://github.com/Tencent/ncnn/releases)，
	- 将arm端的.a文件放至相关jniLibs对应目录下；
	- include的头文件放至cpp目录下；
	- 将mtcnn的c++的接口文件放在cpp目录下；

3. 新建jni接口文件，相关方法自行参考网上其他教程；

4. CmakeList文件的编写：

```
cmake_minimum_required(VERSION 3.4.1)

#include头文件目录
include_directories(src/main/cpp/include
                    src/main/cpp/)

#source directory源文件目录
file(GLOB MTCNN_SRC src/main/cpp/*.h
                    src/main/cpp/*.cpp)
set(MTCNN_COMPILE_CODE ${MTCNN_SRC})

#添加ncnn库
add_library(libncnn STATIC IMPORTED )
set_target_properties(libncnn
  PROPERTIES IMPORTED_LOCATION
  ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libncnn.a)

#编译为动态库
add_library(mtcnn SHARED ${MTCNN_COMPILE_CODE})

#添加工程所依赖的库
find_library(  log-lib log )
target_link_libraries(  mtcnn
                       libncnn
                       jnigraphics
                       z
                       ${log-lib} )

```
5.成功编译mtcnn的so库，在安卓的MainActivity编写接口使用的相关操作；


附粗略实测时间（高通625）：


安卓端速度  | 时间
---|---
squeezenet（原始例子）| 121ms
mtcnn（最小人脸40）| 47ms