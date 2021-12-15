#include <opencv2/imgproc.hpp>
#include "parseSymbols.h"


bool compy(cv::Rect box1,cv::Rect box2){
    if(box1.y>box2.y){
        return 1;
    }return 0;
}

bool compx(cv::Rect box1,cv::Rect box2){
    if(box1.x>box2.x){
        return 1;
    }return 0;
}

std::vector<cv::Mat> splitSymbols(cv::Mat img)
{

    std::vector<cv::Mat> symbols;

    cv::Mat img2;
    cv::cvtColor(img.clone(), img2, cv::COLOR_BGR2GRAY);
    // TODO 101: чтобы извлечь кусок картинки (для каждого прямоугольника cv::Rect вокруг символа) - загуглите "opencv how to extract subimage"
    cv::Mat binary;
    cv::adaptiveThreshold(img2, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 10);
    cv::Mat binary_eroded;
    cv::erode(binary, binary_eroded, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2)));
    std::vector<std::vector<cv::Point>> contoursPoints2;
    cv::findContours(binary_eroded, contoursPoints2,cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);


    std::vector<cv::Rect> boxes;
    std::vector<int> hs;

    for(auto points: contoursPoints2){
        cv::Rect box = cv::boundingRect(points);
        boxes.push_back(box);
        hs.push_back(box.height);
    }

    sort(boxes.begin(), boxes.end(), compy);
    int height = hs[hs.size()/2];
    int last = boxes[0].y;
    std::vector<std::vector<cv::Rect>> sorted(boxes.size());
    int ind = 0;
    for(auto box: boxes){
        if(box.y - last<height){
            sorted[ind].push_back(box);
        }else{
            ind++;
            last = box.y;
            sorted[ind].push_back(box);
        }
    }for(int i = 0;i<sorted.size();++i){
        sort(sorted[i].begin(), sorted[i].end(), compx);
    }

    for(int i=0;i<sorted.size();++i){
        for(int j = 0;j<sorted[i].size();++j){
            cv::Mat subImage(img,sorted[i][j]);
            symbols.push_back(subImage);
        }
    }
    return symbols;
}
