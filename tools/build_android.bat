@echo off
%%修改ndk所在目录，15以上版本
set ANDROID_NDK=E:\AndroidSDK\ndk-bundle

cd ../3rdparty/src/ncnn
mkdir build_android
cd build_android
cmake -G "Unix Makefiles" -DANDROID_TOOLCHAIN=clang -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK%\build\cmake\android.toolchain.cmake" ..\ -DCMAKE_MAKE_PROGRAM="%ANDROID_NDK%/prebuilt/windows-x86_64/bin/make.exe" -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="armeabi-v7a" -DANDROID_ARM_NEON=ON -DANDROID_PLATFORM=android-16 -DANDROID_NATIVE_API_LEVEL=16 ..
cmake --build .
cmake --build . --target install
cd ..
pause