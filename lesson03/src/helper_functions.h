#pragma once

#include <opencv2/highgui.hpp> // подключили часть библиотеки OpenCV, теперь мы можем работать с картинками (знаем про тип cv::Mat)
using namespace std;


cv::Mat makeAllBlackPixelsBlue(cv::Mat image);

cv::Mat invertImageColors(cv::Mat image);

cv::Mat addBackgroundInsteadOfBlackPixels(cv::Mat object, cv::Mat background);

cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackground(cv::Mat object, cv::Mat largeBackground);


cv::Mat drawNUnicorns(cv::Mat object, cv::Mat background, int n);


cv::Mat makeBlackPixelsRand(cv::Mat img);


vector<vector<int>> dilate(vector<vector<int>> mask, int r);
