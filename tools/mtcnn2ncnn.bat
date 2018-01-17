cd ..
set path=%cd%
%path%\3rdparty\bin\caffe2ncnn.exe %path%/models/det1.prototxt %path%/models/det1.caffemodel %path%/models/det1.param %path%/models/det1.bin
%path%\3rdparty\bin\caffe2ncnn.exe %path%/models/det2.prototxt %path%/models/det2.caffemodel %path%/models/det2.param %path%/models/det2.bin
%path%\3rdparty\bin\caffe2ncnn.exe %path%/models/det3.prototxt %path%/models/det3.caffemodel %path%/models/det3.param %path%/models/det3.bin

pause