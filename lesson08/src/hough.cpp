#include "hough.h"

#include <libutils/rasserts.h>

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

cv::Mat buildHough(cv::Mat sobel) {// единственный аргумент - это результат свертки Собелем изначальной картинки
    // проверяем что входная картинка - одноканальная и вещественная:
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
            // TODO
            // ...
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
