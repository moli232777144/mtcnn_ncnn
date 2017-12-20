package com.mtcnn_as;

/**
 * Created by hasee on 2017/12/19.
 */

public class MTCNN{
    //人脸检测模型导入
    public native boolean FaceDetectionModelInit(String faceDetectionModelPath);

    //人脸检测
    public native int[] FaceDetect(byte[] imageDate, int imageWidth , int imageHeight, int imageChannel);

    //人脸检测模型反初始化
    public native boolean FaceDetectionModelUnInit();

    static {
        System.loadLibrary("mtcnn");
    }
}
