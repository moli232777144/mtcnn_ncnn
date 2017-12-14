# 前言
  ncnn是腾讯优图在七月份开源的，一款手机端极致优化的前向计算框架；开源有几个月了，仍然是开源里的扛把子（给nihui大佬递茶）。之前也测试移植过，这次主要做个整理，鉴于很多人不太熟悉如何在window下搭建并使用该框架，本次主要基于MTCNN的人脸检测例子，进行一次该框架的搭建；
  

---
##    主要环境：
- git
- cmake
- vs2015
- android studio

---

#  PC端调试：
pc端调试，繁琐的是依赖库的生成和引用，下面主要用脚本生成，简单演示下主要流程。主要依赖库中Protobuf是caffe的模型序列化存储的规则库，将caffe框架转ncnn框架模型用到，另外opencv库主要用于范例的图像读取操作，可自己配置，或直接使用个人在3rdparty文件夹下编译好的库；

1. 下载源码并更新子模块

```
git clone https://github.com/moli232777144/mtcnn_ncnn.git
git submodule update --ini
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
 
 调用tools下的copydeps脚本，生成protobuf的依赖库到第三方公共文件夹3rdparty下。
 
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

copy %path%\3rdparty\src\ncnn\src\blob.h %path%\3rdparty\include\ncnn\blob.h
copy %path%\3rdparty\src\ncnn\src\cpu.h %path%\3rdparty\include\ncnn\cpu.h
copy %path%\3rdparty\src\ncnn\src\mat.h %path%\3rdparty\include\ncnn\mat.h
copy %path%\3rdparty\src\ncnn\src\net.h %path%\3rdparty\include\ncnn\net.h
copy %path%\3rdparty\src\ncnn\src\opencv.h %path%\3rdparty\include\ncnn\opencv.h
copy %path%\3rdparty\src\ncnn\src\paramdict.h %path%\3rdparty\include\ncnn\paramdict.h

pause
```

接下来可以开始转换caffe模型，调用格式为：

caffe2ncnn.exe  xx.prototxt xx.caffemodel xx.param xx.bin
调用mtcnn2ncnn.bat脚本，即可将mtcnn目录下的model文件都转化为ncnn模型存储方式。
```
cd ..
set path=%cd%
%path%\3rdparty\bin\caffe2ncnn.exe %path%/mtcnn/model/det1.prototxt %path%/mtcnn/model/det1.caffemodel %path%/mtcnn/model/det1.param %path%/mtcnn/model/det1.bin
%path%\3rdparty\bin\caffe2ncnn.exe %path%/mtcnn/model/det2.prototxt %path%/mtcnn/model/det2.caffemodel %path%/mtcnn/model/det2.param %path%/mtcnn/model/det2.bin
%path%\3rdparty\bin\caffe2ncnn.exe %path%/mtcnn/model/det3.prototxt %path%/mtcnn/model/det3.caffemodel %path%/mtcnn/model/det3.param %path%/mtcnn/model/det3.bin

```






