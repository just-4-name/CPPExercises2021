#include "blur.h"

#define _USE_MATH_DEFINES

#include <libutils/rasserts.h>
#include <math.h>

using namespace std;
using namespace cv;

double g(double x, double y, double sigma){
    return exp(-(x*x + y*y)/(2*sigma*sigma));
}

cv::Mat blur(cv::Mat img, double sigma) {
    // TODO формулу весов можно найти тут:
    // https://ru.wikipedia.org/wiki/%D0%A0%D0%B0%D0%B7%D0%BC%D1%8B%D1%82%D0%B8%D0%B5_%D0%BF%D0%BE_%D0%93%D0%B0%D1%83%D1%81%D1%81%D1%83
    int height = img.rows, width = img.cols;
    int cnt = 0, cnt1 = 0;
    for(int i=0;i<height;++i){
        for(int j=0;j<width;++j){
            Vec3d res;
            for(int m=0;m<3;++m) res[m] = 0;
            double sum = 0;
            for(int di = -sigma; di<=sigma; ++di){
                for(int dj = -sigma; dj<=sigma; ++dj){
                    if(di*di + dj*dj > sigma) continue;
                    int y = i + di, x = j + dj;
                    if(x<0 || x>=width || y<0 || y>=height) continue;
                    double k = g(di, dj, sigma);
                    rassert(k!=0,12345)
                    sum+=k;
//                    for(int m = 0;m<3;++m){
//                        res[m] += (double)k*img.at<Vec3b>(y,x)[m];
//                        if(res[m]>10) cnt1++;
//                    }
                    res += k*img.at<Vec3b>(y,x);
                    if(res[0]>10) cnt1++;
                }
            }
            res /= (2*sigma*sigma*M_PI*sum);
            if(res[0]<=1) cnt++;
            img.at<Vec3b>(i,j) = res;
        }
    }
    //rassert(cnt<width*height*4/5,12345)
    rassert(cnt1>200,1235245)
    return img;
}
