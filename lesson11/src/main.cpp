#include <filesystem>
#include <iostream>
#include <libutils/rasserts.h>

#include "parseSymbols.h"
#include "hog.h"

#include <opencv2/imgproc.hpp>


#define NSAMPLES_PER_LETTER 5
#define LETTER_DIR_PATH std::string("lesson11/resultsData/letters")


cv::Scalar randColor() {
    return cv::Scalar(128 + rand() % 128, 128 + rand() % 128, 128 + rand() % 128); // можно было бы брать по модулю 255, но так цвета будут светлее и контрастнее
}


cv::Mat drawContours(int rows, int cols, std::vector<std::vector<cv::Point>> contoursPoints) {

    // TODO 06 реализуйте функцию которая покажет вам как выглядят найденные контура:

    // создаем пустую черную картинку
    cv::Mat blackImage(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));
    // теперь мы на ней хотим нарисовать контуры
    cv::Mat imageWithContoursPoints = blackImage.clone();
    for (int contourI = 0; contourI < contoursPoints.size(); ++contourI) {
        // сейчас мы смотрим на контур номер contourI

        cv::Scalar contourColor = randColor(); // выберем для него случайный цвет
        std::vector<cv::Point> points = contoursPoints[contourI]; // TODO 06 вытащите вектор из точек-пикселей соответствующих текущему контуру который мы хотим нарисовать
        for (auto point: points) { // TODO 06 пробегите по всем точкам-пикселям этого контура
             // TODO 06 и взяв очередную точку-пиксель - нарисуйте выбранный цвет в этом пикселе картинки:
            imageWithContoursPoints.at<cv::Vec3b>(point.y, point.x) = cv::Vec3b(contourColor[0], contourColor[1], contourColor[2]);
        }

    }

    return imageWithContoursPoints;
}


void test(std::string name, std::string k) {
    std::cout << "Processing " << name << "/" << k << "..." << std::endl;

    std::string full_path = "lesson11/data/" + name + "/" + k + ".png";

    // создаем папочки в которые будем сохранять картинки с промежуточными результатами
    std::filesystem::create_directory("lesson11/resultsData/" + name);
    std::string out_path = "lesson11/resultsData/" + name + "/" + k;
    std::filesystem::create_directory(out_path);

    // считываем оригинальную исходную картинку
    cv::Mat original = cv::imread(full_path);
    rassert(!original.empty(), 238982391080010);
    rassert(original.type() == CV_8UC3, 23823947238900020);

    // сохраняем ее сразу для удобства
    cv::imwrite(out_path + "/00_original.jpg", original);

    // преобразуем в черно-белый цвет и тоже сразу сохраняем
    cv::Mat img;
    cv::cvtColor(original, img, cv::COLOR_BGR2GRAY);
    cv::imwrite(out_path + "/01_grey.jpg", img);

    // TODO 01 выполните бинарный трешолдинг картинки, прочитайте документацию по функции cv::threshold и выберите значения аргументов
    cv::Mat binary;
    cv::threshold(img, binary, 128, 255, cv::THRESH_BINARY);
    cv::imwrite(out_path + "/02_binary_thresholding.jpg", binary);

    // TODO 02 выполните адаптивный бинарный трешолдинг картинки, прочитайте документацию по cv::adaptiveThreshold
    cv::adaptiveThreshold(img, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 15, 10);
    cv::imwrite(out_path + "/03_adaptive_thresholding.jpg", binary);

    // TODO 03 чтобы буквы не разваливались на кусочки - морфологическое расширение (эрозия)
    cv::Mat binary_eroded;
    cv::erode(binary, binary_eroded, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    cv::imwrite(out_path + "/04_erode.jpg", binary_eroded);

    // TODO 03 заодно давайте посмотрим что делает морфологическое сужение (диляция)
    cv::Mat binary_dilated;
    cv::dilate(binary, binary_dilated, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
    cv::imwrite(out_path + "/05_dilate.jpg", binary_dilated);

    // TODO 04 дальше работаем с картинкой после морфологичесокго рашсирения или морфологического сжатия - на ваш выбор, подумайте и посмотрите на картинки
    binary = binary_eroded;

    // TODO 05
    std::vector<std::vector<cv::Point>> contoursPoints; // по сути это вектор, где каждый элемент - это одна связная компонента-контур,
                                                        // а что такое компонента-контур? это вектор из точек (из пикселей)
    cv::findContours(binary, contoursPoints,cv::RETR_LIST, cv::CHAIN_APPROX_NONE); // TODO подумайте, какие нужны два последних параметра? прочитайте документацию, после реализации отрисовки контура - поиграйте с этими параметрами чтобы посмотреть как меняется результат
    std::cout << "Contours: " << contoursPoints.size() << std::endl;
    cv::Mat imageWithContoursPoints = drawContours(img.rows, img.cols, contoursPoints); // TODO 06 реализуйте функцию которая покажет вам как выглядят найденные контура
    cv::imwrite(out_path + "/06_contours_points.jpg", imageWithContoursPoints);


//    // TODO:
//    // Обратите внимание на кромку картинки - она всё победила, т.к. черное - это ноль - пустота, а белое - это 255 - сам объект интереса
//    // как перевернуть ситуацию чтобы периметр не был засчитан как контур?
//    // когда подумаете - замрите! и прежде чем кодить:
//    // Посмотрите в документации у функций cv::threshold и cv::adaptiveThreshold
//    // про некоего cv::THRESH_BINARY_INV, чем он отличается от cv::THRESH_BINARY?
//    // Посмотрите как изменились все картинки.
    cv::Mat binary2;
    cv::adaptiveThreshold(img, binary2, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 10);
    cv::imwrite(out_path + "/07_adaptive_thresholdingINV.jpg", binary2);
    // TODO 03 чтобы буквы не разваливались на кусочки - морфологическое расширение (эрозия)
    cv::Mat binary2_eroded;
    cv::erode(binary2, binary2_eroded, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2)));
    cv::imwrite(out_path + "/08_erode2.jpg", binary2_eroded);
    std::vector<std::vector<cv::Point>> contoursPoints2; // по сути это вектор, где каждый элемент - это одна связная компонента-контур,
    // а что такое компонента-контур? это вектор из точек (из пикселей)
    cv::findContours(binary2_eroded, contoursPoints2,cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    std::cout << "Contours2: " << contoursPoints2.size() << std::endl;
    cv::Mat imageWithContoursPoints2 = drawContours(img.rows, img.cols, contoursPoints2);
    cv::imwrite(out_path + "/09_contours_points2.jpg", imageWithContoursPoints2);

    // TODO 06 наконец давайте посмотрим какие буковки нашлись - обрамим их прямоугольниками
    contoursPoints = contoursPoints2;
    cv::Mat imgWithBoxes = original.clone();
    for (int contourI = 0; contourI < contoursPoints.size(); ++contourI) {
        std::vector<cv::Point> points = contoursPoints[contourI]; // перем очередной контур
        cv::Rect box = cv::boundingRect(points); // строим прямоугольник по всем пикселям контура (bounding box = бокс ограничивающий объект)
        cv::Scalar blackColor(0, 0, 0);
        // TODO прочитайте документацию cv::rectangle чтобы нарисовать прямоугольник box с толщиной 2 и черным цветом (обратите внимание какие есть поля у box)
        cv::rectangle(imgWithBoxes, {box.x, box.y}, {box.x + box.width, box.y + box.height}, blackColor, 2);
    }
    cv::imwrite(out_path + "/10_boxes.jpg", imgWithBoxes); // TODO если вдруг у вас в картинке странный результат
                                                           // например если нет прямоугольников - посмотрите в верхний левый пиксель - белый ли он?
                                                           // если не белый, то что это значит? почему так? сколько в целом нашлось связных компонент?
}


double randFontScale() {
    double min_scale = 2.5;
    double max_scale = 3.0;
    double scale = min_scale + (max_scale - min_scale) * ((rand() % 100) / 100.0);
    return scale;
}

int randThickness() {
    int min_thickness = 2;
    int max_thickness = 3;
    int thickness = min_thickness + rand() % (max_thickness - min_thickness + 1);
    return thickness;
}

int randFont() {
    int fonts[] = {
            cv::FONT_HERSHEY_SIMPLEX,
            cv::FONT_HERSHEY_PLAIN,
            cv::FONT_HERSHEY_DUPLEX,
            cv::FONT_HERSHEY_COMPLEX,
            cv::FONT_HERSHEY_TRIPLEX,
            cv::FONT_HERSHEY_COMPLEX_SMALL,
            cv::FONT_HERSHEY_SCRIPT_SIMPLEX,
            cv::FONT_HERSHEY_SCRIPT_COMPLEX,
    };
    int nfonts = (sizeof(fonts) / sizeof(int));
    int font = rand() % nfonts;
    bool is_italic = ((rand() % 5) == 0);
    if  (is_italic) {
        font = font | cv::FONT_ITALIC;
    }

    return font;
}

cv::Mat generateImage(std::string text, int width=128, int height=128) {
    cv::Scalar white(255, 255, 255);
    cv::Scalar backgroundColor = white;
    // Создаем картинку на которую мы нанесем символ (пока что это просто белый фон)
    cv::Mat img(height, width, CV_8UC3, backgroundColor);

    // Выберем случайные параметры отрисовки текста - шрифт, размер, толщину, цвет
    int font = randFont();
    double fontScale = randFontScale();
    int thickness = randThickness();
    cv::Scalar color = randColor();

    // Узнаем размер текста в пикселях (если его нарисовать с указанными параметрами)
    int baseline = 0;
    cv::Size textPixelSize = cv::getTextSize(text, font, fontScale, thickness, &baseline);

    // Рисуем этот текст идеально в середине картинки
    // (для этого и нужно было узнать размер текста в пикселях - чтобы сделать отступ от середины картинки)
    // (ведь при рисовании мы указываем координаты левого нижнего угла текста)
    int xLeft = (width / 2) - (textPixelSize.width / 2);
    int yBottom = (height / 2) + (textPixelSize.height / 2);
    cv::Point coordsOfLeftBorromCorner(xLeft, yBottom);
    cv::putText(img, text, coordsOfLeftBorromCorner, font, fontScale, color, thickness);

    return img;
}

void generateAllLetters() {
    srand(239017); // фиксируем зерно генератора случайных чисел (чтобы картинки от раза к разу генерировались с одинаковыми шрифтами, размерами и т.п.)

    for (char letter = 'a'; letter <= 'z'; ++letter) {

        // Создаем папку для текущей буквы:
        std::string letterDir = LETTER_DIR_PATH + "/" + letter;
        std::filesystem::create_directory(letterDir);

        for (int sample = 1; sample <= NSAMPLES_PER_LETTER; ++sample) {
            std::string text = std::string("") + letter;
            cv::Mat img = generateImage(text);

            cv::blur(img, img, cv::Size(3, 3));

            std::string letterSamplePath = letterDir + "/" + std::to_string(sample) + ".png";
            cv::imwrite(letterSamplePath, img);
        }
    }
}

std::pair<std::string, std::string> detect(const cv::Mat& a) {
    std::string letterMin = "", lm = "";
    double distMin = DBL_MAX;
    HoG hogA = buildHoG(a);
    for (char letterB = 'a'; letterB <= 'z'; ++letterB) {
        for(int i = 1; i <= NSAMPLES_PER_LETTER; i++){
            cv::Mat b = cv::imread(LETTER_DIR_PATH + "/" + letterB + "/" + std::to_string(i) + ".png");
            HoG hogB = buildHoG(b);
            double d = distance(hogA, hogB);
            if (d <= distMin) {
                distMin = d;
                letterMin = LETTER_DIR_PATH + "/" + letterB + "/" + std::to_string(i) + ".png";
                lm = letterB;
            }
        }
    }
    return {letterMin, lm};
}

void finalExperiment(std::string name, std::string k) {
    cv::Mat original = cv::imread("lesson11/data/" + name + "/" + k + ".png");
    std::vector<cv::Mat> vect = splitSymbols(original);
    int i = 0;
    std::vector<std::string> ans;
    for(const cv::Mat& m : vect) {
        std::pair<std::string, std::string> p = detect(m);
        std::cout << i << " " << p.first << "\n";
        cv::imwrite(LETTER_DIR_PATH + "/res/" + std::to_string(i++) + ".png", m);
        ans.push_back(p.second);
        //std::cout<<LETTER_DIR_PATH + "/res/" + std::to_string(i);
    }for(auto i: ans) std::cout<<i<<" ";
    std::cout<<"\n";

}



int main() {
    try {
        generateAllLetters();
        finalExperiment("alphabet", "3_gradient");
        //test("alphabet", "3_gradient");
        //finalExperiment("text", "2");

        // TODO 50: обязательно получите результат на других картинках - прямо в цикле все их обработайте:
//        std::vector<std::string> names;
//        names.push_back("alphabet");
//        names.push_back("line");
//        names.push_back("text");
//        for (int i = 0; i < names.size(); ++i) {
//            for (int j = 1; j <= 5; ++j) {
//                test(names[i], std::to_string(j));
//            }
//        }

        //test("alphabet", "3_gradient");

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}

