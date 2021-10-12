#include <iostream>
#include <filesystem> // это нам понадобится чтобы создать папку для результатов
#include <libutils/rasserts.h>
#include <cstdlib>
#include <ctime>
#include<algorithm>

#include "helper_functions.h"
#include "disjoint.h"

#include <opencv2/highgui.hpp> // подключили часть библиотеки OpenCV, теперь мы можем читать и сохранять картинки

using namespace std;
using namespace cv;

void task1() {
    srand((unsigned)time(0));
    cv::Mat imgUnicorn = cv::imread("lesson03/data/unicorn.png");  // загружаем картинку с единорогом
    rassert(!imgUnicorn.empty(), 3428374817241); // проверяем что картинка загрузилась (что она не пустая)

    // TODO выведите в консоль разрешение картинки (ширина x высота)
    int width = imgUnicorn.cols; // как в ООП - у картинки есть поля доступные через точку, они называются cols и rows - попробуйте их
    int height = imgUnicorn.rows;
    std::cout << "Unicorn image loaded: " << width << "x" << height << std::endl;

    // создаем папку в которую будем сохранять результаты - lesson03/resultsData/
    std::string resultsDir = "lesson03/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }
    cv::Mat blueUnicorn = makeAllBlackPixelsBlue(imgUnicorn.clone()); // TODO реализуйте функцию которая каждый пиксель картинки который близок к белому - делает синим
    std::string filename = resultsDir + "01_blue_unicorn.jpg"; // удобно в начале файла писать число, чтобы файлы были в том порядке в котором мы их создали
    cv::imwrite(filename, blueUnicorn);

    cv::Mat invertedUnicorn = invertImageColors(imgUnicorn.clone()); // TODO реализуйте функцию которая каждый цвет картинки инвертирует
    // TODO сохраните резльутат в ту же папку, но файл назовите "02_inv_unicorn.jpg"
    string filename2 = resultsDir + "02_inv_unicorn.jpg";
    imwrite(filename2, invertedUnicorn);

    cv::Mat castle = imread("lesson03/data/castle.png"); // TODO считайте с диска картинку с замком - castle.png
    cv::Mat unicornInCastle = addBackgroundInsteadOfBlackPixels(imgUnicorn.clone(), castle); // TODO реализуйте функцию которая все черные пиксели картинки-объекта заменяет на пиксели с картинки-фона
    // TODO сохраните результат в ту же папку, назовите "03_unicorn_castle.jpg"
    string filename3 = resultsDir + "03_unicorn_castle.jpg";
    imwrite(filename3, unicornInCastle);

    cv::Mat largeCastle = imread("lesson03/data/castle_large.jpg");// TODO считайте с диска картинку с большим замком - castle_large.png
    rassert(!largeCastle.empty(), 3428374817241);
    cv::Mat unicornInLargeCastle = addBackgroundInsteadOfBlackPixelsLargeBackground(imgUnicorn.clone(), largeCastle.clone()); // TODO реализуйте функцию так, чтобы нарисовался объект ровно по центру на данном фоне, при этом черные пиксели объекта не должны быть нарисованы
    // TODO сохраните результат - "04_unicorn_large_castle.jpg"
    string filename4 = resultsDir + "04_unicorn_large_castle.jpg";
     imwrite(filename4, unicornInLargeCastle);

    // TODO сделайте то же самое, но теперь пусть единорог рисуется N раз (случайно выбранная переменная от 0 до 100)
    // функцию вам придется объявить самостоятельно, включая:
    // 1) придумывание ее имени
    // 2) добавление декларации в helper_functions.h (три аргумента - объект, фон, число рисований объекта)
    // 3) добавление реализации в helper_functions.cpp (список аргументов должен совпадать со списком в декларации)
    // 4) как генерировать случайные числа - найдите самостоятельно через гугл, например "c++ how to random int"
    // 5) при этом каждый единорог рисуется по случайным координатам
    // 6) результат сохраните - "05_unicorns_otake.jpg"

    Mat nUnicorns = drawNUnicorns(imgUnicorn.clone(), largeCastle.clone(), rand()%100);
    imwrite(resultsDir + "05_N_Unicorns.jpg", nUnicorns);

    // TODO растяните картинку единорога так, чтобы она заполнила полностью большую картинку с замком "06_unicorn_upscale.jpg"
    Mat unicornUpscale = largeCastle;
    for(int i = 0; i < largeCastle.rows; ++i){
        for(int j = 0;j < largeCastle.cols;++j){
            unicornUpscale.at<Vec3b>(i,j) = imgUnicorn.at<Vec3b>(min(imgUnicorn.rows - 1, (int)floor(1.0*i*imgUnicorn.rows/largeCastle.rows)), min(imgUnicorn.cols-1, (int)floor(1.0*j*imgUnicorn.cols/largeCastle.cols)));
        }
    }
    imwrite(resultsDir+"06_unicornUpscale.jpg", unicornUpscale);
}

void task2() {
    cv::Mat imgUnicorn = cv::imread("lesson03/data/unicorn.png");
    rassert(!imgUnicorn.empty(), 3428374817241);

    // cv::waitKey - функция некоторое время ждет не будет ли нажата кнопка клавиатуры, если да - то возвращает ее код
    int updateDelay = 10; // указываем сколько времени ждать нажатия кнопки клавиатуры - в миллисекундах
    while (cv::waitKey(updateDelay) != 32) {
        // поэтому если мы выполняемся до тех пор пока эта функция не вернет код 32 (а это код кнопки "пробел"), то достаточно будет нажать на пробел чтобы закончить работу программы

        // кроме сохранения картинок на диск (что часто гораздо удобнее конечно, т.к. между ними легко переключаться)
        // иногда удобно рисовать картинку в окне:
        cv::imshow("lesson03 window", imgUnicorn);
        // TODO сделайте функцию которая будет все черные пиксели (фон) заменять на случайный цвет (аккуратно, будет хаотично и ярко мигать, не делайте если вам это противопоказано)
        imgUnicorn = makeBlackPixelsRand(imgUnicorn.clone());
    }
}

struct MyVideoContent {
    cv::Mat frame;
    int lastClickX;
    int lastClickY;
    int inv = 0;
    vector<pair<int,int>> pixels;
    vector<vector<int>> mask, maskAfterErode, maskAfterDilate;
};

void onMouseClick(int event, int x, int y, int flags, void *pointerToMyVideoContent) {
    MyVideoContent &content = *((MyVideoContent*) pointerToMyVideoContent);
    // не обращайте внимание на предыдущую строку, главное что важно заметить:
    // content.frame - доступ к тому кадру что был только что отображен на экране
    // content.lastClickX - переменная которая вам тоже наверняка пригодится
    // вы можете добавить своих переменных в структурку выше (считайте что это описание объекта из ООП, т.к. почти полноценный класс)

    if (event == cv::EVENT_LBUTTONDOWN) { // если нажата левая кнопка мыши
        std::cout << "Left click at x=" << x << ", y=" << y << std::endl;
        content.pixels.push_back({x, y});
    }
    //if(event == EVENT_LBUTTONDOWN) content.pixels.push_back({content.lastClickX, content.lastClickY});
    if(event == EVENT_RBUTTONDOWN) {
        content.inv++;
    }
}

void task3() {
    // давайте теперь вместо картинок подключим видеопоток с веб камеры:
    cv::VideoCapture video(0);
    // если у вас нет вебкамеры - подключите ваш телефон к компьютеру как вебкамеру - это должно быть не сложно (загуглите)
    // альтернативно если у вас совсем нет вебки - то попробуйте запустить с видеофайла, но у меня не заработало - из-за "there is API version mismath: plugin API level (0) != OpenCV API level (1)"
    // скачайте какое-нибудь видео с https://www.videezy.com/free-video/chroma-key
    // например https://www.videezy.com/elements-and-effects/5594-interactive-hand-gesture-sliding-finger-studio-green-screen
    // если вы увидите кучу ошибок в консоли навроде "DynamicLib::libraryLoad load opencv_videoio_ffmpeg451_64.dll => FAILED", то скопируйте файл C:\...\opencv\build\x64\vc14\bin\opencv_videoio_ffmpeg451_64.dll в папку с проектом
    // и укажите путь к этому видео тут:
//    cv::VideoCapture video("lesson03/data/Spin_1.mp4");

    rassert(video.isOpened(), 3423948392481); // проверяем что видео получилось открыть

    MyVideoContent content; // здесь мы будем хранить всякие полезности - например последний видео кадр, координаты последнего клика и т.п.
    // content.frame - доступ к тому кадру что был только что отображен на экране
    // content.lastClickX - переменная которая вам тоже наверняка пригодится
    // вы можете добавить своих переменных в структурку выше (считайте что это описание объекта из ООП, т.к. почти полноценный класс)
    // просто перейдите к ее объявлению - удерживая Ctrl сделайте клик левой кнопкой мыши по MyVideoContent - и вас телепортирует к ее определению

    while (video.isOpened()) { // пока видео не закрылось - бежим по нему
        bool isSuccess = video.read(content.frame); // считываем из видео очередной кадр
        rassert(isSuccess, 348792347819); // проверяем что считывание прошло успешно
        rassert(!content.frame.empty(), 3452314124643); // проверяем что кадр не пустой
        if(content.inv%2 == 1){
            content.frame = invertImageColors(content.frame);
        }


        cv::imshow("video", content.frame); // покаызваем очередной кадр в окошке
        cv::setMouseCallback("video", onMouseClick, &content); // делаем так чтобы функция выше (onMouseClick) получала оповещение при каждом клике мышкой

        int key = cv::waitKey(10);
        // TODO добавьте завершение программы в случае если нажат пробел
        if(key == 32 || key == 27){
            return;
        }
    }
}

void task4() {
    cv::VideoCapture video(0);

    rassert(video.isOpened(), 3423948392481); // проверяем что видео получилось открыть

    MyVideoContent content; // здесь мы будем хранить всякие полезности - например последний видео кадр, координаты последнего клика и т.п.
    Mat imgLargeCastle = imread("lesson03/data/castle_large.jpg");
    int width = 1024;
    int height = 768;
    Scalar color(0, 0, 0);
    Mat castleUpscale(height, width, CV_8UC3, color);
    for(int i = 0; i < castleUpscale.rows; ++i){
        for(int j = 0;j < castleUpscale.cols;++j){
            castleUpscale.at<Vec3b>(i,j) = imgLargeCastle.at<Vec3b>(min(imgLargeCastle.rows - 1, (int)floor(1.0*i*imgLargeCastle.rows/castleUpscale.rows)),
                                                                    min(imgLargeCastle.cols-1, (int)floor(1.0*j*imgLargeCastle.cols/castleUpscale.cols)));
        }
    }

    //imshow("lesson03_window", castleUpscale);
    bool isFirst = 1;
    Mat firstImage;

    while (video.isOpened()) { // пока видео не закрылось - бежим по нему
        bool isSuccess = video.read(content.frame); // считываем из видео очередной кадр
        rassert(isSuccess, 348792347819); // проверяем что считывание прошло успешно
        rassert(!content.frame.empty(), 3452314124643); // проверяем что кадр не пустой
        std::string resultsDir = "lesson03/test1/";
        if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
            std::filesystem::create_directory(resultsDir); // то создаем ее
        }
        if(isFirst){
            firstImage = content.frame.clone();
            isFirst = 0;
            cout<<"takeFirstImage\n";
            imwrite(resultsDir+"firstImage.jpg", firstImage);
        }else{
            Mat img = content.frame;
            int key = cv::waitKey(1);

            vector<vector<int>> mask(img.rows, vector<int>(img.cols));
            for(int y = 0;y<img.rows;++y){
                for(int x = 0;x<img.cols;++x){
                    if(equeal(img.at<Vec3b>(y,x), firstImage.at<Vec3b>(y,x))){
                        mask[y][x] = 1;
                    }
                }
            }if(key == 32) {
                imwrite(resultsDir+"maskVanilla.jpg", maskToPicture(mask));
                cout<<"isodnfjs";
            }
            erode(mask,1);
            if(key == 32) imwrite(resultsDir + "maskAfterErode.jpg", maskToPicture(mask));
            dilate(mask,1);
            if(key == 32) imwrite(resultsDir + "maskAfterDilate.jpg", maskToPicture(mask));
            height = img.rows;
            width = img.cols;
            DisjointSet set(height*width);
            for(int i = 0; i<height; ++i){
                for(int j = 0;j < width;++j){
                    if(mask[i][j] == 0){
                        if(i>0 && j>0 && mask[i-1][j-1] == 0) set.union_sets(i*width + j, (i-1)*width+j-1);
                        if(i>0 && j<width-1 && mask[i-1][j+1] == 0) set.union_sets(i*width + j, (i-1)*width+j+1);
                        if(i<height-1 && j>0 && mask[i+1][j-1] == 0) set.union_sets(i*width + j, (i+1)*width+j-1);
                        if(i<height-1 && j<width-1 && mask[i+1][j+1] == 0) set.union_sets(i*width + j, (i+1)*width+j+1);
                    }
                }
            }
            for(int i = 0;i<height;++i){
                for(int j = 0;j<width;++j){
                    if(set.get_set_size(width*i+j)<200){
                        mask[i][j] = 1;
                    }
                }
            }
            if(key == 32) imwrite(resultsDir + "maskAfterDisjoint.jpg", maskToPicture(mask));

            for(int i=0;i<img.rows;++i){
                for(int j=0;j<img.cols;++j){
                    if(mask[i][j] == 1){
                        img.at<Vec3b>(i,j) = castleUpscale.at<Vec3b>(i,j);
                    }
                }
            }
        }
        if(content.inv%2 == 1){
            content.frame = invertImageColors(content.frame);
        }


        cv::imshow("video", content.frame); // покаызваем очередной кадр в окошке
        cv::setMouseCallback("video", onMouseClick, &content); // делаем так чтобы функция выше (onMouseClick) получала оповещение при каждом клике мышкой

        int key = cv::waitKey(10);
        if(key == 27){
            return;
        }
    }
}

int main() {
    try {
        //task1();
        //task2();
        //task3();
       task4();
        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
