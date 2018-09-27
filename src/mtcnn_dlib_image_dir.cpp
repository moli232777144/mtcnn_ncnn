#include "mtcnn.h"
#include <opencv2/opencv.hpp>
#include <sys/time.h>
#include <dlib/image_io.h>
#include <dlib/gui_widgets.h>

using namespace cv;

#define MAXFACEOPEN 1 //�����Ƿ񿪹�����������ԣ�1Ϊ��������Ϊ��


static float getElapse(struct timeval *tv1,struct timeval *tv2){
    float t = 0.0f;
    if (tv1->tv_sec == tv2->tv_sec)
        t = (tv2->tv_usec - tv1->tv_usec)/1000.0f;
    else
        t = ((tv2->tv_sec - tv1->tv_sec) * 1000 * 1000 + tv2->tv_usec - tv1->tv_usec)/1000.0f;
    return t;
}

int test_picture() {
    char *model_path = "../models";
    MTCNN mtcnn(model_path);

    struct timeval  tv1,tv2;
    struct timezone tz1,tz2;
    gettimeofday(&tv1,&tz1);

    dlib::array2d<dlib::rgb_pixel> img;
    dlib::load_image(img, "../sample.jpg");
    dlib::rgb_pixel* pImg = &(*img.begin());

    ncnn::Mat ncnn_img = ncnn::Mat::from_pixels((const unsigned char*)pImg, ncnn::Mat::PIXEL_RGB, img.nr(), img.nc());
    std::vector<Bbox> finalBbox;

#if(MAXFACEOPEN == 1)
    mtcnn.detectMaxFace(ncnn_img, finalBbox);
#else
    mtcnn.detect(ncnn_img, finalBbox);
#endif
/*
    const int num_box = finalBbox.size();
    std::vector<cv::Rect> bbox;
    bbox.resize(num_box);
    for (int i = 0; i < num_box; i++) {
        bbox[i] = cv::Rect(finalBbox[i].x1, finalBbox[i].y1, finalBbox[i].x2 - finalBbox[i].x1 + 1,
                           finalBbox[i].y2 - finalBbox[i].y1 + 1);

        for (int j = 0; j < 5; j = j + 1) {
            cv::circle(image, cv::Point(finalBbox[i].ppoint[j], finalBbox[i].ppoint[j + 5]), 2, CV_RGB(0, 255, 0),
                       cv::FILLED);
        }
    }
    for (vector<cv::Rect>::iterator it = bbox.begin(); it != bbox.end(); it++) {
        rectangle(image, (*it), Scalar(0, 0, 255), 2, 8, 0);
    }
*/
    gettimeofday(&tv2,&tz2);
    printf( "%s = %g ms \n ", "Detection All time", getElapse(&tv1, &tv2) );

    //dlib::image_window win(img, "Image");
    //win.wait_until_closed();
}

int main(int argc, char **argv) {
    test_picture();
    return 0;
}