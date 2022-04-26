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
                res += abs(a[k] - b[k]);
            }
        }
    }return res;
}




void tryShift(Mat mask, Mat &original, Mat &image, Mat &shifts, int lvl, int i, int j, int i1, int j1){
    int x = i1 + shifts.at<Vec2i>(i1,j1)[0];
    int y = j1 + shifts.at<Vec2i>(i1,j1)[1];
    int cur = estimateQuality(mask, image, i, j, i + shifts.at<Vec2i> (i,j)[0], j + shifts.at<Vec2i>(i,j)[1],5,5);
    Vec3b curCol = image.at<Vec3b>(i,j);
    image.at<Vec3b>(i,j) = original.at<Vec3b>(x,y);
    int neww = estimateQuality(mask ,image, i, j, x, y,5,5);
    if(neww < cur){
        shifts.at<Vec2i>(i,j) = {x - i, y - j};
    }else{
        image.at<Vec3b>(i,j) = curCol;
    }
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

    cv::Mat shifts(mask.rows, mask.cols, CV_32SC2, Scalar(0,0)); // матрица хранящая смещения, изначально заполнена парами нулей
    cv::Mat image = original; // текущая картинка
    int q = 100;
    int start1 = 0, end1 = image.rows, step1 = 1;
    int start2 = 0, end2 = image.cols, step2 = 1;
    int lvl = 0;
    while (q--) {
        for (int i = 0; i != end1; i += step1) {
            for(int j = start2; j != end2; j+= step2){
                if (!isPixelMasked(mask,i,j)) continue; // пропускаем т.к. его менять не надо
//                if (i > 0 && isPixelMasked(mask, i-1, j)) {
//                    cv::Vec2i dxy = shifts.at<Vec2i> (i,j);//смотрим какое сейчас смещение для этого пикселя в матрице смещения
//                    int ni = i + dxy[0], nj = j + dxy[1];
//                    int currentQuality = estimateQuality(mask, image, i, j, ni, nj, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
//                    if(ni == i && nj == j && isPixelMasked(image,i,j)) currentQuality = 1e9;
//                    int di = - 1 + shifts.at<Vec2i>(i-1,j)[0];// на квадрат 5х5 приложенный центром к (nx, ny)
//                    int dj = shifts.at<Vec2i>(i-1,j)[1];
//                    Vec3b cur = image.at<Vec3b>(i,j);
//                    image.at<Vec3b>(i,j) = original.at<Vec3b>(i + di, j + dj);
//                    int randomQuality = estimateQuality(mask, image, i, j, i + di, j + dj, 5, 5); // оцениваем насколько похоже будет если мы приложим эту случайную гипотезу которую только что выбрали
//                    if (randomQuality < currentQuality) {
//                        shifts.at<Vec2i>(i,j) = {di,dj};
//                    }else image.at<Vec3b>(i,j) = cur;
//                }
//                if(j > 0 && isPixelMasked(mask,i,j-1)) {
//                    cv::Vec2i dxy = shifts.at<Vec2i> (i,j);//смотрим какое сейчас смещение для этого пикселя в матрице смещения
//                    int ni = i + dxy[0], nj = j + dxy[1];
//                    int currentQuality = estimateQuality(mask, image, i, j, ni, nj, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
//                    if(ni == i && nj == j && isPixelMasked(image,i,j)) currentQuality = 1e9;
//                    int di = shifts.at<Vec2i>(i,j-1)[0];// на квадрат 5х5 приложенный центром к (nx, ny)
//                    int dj = shifts.at<Vec2i>(i,j-1)[1] - 1;
//                    Vec3b cur = image.at<Vec3b>(i,j);
//                    image.at<Vec3b>(i,j) = original.at<Vec3b>(i + di, j + dj);
//                    int randomQuality = estimateQuality(mask, image, i, j, i + di, j + dj, 5, 5); // оцениваем насколько похоже будет если мы приложим эту случайную гипотезу которую только что выбрали
//                    if (randomQuality < currentQuality) {
//                        shifts.at<Vec2i>(i,j) = {di,dj};
//                    }else image.at<Vec3b>(i,j) = cur;
//                }
                if(i < image.rows - 1 && isPixelMasked(mask, i + 1, j)) {
                    cv::Vec2i dxy = shifts.at<Vec2i> (i,j);//смотрим какое сейчас смещение для этого пикселя в матрице смещения
                    int ni = i + dxy[0], nj = j + dxy[1];
                    int currentQuality = estimateQuality(mask, image, i, j, ni, nj, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
                    if(ni == i && nj == j && isPixelMasked(image,i,j)) currentQuality = 1e9;
                    int di = 1 + shifts.at<Vec2i>(i+1,j)[0];// на квадрат 5х5 приложенный центром к (nx, ny)
                    int dj = shifts.at<Vec2i>(i+1,j)[1];
                    Vec3b cur = image.at<Vec3b>(i,j);
                    image.at<Vec3b>(i,j) = original.at<Vec3b>(i + di, j + dj);
                    int randomQuality = estimateQuality(mask, image, i, j, i + di, j + dj, 5, 5); // оцениваем насколько похоже будет если мы приложим эту случайную гипотезу которую только что выбрали
                    if (randomQuality < currentQuality) {
                        shifts.at<Vec2i>(i,j) = {di,dj};
                    }else image.at<Vec3b>(i,j) = cur;
                }
                if(j < image.cols - 1 && isPixelMasked(mask, i, j + 1)){
                    cv::Vec2i dxy = shifts.at<Vec2i> (i,j);//смотрим какое сейчас смещение для этого пикселя в матрице смещения
                    int ni = i + dxy[0], nj = j + dxy[1];
                    int currentQuality = estimateQuality(mask, image, i, j, ni, nj, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
                    if(ni == i && nj == j && isPixelMasked(image,i,j)) currentQuality = 1e9;
                    int di = shifts.at<Vec2i>(i,j+1)[0];// на квадрат 5х5 приложенный центром к (nx, ny)
                    int dj = 1 + shifts.at<Vec2i>(i,j+1)[1];
                    Vec3b cur = image.at<Vec3b>(i,j);
                    image.at<Vec3b>(i,j) = original.at<Vec3b>(i + di, j + dj);
                    int randomQuality = estimateQuality(mask, image, i, j, i + di, j + dj, 5, 5); // оцениваем насколько похоже будет если мы приложим эту случайную гипотезу которую только что выбрали
                    if (randomQuality < currentQuality) {
                        shifts.at<Vec2i>(i,j) = {di,dj};
                    }else image.at<Vec3b>(i,j) = cur;
                }
                cv::Vec2i dxy = shifts.at<Vec2i> (i,j);//смотрим какое сейчас смещение для этого пикселя в матрице смещения
                int ni = i + dxy[0], nj = j + dxy[1];
                int currentQuality = estimateQuality(mask, image, i, j, ni, nj, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
                if(ni == i && nj == j && isPixelMasked(image,i,j)) currentQuality = 1e9;
                int di = random.next(-image.rows, image.rows);// на квадрат 5х5 приложенный центром к (nx, ny)
                int dj = random.next(-image.cols, image.cols);
                while(1){
                    bool ok = 1;
                    if(i + di < 0 || i + di >= image.rows) ok = 0;
                    if(j + dj < 0 || j + dj >= image.cols) ok = 0;
                    if(ok && isPixelMasked(image, i+di, j +dj)) ok = 0;
                    if(ok) break;
                    else {
                        di = random.next(-image.rows, image.rows);
                        dj = random.next(-image.cols, image.cols);
                    }
                }
                Vec3b cur = image.at<Vec3b>(i,j);
                image.at<Vec3b>(i,j) = original.at<Vec3b>(i + di, j + dj);
                int randomQuality = estimateQuality(mask, image, i, j, i + di, j + dj, 5, 5); // оцениваем насколько похоже будет если мы приложим эту случайную гипотезу которую только что выбрали
                if (randomQuality < currentQuality) {
                    shifts.at<Vec2i>(i,j) = {di,dj};
                }else image.at<Vec3b>(i,j) = cur;
            }
        }
        if(start1 == 0){
                start1 = image.rows - 1; end1 = -1; step1 = -1;
                start2 = image.cols - 1; end2 = -1; step2 = -1;
        }else{
            start1 = 0; end1 = image.rows; step1 = 1;
            start2 = 0; end2 = image.cols; step2 = 1;
        }
    }
    imwrite(resultsDir + "3_res.png", image);
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