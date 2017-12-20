
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
copy %path%\3rdparty\src\ncnn\src\layer.h %path%\3rdparty\include\ncnn\layer.h
copy %path%\3rdparty\src\ncnn\src\cpu.h %path%\3rdparty\include\ncnn\cpu.h
copy %path%\3rdparty\src\ncnn\src\mat.h %path%\3rdparty\include\ncnn\mat.h
copy %path%\3rdparty\src\ncnn\src\net.h %path%\3rdparty\include\ncnn\net.h
copy %path%\3rdparty\src\ncnn\src\opencv.h %path%\3rdparty\include\ncnn\opencv.h
copy %path%\3rdparty\src\ncnn\src\paramdict.h %path%\3rdparty\include\ncnn\paramdict.h

pause