#include "helper_functions.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <libutils/rasserts.h>
using namespace std;
using namespace cv;

cv::Mat makeAllBlackPixelsBlue(cv::Mat image) {
    // TODO реализуйте функцию которая каждый черный пиксель картинки сделает синим

    int height = image.rows, width = image.cols;
    for(int i = 0; i<height; ++i) {
        for (int j = 0; j < width; ++j) {
            Vec3b color = image.at<Vec3b>(i, j);
            unsigned char b = color[0], g = color[1], r = color[2];
            if (r < 30 && g < 30 && b < 30) {
                b = 255;
                r = g = 0;
            }
            image.at<Vec3b>(i, j) = Vec3b(b, g, r);
        }
    }

    // ниже приведен пример как узнать цвет отдельного пикселя - состоящий из тройки чисел BGR (Blue Green Red)
    // чем больше значение одного из трех чисел - тем насыщеннее его оттенок
    // всего их диапазон значений - от 0 до 255 включительно
    // т.е. один байт, поэтому мы используем ниже тип unsigned char - целое однобайтовое неотрицательное число
//    cv::Vec3b color = image.at<cv::Vec3b>(13, 5); // взяли и узнали что за цвет в пикселе в 14-ом ряду (т.к. индексация с нуля) и 6-ой колонке
//    unsigned char blue = color[0]; // если это число равно 255 - в пикселе много синего, если равно 0 - в пикселе нет синего
//    unsigned char green = color[1];
//    unsigned char red = color[2];
//
//    // как получить белый цвет? как получить черный цвет? как получить желтый цвет?
//    // поэкспериментируйте! например можете всю картинку заполнить каким-то одним цветом
//
//    // пример как заменить цвет по тем же координатам
//    red = 255;
//    // запустите эту версию функции и посмотрите на получившуюся картинку - lesson03/resultsData/01_blue_unicorn.jpg
//    // какой пиксель изменился? почему он не чисто красный?
//    image.at<cv::Vec3b>(13, 5) = cv::Vec3b(blue, green, red);

    return image;
}

cv::Mat invertImageColors(cv::Mat image) {
    // TODO реализуйте функцию которая каждый цвет картинки инвертирует:
    // т.е. пусть ночь станет днем, а сумрак рассеется
    // иначе говоря замените каждое значение яркости x на (255-x) (т.к находится в диапазоне от 0 до 255)

    int height = image.rows, width = image.cols;
    for(int i = 0; i<height; ++i) {
        for (int j = 0; j < width; ++j) {
            Vec3b color = image.at<Vec3b>(i, j);
            unsigned char b = color[0], g = color[1], r = color[2];
            b = 255 - b;
            g = 255 - g;
            r = 255 - r;
            image.at<Vec3b>(i, j) = Vec3b(b, g, r);
        }
    }


    return image;
}

cv::Mat addBackgroundInsteadOfBlackPixels(cv::Mat object, cv::Mat background) {
    // TODO реализуйте функцию которая все черные пиксели картинки-объекта заменяет на пиксели с картинки-фона
    // т.е. что-то вроде накладного фона получится

    // гарантируется что размеры картинок совпадают - проверьте это через rassert, вот например сверка ширины:

    rassert(object.cols == background.cols, 341241251251351);
    rassert(object.rows == background.rows, 341241251251351);

    int height = object.rows, width = object.cols;
    for(int i = 0; i<height; ++i) {
        for (int j = 0; j < width; ++j) {
            Vec3b color = object.at<Vec3b>(i, j);
            Vec3b color1 = background.at<Vec3b>(i,j);
            unsigned char b = color[0], g = color[1], r = color[2];
            if(b<30 && r<30 && g<30){
                object.at<Vec3b>(i,j) = color1;
            }
        }
    }


    return object;
}

cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackground(cv::Mat object, cv::Mat largeBackground) {
    // теперь вам гарантируется что largeBackground гораздо больше - добавьте проверок этого инварианта (rassert-ов)

    // TODO реализуйте функцию так, чтобы нарисовался объект ровно по центру на данном фоне, при этом черные пиксели объекта не должны быть нарисованы
    int height = object.rows, width = object.cols;
    int height1 = largeBackground.rows, width1 = largeBackground.cols;

    for(int i = 0; i<height; ++i) {
        for (int j = 0; j < width; ++j) {
            Vec3b color = object.at<Vec3b>(i, j);
            unsigned char b = color[0], g = color[1], r = color[2];
            if(!(b<30 && r<30 && g<30)){
                largeBackground.at<Vec3b>(i+(height1-height)/2, j + (width1-width)/2) = color;
            }
        }
    }

    return largeBackground;

}


Mat drawNUnicorns(Mat object, Mat background, int n){
    srand((unsigned)time(0));
    for(int i=0;i<n;++i){
        int x = (rand() % (background.cols - object.cols-5));
        int y = (rand() % (background.rows - object.rows-5));
        for(int  j = 0;j<object.rows;++j){
            for(int k = 0; k<object.cols;++k){
                Vec3b color = object.at<Vec3b>(j, k);
                unsigned char b = color[0], g = color[1], r = color[2];
                if(!(b<30 && r<30 && g<30)){
                    rassert(y+j>=0 && y+j<=background.rows,2131441)
                    background.at<Vec3b>(y + j, x + k) = color;
                }
            }
        }
    }
    return background;
}


Mat makeBlackPixelsRand(Mat img){
    srand((unsigned)time(0));
    for(int i = 0;i<img.rows;++i){
        for(int j = 0;j<img.cols;++j){
            Vec3b color = img.at<Vec3b>(i,j);
            unsigned char b = color[0], g = color[1], r = color[2];
            if(b<30 && r<30 && g<30){
                r = rand()%256;
                g = rand()%256;
                b = rand()%256;
                img.at<Vec3b>(i,j) = Vec3b(b,g,r);
            }
        }
    }return img;
}



vector<vector<int>> dilate(vector<vector<int>> mask, int r){ //0 - обьект
    for(int i=0;i<mask.size();++i){
        for(int j = 0;j<mask[i].size();++j){
            for(int dy = -r;dy<=r;++dy){
                for(int dx = -r;dx<=r;++dx){
                    int y = i + dy;
                    int x = j + dx;
                    if(y<0 || y>=mask.size()) continue;
                    if(x<0 || x>=mask[i].size()) continue;
                    if(mask[y][x] == 0){
                        mask[i][j] = 0;
                    }
                }
            }
        }
    }return mask;
}


vector<vector<int>> erode(vector<vector<int>> mask, int r){ //0 - обьект
    for(int i=0;i<mask.size();++i){
        for(int j = 0;j<mask[i].size();++j){
            for(int dy = -r;dy<=r;++dy){
                for(int dx = -r;dx<=r;++dx){
                    int y = i + dy;
                    int x = j + dx;
                    if(y<0 || y>=mask.size()) continue;
                    if(x<0 || x>=mask[i].size()) continue;
                    if(mask[y][x] == 1){
                        mask[i][j] = 1;
                    }
                }
            }
        }
    }return mask;
}



bool equeal(Vec3b c1, Vec3b c2){
    if(abs(c1[0] - c2[0])<20 && abs(c1[1] - c2[1])<20 && abs(c1[2] - c2[2])<20){
        return 1;
    }else return 0;
}