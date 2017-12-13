cd ../3rdparty/src/protobuf/cmake
mkdir build
cd build
cmake .. -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_MSVC_STATIC_RUNTIME=OFF -G "Visual Studio 14 2015 Win64"
pause