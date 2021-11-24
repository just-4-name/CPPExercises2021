#include "hough.h"

#include <libutils/rasserts.h>

#include <opencv2/imgproc.hpp>
using namespace std;
using namespace cv;

double toRadians(double degrees)
{
    const double PI = 3.14159265358979323846264338327950288;
    return degrees * PI / 180.0;
}

double estimateR(double x0, double y0, double theta0radians)
{
    double r0 = x0 * cos(theta0radians) + y0 * sin(theta0radians);
    return r0;
}


cv::Mat buildHough(cv::Mat sobel) {
    rassert(sobel.type() == CV_32FC1, 237128273918006);

    // TODO
    // Эта функция по картинке с силами градиентов (после свертки оператором Собеля) строит пространство Хафа
    // Вы можете либо взять свою реализацию из прошлого задания, либо взять эту заготовку:

    int width = sobel.cols;
    int height = sobel.rows;

    // решаем какое максимальное значение у параметра theta в нашем пространстве прямых
    int max_theta = 360;

    // решаем какое максимальное значение у параметра r в нашем пространстве прямых:
    int max_r = hypot(width, height);

    // создаем картинку-аккумулятор, в которой мы будем накапливать суммарные голоса за прямые
    // так же известна как пространство Хафа
    cv::Mat accumulator(max_r, max_theta, CV_32FC1, 0.0f); // зануление аккумулятора через указание значения по умолчанию в конструкторе

    // проходим по всем пикселям нашей картинки (уже свернутой оператором Собеля)
    for (int y0 = 0; y0 < height; ++y0) {
        for (int x0 = 0; x0 < width; ++x0) {
            // смотрим на пиксель с координатами (x0, y0)
            float strength = sobel.at<float>(y0, x0);

            // теперь для текущего пикселя надо найти все возможные прямые которые через него проходят
            // переберем параметр theta по всему возможному диапазону (в градусах):
            for (int theta0 = 0; theta0 + 1 < max_theta; ++theta0) {

                double theta0radians = toRadians(theta0);
                int r0 = (int) round(estimateR(x0, y0, theta0radians)); // оцениваем r0 и округляем его до целого числа
                if (r0 < 0 || r0 >= max_r)
                    continue;

                // TODO надо определить в какие пиксели i,j надо внести наш голос с учетом проблемы "Почему два экстремума?" обозначенной на странице:
                // https://www.polarnick.com/blogs/239/2021/school239_11_2021_2022/2021/11/09/lesson9-hough2-interpolation-extremum-detection.html
                int r1 = (int) round(estimateR(x0,y0, toRadians(theta0 + 1)));
                if(r1 < 0 || r1 >= max_r) continue;
                if(r0>r1) swap(r1,r0);
                for(int i = r1;i>r0;--i){
                    float l = (i-r0)/(r1-r0);
                    rassert(l<=1,123)
                    float r = 1-l;
                    accumulator.at<float> (i,theta0) += l*strength;
                    accumulator.at<float> (i,theta0 + 1) += r*strength;
                }
            }
        }
    }

    return accumulator;
}

std::vector<PolarLineExtremum> findLocalExtremums(cv::Mat houghSpace)
{
    rassert(houghSpace.type() == CV_32FC1, 234827498237080);

    const int max_theta = 360;
    rassert(houghSpace.cols == max_theta, 233892742893082);
    const int max_r = houghSpace.rows;

    std::vector<PolarLineExtremum> winners;

    for (int theta = 0; theta < max_theta; ++theta) {
        for (int r = 0; r < max_r; ++r) {
            bool ok = 1;
            for(int dtheta = -1; dtheta<=1; dtheta++){
                for(int dr = -1; dr<=1; ++dr){
                    if(dr == 0 && dtheta == 0) continue;
                    if(theta + dtheta>=max_theta || theta + dtheta<0) continue;
                    if(r+dr>=max_r || r+dr<0) continue;
                    if(houghSpace.at<float>(r+dr, theta+dtheta)>=houghSpace.at<float>(r,theta)) {
                        ok = 0;
                        break;
                    }
                }
            }
            if (ok) {
                PolarLineExtremum line(theta, r, houghSpace.at<float>(r,theta));
                winners.push_back(line);
            }
        }
    }

    return winners;
}

std::vector<PolarLineExtremum> filterStrongLines(std::vector<PolarLineExtremum> allLines, double thresholdFromWinner)
{
    std::vector<PolarLineExtremum> strongLines;

    // Эта функция по множеству всех найденных локальных экстремумов (прямых) находит самую популярную прямую
    // и возвращает только вектор из тех прямых, что не сильно ее хуже (набрали хотя бы thresholdFromWinner голосов от победителя, т.е. например половину)

    int m = 0;
    for(int i = 0;i<allLines.size();++i){
        m = max(m, (int)(round(allLines[i].votes)));
    }
    for(int i=0;i<allLines.size();++i){
        if(allLines[i].votes>=thresholdFromWinner*m){
            strongLines.push_back(allLines[i]);
        }
    }

    return strongLines;
}

cv::Mat drawCirclesOnExtremumsInHoughSpace(cv::Mat houghSpace, std::vector<PolarLineExtremum> lines, int radius)
{
    // TODO Доделайте эту функцию - пусть она скопирует картинку с пространством Хафа и отметит на ней красным кружком указанного радиуса (radius) места где были обнаружены экстремумы (на базе списка прямых)

    // делаем копию картинки с пространством Хафа (чтобы не портить само пространство Хафа)
    cv::Mat houghSpaceWithCrosses = houghSpace.clone();

    // проверяем что пространство состоит из 32-битных вещественных чисел (т.е. картинка одноканальная)
    rassert(houghSpaceWithCrosses.type() == CV_32FC1, 347823472890137);

    // но мы хотим рисовать КРАСНЫЙ кружочек вокруг найденных экстремумов, а значит нам не подходит черно-белая картинка
    // поэтому ее надо преобразовать в обычную цветную BGR картинку
    cv::cvtColor(houghSpaceWithCrosses, houghSpaceWithCrosses, cv::COLOR_GRAY2BGR);
    // проверяем что теперь все хорошо и картинка трехканальная (но при этом каждый цвет - 32-битное вещественное число)
    rassert(houghSpaceWithCrosses.type() == CV_32FC3, 347823472890148);

    for (int i = 0; i < lines.size(); ++i) {
        PolarLineExtremum line = lines[i];

        // Пример как рисовать кружок в какой-то точке (закомментируйте его):
//        cv::Point point(100, 50);
        cv::Scalar color(0, 0, 255); // BGR, т.е. красный цвет
//        cv::circle(houghSpaceWithCrosses, point, 3, color);

        Point point(line.theta, line.r);
        circle(houghSpaceWithCrosses, point, radius, color);

        // TODO отметьте в пространстве Хафа красным кружком радиуса radius экстремум соответствующий прямой line
    }

    return houghSpaceWithCrosses;
}

const double EPS = 1e-9;

double det (double a, double b, double c, double d) {
    return a * d - b * c;
}

const int INF = 1e9;


cv::Point PolarLineExtremum::intersect(PolarLineExtremum that)
{
    // Одна прямая - наш текущий объект (this) у которого был вызван этот метод, у этой прямой такие параметры:
    double theta0 = this->theta;
    double r0 = this->r;

    // Другая прямая - другой объект (that) который был передан в этот метод как аргумент, у этой прямой такие параметры:
    double theta1 = that.theta;
    double r1 = that.r;

    double a1 = cos(toRadians(theta0)), b1 = sin(toRadians(theta0)), c1 = -r0;
    double a2 = cos(toRadians(theta1)), b2 = sin(toRadians(theta1)), c2 = -r1;

    // TODO реализуйте поиск пересечения этих двух прямых, напоминаю что формула прямой описана тут - https://www.polarnick.com/blogs/239/2021/school239_11_2021_2022/2021/11/02/lesson8-hough-transform.html
    // после этого загуглите как искать пересечение двух прямых, пример запроса: "intersect two 2d lines"
    // и не забудьте что cos/sin принимают радианы (используйте toRadians)
    double zn = det (a1, b1, a2, b2);
    if (abs (zn) < EPS)
        return Point(INF, INF);
    int x = - det (c1, b1, c2, b2) / zn;
    int y = - det (a1, c1, a2, c2) / zn;

    return cv::Point(x, y);
}

// TODO Реализуйте эту функцию - пусть она скопирует картинку и отметит на ней прямые в соответствии со списком прямых
cv::Mat drawLinesOnImage(cv::Mat img, std::vector<PolarLineExtremum> lines)
{
    // делаем копию картинки (чтобы при рисовании не менять саму оригинальную картинку)
    cv::Mat imgWithLines = img.clone();

    // проверяем что картинка черно-белая (мы ведь ее такой сделали ради оператора Собеля) и 8-битная
    rassert(imgWithLines.type() == CV_8UC1, 45728934700167);

    // но мы хотим рисовать КРАСНЫЕ прямые, а значит нам не подходит черно-белая картинка
    // поэтому ее надо преобразовать в обычную цветную BGR картинку с 8 битами в каждом пикселе
    cv::cvtColor(imgWithLines, imgWithLines, cv::COLOR_GRAY2BGR);
    rassert(imgWithLines.type() == CV_8UC3, 45728934700172);

    // выпишем размер картинки
    int width = imgWithLines.cols;
    int height = imgWithLines.rows;

    for (int i = 0; i < lines.size(); ++i) {
        PolarLineExtremum line = lines[i];

        // нам надо найти точки на краях картинки
        cv::Point pointA;
        cv::Point pointB;

        // TODO создайте четыре прямых соответствующих краям картинки (на бумажке нарисуйте картинку и подумайте какие theta/r должны быть у прямых?):
        // напоминаю - чтобы посмотреть какие аргументы требует функция (или в данном случае конструктор объекта) - нужно:
        // 1) раскомментировать эти четыре строки ниже
        // 2) поставить каретку (указатель где вы вводите новые символы) внутри скобок функции (или конструктора, т.е. там где были три вопроса: ???)
        // 3) нажать Ctrl+P чтобы показать список параметров (P=Parameters)
        PolarLineExtremum leftImageBorder(0, 0, 0);
        PolarLineExtremum bottomImageBorder(90, height, 0);
        PolarLineExtremum rightImageBorder(0, width, 0);
        PolarLineExtremum topImageBorder(90,0,0);

        // TODO воспользуйтесь недавно созданной функций поиска пересечения прямых чтобы найти точки пересечения краев картинки:
        if((line.theta>=45 && line.theta<=135) || (line.theta>=225 && line.theta<=315)) {
            pointA = line.intersect(leftImageBorder);
            pointB = line.intersect(rightImageBorder);
        }else{
            pointA = line.intersect(topImageBorder);
            pointB = line.intersect(bottomImageBorder);
        }
        //pointA = line.intersect(leftImageBorder);
        //pointB = line.intersect(rightImageBorder);

        // TODO а в каких случаях нужно использовать пересечение с верхним и нижним краем картинки?
//        pointA = line.intersect(???);
//        pointB = line.intersect(???);

        // TODO переделайте так чтобы цвет для каждой прямой был случайным (чтобы легче было различать близко расположенные прямые)
        srand((unsigned)time(0));
        cv::Scalar color((rand()+255)%255, (rand()+255)%255, (rand()+255)%255);
        cv::line(imgWithLines, pointA, pointB, color);
    }

    return imgWithLines;
}
