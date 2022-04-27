#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include<time.h>
#include<stdlib.h>
#include<random>
#include<cmath>

#include <set>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <memory>

#include <libutils/rasserts.h>
#include <libutils/fast_random.h>



using namespace std;
using namespace cv;


// Эта функция говорит нам правда ли пиксель отмаскирован, т.е. отмечен как "удаленный", т.е. белый
bool isPixelMasked(cv::Mat mask, int j, int i) {
    rassert(j >= 0 && j < mask.rows, 372489347280017);
    rassert(i >= 0 && i < mask.cols, 372489347280018);
    rassert(mask.type() == CV_8UC3, 2348732984792380019);


    Vec3b c = mask.at<Vec3b>(j,i);
    for(int t = 0; t<3; ++t){
        if(c[t] != 255) return false;
    }
    return true;
}


int estimateQuality(Mat mask, Mat mat, int i, int j, int ni, int nj, int h, int w) {
    int res = 0;
    for(int di = -h/2; di <=h/2; ++di) {
        for(int dj = -w/2; dj <= w/2; ++dj) {
            if(di == 0 && dj == 0) continue;
            if(i + di < 0 || ni + di < 0 || i + di >= mat.rows || ni + di >= mat.rows || j + dj < 0 ||
               nj + dj < 0 || j + dj >= mat.cols || nj + dj >= mat.cols) {
                res += 1e7;
                continue;
            }
            if(isPixelMasked(mask, ni + di, nj + dj)){
                res += 1e7;
            }
            Vec3b a = mat.at<Vec3b> (i + di, j + dj), b = mat.at<Vec3b> (ni + di, nj + dj);
            for(int k = 0; k < 3; ++k) {
                res +=(a[k] - b[k]) * (a[k] - b[k]);
            }
        }
    }return res;
}


bool isAreaMasked(Mat mask,int i,int j){
    bool ok = 1;
    for(int di = -2; di <=2; ++di) {
        for (int dj = -2; dj <= 2; ++dj) {
            if(i + di < 0 || i + di >= mask.rows || j + dj < 0 || j + dj >= mask.cols) {
                return true;
            }if(isPixelMasked(mask,i+di,j+dj)) return true;
        }
    }return false;
}


void run(int caseNumber, std::string caseName) {
    //srand((unsigned)time(0));
    std::cout << "_________Case #" << caseNumber << ": " <<  caseName << "_________" << std::endl;

    cv::Mat original = cv::imread("lesson18/data/" + std::to_string(caseNumber) + "_" + caseName + "/" + std::to_string(caseNumber) + "_original.jpg");
    cv::Mat mask = cv::imread("lesson18/data/" + std::to_string(caseNumber) + "_" + caseName + "/" + std::to_string(caseNumber) + "_mask.png");
    rassert(!original.empty(), 324789374290018);
    rassert(!mask.empty(), 378957298420019);

    // TODO напишите rassert сверяющий разрешение картинки и маски
    rassert(mask.rows == original.rows && mask.cols == original.cols, 12312314135)
    // TODO выведите в консоль это разрешение картинки
    std::cout << "Image resolution: " << mask.rows << " " << mask.cols << std::endl;

    // создаем папку в которую будем сохранять результаты - lesson18/resultsData/ИМЯ_НАБОРА/
    std::string resultsDir = "lesson18/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }
    resultsDir += std::to_string(caseNumber) + "_" + caseName + "/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }

    // сохраняем в папку с результатами оригинальную картинку и маску
    cv::imwrite(resultsDir + "0original.png", original);
    cv::imwrite(resultsDir + "1mask.png", mask);

    // TODO замените белым цветом все пиксели в оригинальной картинке которые покрыты маской
    int cnt = 0;
    for(int i = 0; i<mask.rows; ++i){
        for(int j = 0; j<mask.cols; ++j){
            if(isPixelMasked(mask,i,j)){
                cnt++;
                original.at<Vec3b>(i,j) = mask.at<Vec3b>(i,j);
            }
        }
    }
    //cout<<"LULKWKJDOcsbdhv";
    // TODO сохраните в папку с результатами то что получилось под названием "2_original_cleaned.png"
    imwrite(resultsDir + "2_original_cleaned.png", original);
    // TODO посчитайте и выведите число отмаскированных пикселей (числом и в процентах) - в таком формате:
    // Number of masked pixels: 7899/544850 = 1%
    cout<<"Number of masked pixels: "<<cnt<<"/"<<mask.cols * mask.rows<<" = " <<(double)cnt/(mask.cols * mask.rows) * 100<<"%\n";

    FastRandom random(32542341); // этот объект поможет вам генерировать случайные гипотезы
    vector<Mat> pyr, masks;
    Mat mask1 = mask;
    while(original.cols >= 20 && original.rows >= 20) {
        pyr.push_back(original);
        pyrDown(original, original);
    }reverse(pyr.begin(), pyr.end());
    while(mask.cols >= 20 && mask.rows >= 20) {
        masks.push_back(mask);
        pyrDown(mask, mask);
    }reverse(masks.begin(), masks.end());
    mask = mask1;
    int n = pyr.size();
    vector<Mat> shifts(n); // матрица хранящая смещения, изначально заполнена парами нулей
    for(int i = 0; i < n; ++i){
        shifts[i] = Mat(pyr[i].rows, pyr[i].cols, CV_32SC2, Scalar(0,0));
    }
    vector<Mat> images = pyr;



    int q = 100;

    while(q--){
        for(int i = 0; i < pyr[0].rows; ++i){
            for(int j = 0; j < pyr[0].cols; ++j){
                int di,dj;
                while(1){
                    di = random.next(-pyr[0].rows, pyr[0].rows);
                    dj = random.next(-pyr[0].cols, pyr[0].cols);
                    if(i + di < 0 || i + di >= pyr[0].rows || j + dj < 0 || j + dj >= pyr[0].cols) continue;
                    if(isAreaMasked(masks[0], i + di, j + dj)) continue;
                    break;
                }
                Vec2i sh = shifts[0].at<Vec2i>(i,j);
                int currentQuality = estimateQuality(masks[0], images[0], i, j, i + sh[0], j + sh[1], 5, 5);
                int randomQuality = estimateQuality(masks[0], images[0],i,j,i + di,j + dj,5,5);
                if(randomQuality < currentQuality){
                    shifts[0].at<Vec2i>(i,j) = {di,dj};
                    images[0].at<Vec3b>(i,j) = pyr[0].at<Vec3b>(i + di, j + dj);
                }
            }
        }
    }




    for(int lvl = 1; lvl < n; ++lvl){
        cout<<lvl<<" ";
        for(int i = 0; i < images[lvl].rows; ++i){
            for(int j = 0; j < images[lvl].cols; ++j){
                if(isPixelMasked(masks[lvl], i, j)){
                    shifts[lvl].at<Vec2i>(i,j) = shifts[lvl - 1].at<Vec2i>(i/2, j/2);
                    shifts[lvl].at<Vec2i>(i,j)[0]*=2;
                    shifts[lvl].at<Vec2i>(i,j)[1]*=2;
//                    rassert(i + shifts[lvl].at<Vec2i>(i,j)[0] >= 0 && i + shifts[lvl].at<Vec2i>(i,j)[0] < images[lvl].rows,12314135)
//                    rassert(i + shifts[lvl].at<Vec2i>(i,j)[0] >= 0 && i + shifts[lvl].at<Vec2i>(i,j)[0] < images[lvl].rows,12314135)
                }
            }
        }
        int start1 = 0, end1 = images[lvl].rows, step1 = 1;
        int start2 = 0, end2 = images[lvl].cols, step2 = 1;
        q = 100;
        while (q--) {
            for (int i = 0; i != end1; i += step1) {
                for(int j = start2; j != end2; j+= step2){
                    if (!isPixelMasked(masks[lvl],i,j)) continue; // пропускаем т.к. его менять не надо
                    if (i > 0 && isPixelMasked(masks[lvl], i-1, j) && q<90) {
                        cv::Vec2i dxy = shifts[lvl].at<Vec2i> (i,j);//смотрим какое сейчас смещение для этого пикселя в матрице смещения
                        int ni = i + dxy[0], nj = j + dxy[1];
                        int currentQuality = estimateQuality(masks[lvl], images[lvl], i, j, ni, nj, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
                        //if(ni == i && nj == j && isPixelMasked(image,i,j)) currentQuality = 1e9;
                        int di = shifts[lvl].at<Vec2i>(i-1,j)[0] - 1;// на квадрат 5х5 приложенный центром к (nx, ny)
                        int dj = shifts[lvl].at<Vec2i>(i-1,j)[1];
                        int randomQuality = estimateQuality(masks[lvl], images[lvl], i, j, i + di, j + dj, 5, 5); // оцениваем насколько похоже будет если мы приложим эту случайную гипотезу которую только что выбрали
                        if(isAreaMasked(masks[lvl],i+di,j+dj)) randomQuality = 1e9 + 19;
                        if (randomQuality < currentQuality) {
                            shifts[lvl].at<Vec2i>(i,j) = {di,dj};
                            images[lvl].at<Vec3b>(i,j) = pyr[lvl].at<Vec3b>(i + di, j + dj);
                        }
                    }
                    if(j > 0 && isPixelMasked(masks[lvl],i,j-1) && q<90) {
                        cv::Vec2i dxy = shifts[lvl].at<Vec2i> (i,j);//смотрим какое сейчас смещение для этого пикселя в матрице смещения
                        int ni = i + dxy[0], nj = j + dxy[1];
                        int currentQuality = estimateQuality(masks[lvl], images[lvl], i, j, ni, nj, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
                        //if(ni == i && nj == j && isPixelMasked(image,i,j)) currentQuality = 1e9;
                        int di = shifts[lvl].at<Vec2i>(i,j-1)[0];// на квадрат 5х5 приложенный центром к (nx, ny)
                        int dj = shifts[lvl].at<Vec2i>(i,j-1)[1] - 1;
                        int randomQuality = estimateQuality(masks[lvl], images[lvl], i, j, i + di, j + dj, 5, 5);
                        if(isAreaMasked(masks[lvl],i+di,j+dj)) randomQuality = 1e9 + 19;
                        if (randomQuality < currentQuality) {
                            shifts[lvl].at<Vec2i>(i,j) = {di,dj};
                            images[lvl].at<Vec3b>(i,j) = pyr[lvl].at<Vec3b>(i + di, j + dj);
                        }
                    }
                    if(i < images[lvl].rows - 1 && isPixelMasked(masks[lvl], i + 1, j) && q<90) {
                        cv::Vec2i dxy = shifts[lvl].at<Vec2i> (i,j);//смотрим какое сейчас смещение для этого пикселя в матрице смещения
                        int ni = i + dxy[0], nj = j + dxy[1];
                        int currentQuality = estimateQuality(masks[lvl], images[lvl], i, j, ni, nj, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
                        //if(ni == i && nj == j && isPixelMasked(image,i,j)) currentQuality = 1e9;
                        int di = 1 + shifts[lvl].at<Vec2i>(i+1,j)[0];// на квадрат 5х5 приложенный центром к (nx, ny)
                        int dj = shifts[lvl].at<Vec2i>(i+1,j)[1];
                        int randomQuality = estimateQuality(masks[lvl], images[lvl], i, j, i + di, j + dj, 5, 5);
                        if(isAreaMasked(masks[lvl],i+di,j+dj)) randomQuality = 1e9 + 19;
                        if (randomQuality < currentQuality) {
                            shifts[lvl].at<Vec2i>(i,j) = {di,dj};
                            images[lvl].at<Vec3b>(i,j) = pyr[lvl].at<Vec3b>(i + di, j + dj);
                        }
                    }
                    if(j < images[lvl].cols - 1 && isPixelMasked(masks[lvl], i, j + 1) && q < 90){
                        cv::Vec2i dxy = shifts[lvl].at<Vec2i> (i,j);//смотрим какое сейчас смещение для этого пикселя в матрице смещения
                        int ni = i + dxy[0], nj = j + dxy[1];
                        int currentQuality = estimateQuality(masks[lvl], images[lvl], i, j, ni, nj, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
                        //if(ni == i && nj == j && isPixelMasked(image,i,j)) currentQuality = 1e9;
                        int di = shifts[lvl].at<Vec2i>(i,j+1)[0];// на квадрат 5х5 приложенный центром к (nx, ny)
                        int dj = 1 + shifts[lvl].at<Vec2i>(i,j+1)[1];
                        int randomQuality = estimateQuality(masks[lvl], images[lvl], i, j, i + di, j + dj, 5, 5);
                        if(isAreaMasked(masks[lvl],i+di,j+dj)) randomQuality = 1e9 + 19;
                        if (randomQuality < currentQuality) {
                            shifts[lvl].at<Vec2i>(i,j) = {di,dj};
                            images[lvl].at<Vec3b>(i,j) = pyr[lvl].at<Vec3b>(i + di, j + dj);
                        }
                    }
                    cv::Vec2i dxy = shifts[lvl].at<Vec2i> (i,j);//смотрим какое сейчас смещение для этого пикселя в матрице смещения
                    int ni = i + dxy[0], nj = j + dxy[1];
                    int currentQuality = estimateQuality(masks[lvl], images[lvl], i, j, ni, nj, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
                    //if(ni == i && nj == j && isPixelMasked(image,i,j)) currentQuality = 1e9;

                    int dh = images[lvl].rows;
                    int dw = images[lvl].cols;


                    while(!(dh == 0 && dw == 0)){
                        int di,dj;
                        int c = 0;
                        while(c < 20) {
                            c++;
                            bool ok = 1;
                            di = shifts[lvl].at<Vec2i>(i, j)[0] + random.next(-dh, dh);
                            dj = shifts[lvl].at<Vec2i>(i, j)[1] + random.next(-dw, dw);
                            if (i + di < 0 || i + di >= images[lvl].rows) continue;
                            if (j + dj < 0 || j + dj >= images[lvl].cols) continue;
                            if (ok && isPixelMasked(masks[lvl], i + di, j + dj)) continue;
                            for(int dy = -2; dy<= 2; ++ dy){
                                for(int dx = -2; dx <= 2; ++ dx){
                                    if (i + di + dy < 0 || i + di + dy >= images[lvl].rows) {
                                        ok = 0;
                                        continue;
                                    }
                                    if (j + dj + dx< 0 || j + dj + dx>= images[lvl].cols){
                                        ok = 0;
                                        continue;
                                    }
                                    if(isPixelMasked(masks[lvl],i + di + dy, j + dj + dx)) ok = 0;
                                }
                            }
                            if (ok) break;
                        }
                        if(c == 20) {
                            dh/=2;
                            dw/=2;
                            continue;
                        }
                        int randomQuality = estimateQuality(masks[lvl], images[lvl], i, j, i + di, j + dj, 5, 5); // оцениваем насколько похоже будет если мы приложим эту случайную гипотезу которую только что выбрали
                        if (randomQuality < currentQuality) {
                            shifts[lvl].at<Vec2i>(i,j) = {di,dj};
                            images[lvl].at<Vec3b>(i,j) = pyr[lvl].at<Vec3b>(i + di, j + dj);
                        }
                        dh /= 2;
                        dw /= 2;


                    }
                }
            }
            if(start1 == 0){
                start1 = images[lvl].rows - 1; end1 = -1; step1 = -1;
                start2 = images[lvl].cols - 1; end2 = -1; step2 = -1;
            }else{
                start1 = 0; end1 = images[lvl].rows; step1 = 1;
                start2 = 0; end2 = images[lvl].cols; step2 = 1;
            }
        }
    }



    imwrite(resultsDir + "3_res.png", images[n-1]);
}


int main() {
    try {
        run(1, "mic");
        // TODO протестируйте остальные случаи:
        //run(2, "flowers");
        //run(3, "baloons");
        //run(4, "brickwall");
        //run(5, "old_photo");
//        run(6, "your_data"); // TODO придумайте свой случай для тестирования (рекомендуется не очень большое разрешение, например 300х300)

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}