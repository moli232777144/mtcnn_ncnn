#include "mtcnn.h"
#include <opencv2/opencv.hpp>
#include <sys/time.h>

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

void test_video() {
    char *model_path = "../models";
    MTCNN mtcnn(model_path);
    mtcnn.SetMinFace(40);
    cv::VideoCapture mVideoCapture(0);
    if (!mVideoCapture.isOpened()) {
        return;
    }
    cv::Mat frame;
    mVideoCapture >> frame;
    while (!frame.empty()) {
        mVideoCapture >> frame;
        if (frame.empty()) {
            break;
        }

        struct timeval  tv1,tv2;
        struct timezone tz1,tz2;
        gettimeofday(&tv1,&tz1);

        ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(frame.data, ncnn::Mat::PIXEL_BGR2RGB, frame.cols, frame.rows);
        std::vector<Bbox> finalBbox;
#if(MAXFACEOPEN == 1)
        mtcnn.detectMaxFace(ncnn_img, finalBbox);
#else
        mtcnn.detect(ncnn_img, finalBbox);
#endif
        gettimeofday(&tv2,&tz2);
        printf( "%s = %g ms \n ", "Detection All time", getElapse(&tv1, &tv2) );
        const int num_box = finalBbox.size();
        std::vector<cv::Rect> bbox;
        bbox.resize(num_box);
        for (int i = 0; i < num_box; i++) {
            bbox[i] = cv::Rect(finalBbox[i].x1, finalBbox[i].y1, finalBbox[i].x2 - finalBbox[i].x1 + 1,
                               finalBbox[i].y2 - finalBbox[i].y1 + 1);

            for (int j = 0; j < 5; j = j + 1) {
                cv::circle(frame, cv::Point(finalBbox[i].ppoint[j], finalBbox[i].ppoint[j + 5]), 2, CV_RGB(0, 255, 0),
                           cv::FILLED);
            }
        }
        for (vector<cv::Rect>::iterator it = bbox.begin(); it != bbox.end(); it++) {
            rectangle(frame, (*it), Scalar(0, 0, 255), 2, 8, 0);
        }

        imshow("face_detection", frame);

        int q = cv::waitKey(10);
        if (q == 27) {
            break;
        }
    }
    return;
}

int test_picture() {
    char *model_path = "../models";
    MTCNN mtcnn(model_path);

    struct timeval  tv1,tv2;
    struct timezone tz1,tz2;
    gettimeofday(&tv1,&tz1);

    cv::Mat image;
    image = cv::imread("../sample.jpg");
    ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(image.data, ncnn::Mat::PIXEL_BGR2RGB, image.cols, image.rows);
    std::vector<Bbox> finalBbox;

#if(MAXFACEOPEN == 1)
    mtcnn.detectMaxFace(ncnn_img, finalBbox);
#else
    mtcnn.detect(ncnn_img, finalBbox);
#endif

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

    imshow("face_detection", image);
    gettimeofday(&tv2,&tz2);
    printf( "%s = %g ms \n ", "Detection All time", getElapse(&tv1, &tv2) );

    cv::waitKey(0);

}

int main(int argc, char **argv) {

    test_video();
    return 0;
}