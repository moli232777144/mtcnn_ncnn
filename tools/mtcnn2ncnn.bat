cd ..
set path=%cd%
%path%\3rdparty\src\ncnn\build\tools\caffe\Release\caffe2ncnn.exe %path%/mtcnn/model/det1.prototxt %path%/mtcnn/model/det1.caffemodel %path%/mtcnn/model/det1.param %path%/mtcnn/model/det1.bin
%path%\3rdparty\src\ncnn\build\tools\caffe\Release\caffe2ncnn.exe %path%/mtcnn/model/det2.prototxt %path%/mtcnn/model/det2.caffemodel %path%/mtcnn/model/det2.param %path%/mtcnn/model/det2.bin
%path%\3rdparty\src\ncnn\build\tools\caffe\Release\caffe2ncnn.exe %path%/mtcnn/model/det3.prototxt %path%/mtcnn/model/det3.caffemodel %path%/mtcnn/model/det3.param %path%/mtcnn/model/det3.bin

pause