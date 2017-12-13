# mtcnn_ncnn

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

1. 编译protubuf
调用tools下的protubuf脚本编译

```
cd ../3rdparty/src/protobuf/cmake
mkdir build
cd build
cmake .. -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_MSVC_STATIC_RUNTIME=OFF -G "Visual Studio 14 2015 Win64"
pause
```

 vs2015打开./3rdparty/src/protobuf/cmake/build下的protobuf.sln工程，编译DDebug及Release版本
 
 调用tools下的copydeps脚本，生成protobuf的依赖库到公共文件夹
 
2.  编译ncnn

修改./3rdparty/src/ncnn目录下的CMakeLists.txt中的add_definitions相关注释，修改后结果如下：

```
add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
#add_definitions(-Wall -Wextra)
#add_definitions(-fPIC)
#add_definitions(-Ofast)
#add_definitions(-ffast-math)
# add_definitions(-march=native)
# add_definitions(-flto)
#add_definitions(-fvisibility=hidden -fvisibility-inlines-hidden)
```


因为 VS2015 只支持 openmp 2.0，所以修改 src/layer/convolutiondepthwise.cpp

```

#pragma omp parallel for collapse(2)
```

去掉 collapse(2)

```
#pragma omp parallel for
```

接着调用tools下的ncnn.bat工具
可以生成ncnn.sln工程, 编译Release版本；



