cd ../3rdparty/src/ncnn
mkdir build
cd build
cmake .. -G"Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%cd%/install -DProtobuf_INCLUDE_DIR=F:\mtcnn_ncnn\3rdparty\include -DProtobuf_LIBRARIES=F:\mtcnn_ncnn\3rdparty\lib\libprotobuf.lib -DProtobuf_PROTOC_EXECUTABLE=F:\mtcnn_ncnn\3rdparty\bin\protoc.exe
pause