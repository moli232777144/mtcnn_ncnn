call ..\3rdparty\src\protobuf\cmake\build\extract_includes.bat
move include\google ..\3rdparty\include\
copy ..\3rdparty\src\protobuf\cmake\build\Debug\libprotobufd.lib ..\3rdparty\lib\libprotobufd.lib
copy ..\3rdparty\src\protobuf\cmake\build\Release\libprotobuf.lib ..\3rdparty\lib\libprotobuf.lib
copy ..\3rdparty\src\protobuf\cmake\build\Release\protoc.exe ..\3rdparty\bin\protoc.exe
pause