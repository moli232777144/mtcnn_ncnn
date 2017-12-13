cd ..
set path=%cd%

cd 3rdparty/src/ncnn
mkdir build
cd build
cmake -G"Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%cd%/install -DProtobuf_INCLUDE_DIR=%path%\3rdparty\include -DProtobuf_LIBRARIES=%path%\3rdparty\lib\libprotobuf.lib -DProtobuf_PROTOC_EXECUTABLE=.%path%\3rdparty\bin\protoc.exe ..
pause