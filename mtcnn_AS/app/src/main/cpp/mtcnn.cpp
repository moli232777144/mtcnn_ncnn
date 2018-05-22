//
// Created by Longqi on 2017/11/18..
//

/*
 * TO DO : change the P-net and update the generat box
 */

#include "mtcnn.h"

#include <android/log.h>
#define TAG "MtcnnCpp"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
static unsigned long get_current_time(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (tv.tv_sec*1000000 + tv.tv_usec);
}

bool cmpScore(Bbox lsh, Bbox rsh) {
	if (lsh.score < rsh.score)
		return true;
	else
		return false;
}

bool cmpArea(Bbox lsh, Bbox rsh) {
    if (lsh.area < rsh.area)
        return false;
    else
        return true;
}


//MTCNN::MTCNN(){}
MTCNN::MTCNN(const string &model_path) {

	std::vector<std::string> param_files = {
		model_path+"/det1.param",
		model_path+"/det2.param",
		model_path+"/det3.param"
	};

	std::vector<std::string> bin_files = {
		model_path+"/det1.bin",
		model_path+"/det2.bin",
		model_path+"/det3.bin"
	};

    Pnet.load_param(param_files[0].data());
    Pnet.load_model(bin_files[0].data());
	Rnet.load_param(param_files[1].data());
	Rnet.load_model(bin_files[1].data());
	Onet.load_param(param_files[2].data());
	Onet.load_model(bin_files[2].data());
}

MTCNN::MTCNN(const std::vector<std::string> param_files, const std::vector<std::string> bin_files){
    Pnet.load_param(param_files[0].data());
    Pnet.load_model(bin_files[0].data());
    Rnet.load_param(param_files[1].data());
    Rnet.load_model(bin_files[1].data());
    Onet.load_param(param_files[2].data());
    Onet.load_model(bin_files[2].data());
}


MTCNN::~MTCNN(){
    Pnet.clear();
    Rnet.clear();
    Onet.clear();
}

void MTCNN::SetMinFace(int minSize){
	minsize = minSize;
}

void MTCNN::SetNumThreads(int numThreads){
    num_threads = numThreads;
}

void MTCNN::SetTimeCount(int timeCount) {
    count = timeCount;
}


void MTCNN::generateBbox(ncnn::Mat score, ncnn::Mat location, std::vector<Bbox>& boundingBox_, float scale){
    const int stride = 2;
    const int cellsize = 12;
    //score p
    float *p = score.channel(1);//score.data + score.cstep;
    //float *plocal = location.data;
    Bbox bbox;
    float inv_scale = 1.0f/scale;
    for(int row=0;row<score.h;row++){
        for(int col=0;col<score.w;col++){
            if(*p>threshold[0]){
                bbox.score = *p;
                bbox.x1 = round((stride*col+1)*inv_scale);
                bbox.y1 = round((stride*row+1)*inv_scale);
                bbox.x2 = round((stride*col+1+cellsize)*inv_scale);
                bbox.y2 = round((stride*row+1+cellsize)*inv_scale);
                bbox.area = (bbox.x2 - bbox.x1) * (bbox.y2 - bbox.y1);
                const int index = row * score.w + col;
                for(int channel=0;channel<4;channel++){
                    bbox.regreCoord[channel]=location.channel(channel)[index];
                }
                boundingBox_.push_back(bbox);
            }
            p++;
            //plocal++;
        }
    }
}


void MTCNN::nmsTwoBoxs(vector<Bbox>& boundingBox_, vector<Bbox>& previousBox_, const float overlap_threshold, string modelname)
{
    if (boundingBox_.empty()) {
        return;
    }
    sort(boundingBox_.begin(), boundingBox_.end(), cmpScore);
    float IOU = 0;
    float maxX = 0;
    float maxY = 0;
    float minX = 0;
    float minY = 0;
    //std::cout << boundingBox_.size() << " ";
    for (std::vector<Bbox>::iterator ity = previousBox_.begin(); ity != previousBox_.end(); ity++) {
        for (std::vector<Bbox>::iterator itx = boundingBox_.begin(); itx != boundingBox_.end();) {
            int i = itx - boundingBox_.begin();
            int j = ity - previousBox_.begin();
            maxX = std::max(boundingBox_.at(i).x1, previousBox_.at(j).x1);
            maxY = std::max(boundingBox_.at(i).y1, previousBox_.at(j).y1);
            minX = std::min(boundingBox_.at(i).x2, previousBox_.at(j).x2);
            minY = std::min(boundingBox_.at(i).y2, previousBox_.at(j).y2);
            //maxX1 and maxY1 reuse
            maxX = ((minX - maxX + 1)>0) ? (minX - maxX + 1) : 0;
            maxY = ((minY - maxY + 1)>0) ? (minY - maxY + 1) : 0;
            //IOU reuse for the area of two bbox
            IOU = maxX * maxY;
            if (!modelname.compare("Union"))
                IOU = IOU / (boundingBox_.at(i).area + previousBox_.at(j).area - IOU);
            else if (!modelname.compare("Min")) {
                IOU = IOU / ((boundingBox_.at(i).area < previousBox_.at(j).area) ? boundingBox_.at(i).area : previousBox_.at(j).area);
            }
            if (IOU > overlap_threshold&&boundingBox_.at(i).score>previousBox_.at(j).score) {
                //if (IOU > overlap_threshold) {
                itx = boundingBox_.erase(itx);
            }
            else {
                itx++;
            }
        }
    }
    //std::cout << boundingBox_.size() << std::endl;
}

void MTCNN::nms(std::vector<Bbox> &boundingBox_, const float overlap_threshold, string modelname){
    if(boundingBox_.empty()){
        return;
    }
    sort(boundingBox_.begin(), boundingBox_.end(), cmpScore);
    float IOU = 0;
    float maxX = 0;
    float maxY = 0;
    float minX = 0;
    float minY = 0;
    std::vector<int> vPick;
    int nPick = 0;
    std::multimap<float, int> vScores;
    const int num_boxes = boundingBox_.size();
	vPick.resize(num_boxes);
	for (int i = 0; i < num_boxes; ++i){
		vScores.insert(std::pair<float, int>(boundingBox_[i].score, i));
	}
    while(vScores.size() > 0){
        int last = vScores.rbegin()->second;
        vPick[nPick] = last;
        nPick += 1;
        for (std::multimap<float, int>::iterator it = vScores.begin(); it != vScores.end();){
            int it_idx = it->second;
            maxX = std::max(boundingBox_.at(it_idx).x1, boundingBox_.at(last).x1);
            maxY = std::max(boundingBox_.at(it_idx).y1, boundingBox_.at(last).y1);
            minX = std::min(boundingBox_.at(it_idx).x2, boundingBox_.at(last).x2);
            minY = std::min(boundingBox_.at(it_idx).y2, boundingBox_.at(last).y2);
            //maxX1 and maxY1 reuse 
            maxX = ((minX-maxX+1)>0)? (minX-maxX+1) : 0;
            maxY = ((minY-maxY+1)>0)? (minY-maxY+1) : 0;
            //IOU reuse for the area of two bbox
            IOU = maxX * maxY;
            if(!modelname.compare("Union"))
                IOU = IOU/(boundingBox_.at(it_idx).area + boundingBox_.at(last).area - IOU);
            else if(!modelname.compare("Min")){
                IOU = IOU/((boundingBox_.at(it_idx).area < boundingBox_.at(last).area)? boundingBox_.at(it_idx).area : boundingBox_.at(last).area);
            }
            if(IOU > overlap_threshold){
                it = vScores.erase(it);
            }else{
                it++;
            }
        }
    }
    
    vPick.resize(nPick);
    std::vector<Bbox> tmp_;
    tmp_.resize(nPick);
    for(int i = 0; i < nPick; i++){
        tmp_[i] = boundingBox_[vPick[i]];
    }
    boundingBox_ = tmp_;
}
void MTCNN::refine(vector<Bbox> &vecBbox, const int &height, const int &width, bool square){
    if(vecBbox.empty()){
        cout<<"Bbox is empty!!"<<endl;
        return;
    }
    float bbw=0, bbh=0, maxSide=0;
    float h = 0, w = 0;
    float x1=0, y1=0, x2=0, y2=0;
    for(vector<Bbox>::iterator it=vecBbox.begin(); it!=vecBbox.end();it++){
        bbw = (*it).x2 - (*it).x1 + 1;
        bbh = (*it).y2 - (*it).y1 + 1;
        x1 = (*it).x1 + (*it).regreCoord[0]*bbw;
        y1 = (*it).y1 + (*it).regreCoord[1]*bbh;
        x2 = (*it).x2 + (*it).regreCoord[2]*bbw;
        y2 = (*it).y2 + (*it).regreCoord[3]*bbh;

        
        
        if(square){
            w = x2 - x1 + 1;
            h = y2 - y1 + 1;
            maxSide = (h>w)?h:w;
            x1 = x1 + w*0.5 - maxSide*0.5;
            y1 = y1 + h*0.5 - maxSide*0.5;
            (*it).x2 = round(x1 + maxSide - 1);
            (*it).y2 = round(y1 + maxSide - 1);
            (*it).x1 = round(x1);
            (*it).y1 = round(y1);
        }

        //boundary check
        if((*it).x1<0)(*it).x1=0;
        if((*it).y1<0)(*it).y1=0;
        if((*it).x2>width)(*it).x2 = width - 1;
        if((*it).y2>height)(*it).y2 = height - 1;

        it->area = (it->x2 - it->x1)*(it->y2 - it->y1);
    }
}

void MTCNN::extractMaxFace(vector<Bbox>& boundingBox_)
{
    if (boundingBox_.empty()) {
        return;
    }
    sort(boundingBox_.begin(), boundingBox_.end(), cmpArea);
    for (std::vector<Bbox>::iterator itx = boundingBox_.begin() + 1; itx != boundingBox_.end();) {
        itx = boundingBox_.erase(itx);
    }
}

void MTCNN::PNet(float scale)
{
    //first stage
    int hs = (int)ceil(img_h*scale);
    int ws = (int)ceil(img_w*scale);
    ncnn::Mat in;
    resize_bilinear(img, in, ws, hs);
    ncnn::Extractor ex = Pnet.create_extractor();
    ex.set_light_mode(true);
    ex.set_num_threads(num_threads);
    ex.input("data", in);
    ncnn::Mat score_, location_;
    ex.extract("prob1", score_);
    ex.extract("conv4-2", location_);
    std::vector<Bbox> boundingBox_;

    generateBbox(score_, location_, boundingBox_, scale);
    nms(boundingBox_, nms_threshold[0]);

    firstBbox_.insert(firstBbox_.end(), boundingBox_.begin(), boundingBox_.end());
    boundingBox_.clear();
}


void MTCNN::PNet(){
    firstBbox_.clear();
    float minl = img_w < img_h? img_w: img_h;
    float m = (float)MIN_DET_SIZE/minsize;
    minl *= m;
    float factor = pre_facetor;
    vector<float> scales_;
    while(minl>MIN_DET_SIZE){
        scales_.push_back(m);
        minl *= factor;
        m = m*factor;
    }
    for (size_t i = 0; i < scales_.size(); i++) {
        int hs = (int)ceil(img_h*scales_[i]);
        int ws = (int)ceil(img_w*scales_[i]);
        ncnn::Mat in;
        resize_bilinear(img, in, ws, hs);
        ncnn::Extractor ex = Pnet.create_extractor();
        ex.set_num_threads(num_threads);
        ex.set_light_mode(true);
        ex.input("data", in);
        ncnn::Mat score_, location_;
        ex.extract("prob1", score_);
        ex.extract("conv4-2", location_);
        std::vector<Bbox> boundingBox_;
        generateBbox(score_, location_, boundingBox_, scales_[i]);
        nms(boundingBox_, nms_threshold[0]);
        firstBbox_.insert(firstBbox_.end(), boundingBox_.begin(), boundingBox_.end());
        boundingBox_.clear();
    }
}
void MTCNN::RNet(){
    secondBbox_.clear();
    int count = 0;
    for(vector<Bbox>::iterator it=firstBbox_.begin(); it!=firstBbox_.end();it++){
        ncnn::Mat tempIm;
        copy_cut_border(img, tempIm, (*it).y1, img_h-(*it).y2, (*it).x1, img_w-(*it).x2);
        ncnn::Mat in;
        resize_bilinear(tempIm, in, 24, 24);
        ncnn::Extractor ex = Rnet.create_extractor();
        ex.set_num_threads(num_threads);
        ex.set_light_mode(true);
        ex.input("data", in);
        ncnn::Mat score, bbox;
        ex.extract("prob1", score);
        ex.extract("conv5-2", bbox);
        if((float)score[1] > threshold[1]){
            for(int channel=0;channel<4;channel++){
                it->regreCoord[channel]=(float)bbox[channel];//*(bbox.data+channel*bbox.cstep);
            }
            it->area = (it->x2 - it->x1)*(it->y2 - it->y1);
            it->score = score.channel(1)[0];//*(score.data+score.cstep);
            secondBbox_.push_back(*it);
        }
    }
}
void MTCNN::ONet(){
    thirdBbox_.clear();
    for(vector<Bbox>::iterator it=secondBbox_.begin(); it!=secondBbox_.end();it++){
        ncnn::Mat tempIm;
        copy_cut_border(img, tempIm, (*it).y1, img_h-(*it).y2, (*it).x1, img_w-(*it).x2);
        ncnn::Mat in;
        resize_bilinear(tempIm, in, 48, 48);
        ncnn::Extractor ex = Onet.create_extractor();
        ex.set_num_threads(num_threads);
        ex.set_light_mode(true);
        ex.input("data", in);
        ncnn::Mat score, bbox, keyPoint;
        ex.extract("prob1", score);
        ex.extract("conv6-2", bbox);
        ex.extract("conv6-3", keyPoint);
        if((float)score[1] > threshold[2]){
            for(int channel = 0; channel < 4; channel++){
                it->regreCoord[channel]=(float)bbox[channel];
            }
            it->area = (it->x2 - it->x1) * (it->y2 - it->y1);
            it->score = score.channel(1)[0];
            for(int num=0;num<5;num++){
                (it->ppoint)[num] = it->x1 + (it->x2 - it->x1) * keyPoint[num];
                (it->ppoint)[num+5] = it->y1 + (it->y2 - it->y1) * keyPoint[num+5];
            }

            thirdBbox_.push_back(*it);
        }
    }
}

#define TIMEOPEN 1 //设置是否开关调试，1为开，其它为关

void MTCNN::detect(ncnn::Mat& img_, std::vector<Bbox>& finalBbox_){
    img = img_;
    img_w = img.w;
    img_h = img.h;
    img.substract_mean_normalize(mean_vals, norm_vals);

#if(TIMEOPEN==1)
    double total_time = 0.;
    double min_time = DBL_MAX;
    double max_time = 0.0;
    double temp_time = 0.0;
    unsigned long time_0, time_1;

    for(int i =0 ;i < count; i++) {
        time_0 = get_current_time();
#endif

        PNet();
        //the first stage's nms
        if(firstBbox_.size() < 1) return;
        nms(firstBbox_, nms_threshold[0]);
        refine(firstBbox_, img_h, img_w, true);
        printf("firstBbox_.size()=%d\n", firstBbox_.size());
        //second stage
        RNet();
        printf("secondBbox_.size()=%d\n", secondBbox_.size());
        if (secondBbox_.size() < 1) return;
        nms(secondBbox_, nms_threshold[1]);
        refine(secondBbox_, img_h, img_w, true);

        //third stage
        ONet();
        printf("thirdBbox_.size()=%d\n", thirdBbox_.size());
        if (thirdBbox_.size() < 1) return;
        refine(thirdBbox_, img_h, img_w, true);
        nms(thirdBbox_, nms_threshold[2], "Min");
        finalBbox_ = thirdBbox_;

#if(TIMEOPEN==1)
        time_1 = get_current_time();
        temp_time = ((time_1 - time_0)/1000.0);
        if(temp_time < min_time)
        {
            min_time = temp_time;
        }
        if(temp_time > max_time)
        {
            max_time = temp_time;
        }
        total_time += temp_time;

        LOGD("iter %d/%d cost: %.3f ms\n", i+1, count, temp_time);
    }
    LOGD("Time cost:Max %.2fms,Min %.2fms,Avg %.2fms\n", max_time,min_time,total_time/count);
#endif

}


void MTCNN::detectMaxFace(ncnn::Mat& img_, std::vector<Bbox>& finalBbox) {
    firstPreviousBbox_.clear();
    secondPreviousBbox_.clear();
    thirdPrevioussBbox_.clear();
    firstBbox_.clear();
    secondBbox_.clear();
    thirdBbox_.clear();

    //norm
    img = img_;
    img_w = img.w;
    img_h = img.h;
    img.substract_mean_normalize(mean_vals, norm_vals);

#if(TIMEOPEN==1)
    double total_time = 0.;
    double min_time = DBL_MAX;
    double max_time = 0.0;
    double temp_time = 0.0;
    unsigned long time_0, time_1;

    for(int i =0 ;i < count; i++) {
        time_0 = get_current_time();
#endif

    //pyramid size
    float minl = img_w < img_h ? img_w : img_h;
    float m = (float)MIN_DET_SIZE / minsize;
    minl *= m;
    float factor = pre_facetor;
    vector<float> scales_;
    while (minl>MIN_DET_SIZE) {
        scales_.push_back(m);
        minl *= factor;
        m = m*factor;
    }
    sort(scales_.begin(), scales_.end());
    //printf("scales_.size()=%d\n", scales_.size());

    //Change the sampling process.
    for (size_t i = 0; i < scales_.size(); i++)
    {
        //first stage
        PNet(scales_[i]);
        nms(firstBbox_, nms_threshold[0]);
        nmsTwoBoxs(firstBbox_, firstPreviousBbox_, nms_threshold[0]);
        if (firstBbox_.size() < 1) {
            firstBbox_.clear();
            continue;
        }
        firstPreviousBbox_.insert(firstPreviousBbox_.end(), firstBbox_.begin(), firstBbox_.end());
        refine(firstBbox_, img_h, img_w, true);
        //printf("firstBbox_.size()=%d\n", firstBbox_.size());

        //second stage
        RNet();
        nms(secondBbox_, nms_threshold[1]);
        nmsTwoBoxs(secondBbox_, secondPreviousBbox_, nms_threshold[0]);
        secondPreviousBbox_.insert(secondPreviousBbox_.end(), secondBbox_.begin(), secondBbox_.end());
        if (secondBbox_.size() < 1) {
            firstBbox_.clear();
            secondBbox_.clear();
            continue;
        }
        refine(secondBbox_, img_h, img_w, true);
        //printf("secondBbox_.size()=%d\n", secondBbox_.size());

        //third stage
        ONet();
        //printf("thirdBbox_.size()=%d\n", thirdBbox_.size());
        if (thirdBbox_.size() < 1) {
            firstBbox_.clear();
            secondBbox_.clear();
            thirdBbox_.clear();
            continue;
        }
        refine(thirdBbox_, img_h, img_w, true);
        nms(thirdBbox_, nms_threshold[2], "Min");

        if (thirdBbox_.size() > 0) {
            extractMaxFace(thirdBbox_);
            finalBbox = thirdBbox_;//if largest face size is similar,.
            break;
        }
    }

    //printf("firstPreviousBbox_.size()=%d\n", firstPreviousBbox_.size());
    //printf("secondPreviousBbox_.size()=%d\n", secondPreviousBbox_.size());

#if(TIMEOPEN==1)
        time_1 = get_current_time();
        temp_time = ((time_1 - time_0)/1000.0);
        if(temp_time < min_time)
        {
            min_time = temp_time;
        }
        if(temp_time > max_time)
        {
            max_time = temp_time;
        }
        total_time += temp_time;

        LOGD("iter %d/%d cost: %.3f ms\n", i+1, count, temp_time);
    }
    LOGD("Time cost:Max %.2fms,Min %.2fms,Avg %.2fms\n", max_time,min_time,total_time/count);
#endif
}

//void MTCNN::detection(const cv::Mat& img, std::vector<cv::Rect>& rectangles){
//    ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_BGR2RGB, img.cols, img.rows);
//    std::vector<Bbox> finalBbox;
//    detect(ncnn_img, finalBbox);
//    const int num_box = finalBbox.size();
//    rectangles.resize(num_box);
//    for(int i = 0; i < num_box; i++){
//        rectangles[i] = cv::Rect(finalBbox[i].x1, finalBbox[i].y1, finalBbox[i].x2 - finalBbox[i].x1 + 1, finalBbox[i].y2 - finalBbox[i].y1 + 1);
//    }
//}
