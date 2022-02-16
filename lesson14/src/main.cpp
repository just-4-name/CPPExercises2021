#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <chrono>
#include <iostream>
#include <filesystem>
#include <memory>

#include <libutils/rasserts.h>


void drawText(cv::Mat img, std::string text, double fontScale, int &yOffset) {
    cv::Scalar white(255, 255, 255);
    // рассчитываем высоту текста в пикселях:
    float textHeight = cv::getTextSize(text, cv::FONT_HERSHEY_DUPLEX, fontScale, 1, nullptr).height;
    yOffset += textHeight; // увеличиваем сдвиг на высоту текста в пикселях
    cv::putText(img, text, cv::Point(0, yOffset), cv::FONT_HERSHEY_DUPLEX, fontScale, white);
}


void run() {
    const bool useWebcam = 0; // TODO попробуйте выставить в true, если у вас работает вебкамера - то и здорово! иначе - работайте хотя бы со статичными картинками

    bool drawOver = true; // рисовать ли поверх наложенную картинку (можно включить-включить чтобы мигнуть картинкой и проверить качество выравнивания)
    bool drawDebug = true; // рисовать ли поверх отладочную информацию (например красный кант вокруг нарисованной поверх картинки)
    bool useSIFTDescriptor = true; // SIFT работает довольно медленно, попробуйте использовать ORB + не забудьте что тогда вам нужен другой DescriptorMatcher

    cv::Mat imgFrame, imgForDetection, imgToDraw;
    // если у вас не работает через веб. камеру - будут использоваться заготовленные картинки
    imgFrame = cv::imread("lesson14/data/1_box2/box0.png"); // пример кадра с вебкамеры, на нем мы хотим найти объект и вместо него нарисовать другую картинку
    imgForDetection = cv::imread("lesson14/data/1_box2/box1.png"); // пример картинки которую мы хотим найти на видеокадре
    imgToDraw = cv::imread("lesson14/data/1_box2/box1_nesquik.png"); // пример картинки которую мы хотим нарисовать заместо искомой
    rassert(!imgFrame.empty(), 324789374290023);
    rassert(!imgForDetection.empty(), 3789572984290019);
    rassert(!imgToDraw.empty(), 3789572984290021);

    std::shared_ptr<cv::VideoCapture> video;
    if (useWebcam) {
        std::cout << "Trying to open web camera..." << std::endl;
        video = std::make_shared<cv::VideoCapture>(0);
        rassert(video->isOpened(), 3482789346289027);
        std::cout << "Web camera video stream opened." << std::endl;
    }

    while (true) {
        cv::Mat currentFrame; // текущий кадр с веб. камеры
        if (useWebcam) {
            bool isSuccess = video->read(currentFrame);
            rassert(isSuccess, 347283947290039);
            rassert(!currentFrame.empty(), 347283947290040);
        } else {
            currentFrame = imgFrame; // или если у вас не работает OpenCV с веб. камерой - то пусть хотя бы картинка-пример используется
        }

        auto frameProcessingStartTime = std::chrono::steady_clock::now(); // замеряем сколько сейчас времени чтобы оценить FPS
        auto frameFilteringStartTime = std::chrono::steady_clock::now();
        auto frameMatchingStartTime = std::chrono::steady_clock::now();
        cv::Mat mainWindowImage = currentFrame.clone(); // делаем копию чтобы на ней рисовать любую отладочную информацию не портя оригинальную картинку

        {
            // TODO сопоставьте две картинки: currentFrame и imgForDetection, затем нарисуйте imgToDraw в соответствии с матрицей Гомографии
            cv::Ptr<cv::FeatureDetector> detector;
            cv::Ptr<cv::DescriptorMatcher> matcher;
            if (useSIFTDescriptor) {
                detector = cv::SIFT::create();
                matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
            } else {
                // TODO SIFT работает довольно медленно, попробуйте использовать ORB + не забудьте что тогда вам нужен другой DescriptorMatcher

                // TODO кроме того будет быстрее работать если вы будете использовать релизную сборку вместо Debug:
                // см. "Как ускорить программу" - https://www.polarnick.com/blogs/239/2021/school239_11_2021_2022/2021/10/05/lesson5-disjoint-set.html
            }
            std::vector<cv::KeyPoint> keypoints0, keypoints1; // здесь будет храниться список ключевых точек
            cv::Mat descriptors0, descriptors1; // здесь будут зраниться дескрипторы этих ключевых точек
            std::cout << "Detecting SIFT keypoints and describing them (computing their descriptors)..." << std::endl;
            detector->detectAndCompute(currentFrame, cv::noArray(), keypoints0, descriptors0);
            detector->detectAndCompute(imgForDetection, cv::noArray(), keypoints1, descriptors1);
            std::cout << "SIFT keypoints detected and described: " << keypoints0.size() << " and " << keypoints1.size() << std::endl; // TODO


            // Теперь давайте сопоставим ключевые точки между картинкой 0 и картинкой 1:
            // найдя для каждой точки из первой картинки - ДВЕ самые похожие точки из второй картинки
            frameMatchingStartTime = std::chrono::steady_clock::now();
            std::vector<std::vector<cv::DMatch>> matches01;
            matcher->knnMatch(descriptors0, descriptors1, matches01, 2); // k: 2 - указывает что мы ищем ДВЕ ближайшие точки, а не ОДНУ САМУЮ БЛИЖАЙШУЮ
            // т.к. мы для каждой точки keypoints0 ищем ближайшую из keypoints1, то сопоставлений найдено столько же сколько точек в keypoints0:


            // TODO: исследуйте минимальное/медианное/максимальное расстояние в найденных сопоставлениях
            std::vector<double> distances;
            for (int i = 0; i < matches01.size(); ++i) {
                distances.push_back( matches01[i][0].distance );
            }
            std::sort( distances.begin(), distances.end() ); // GOOGLE: "cpp how to sort vector"
                        // Теперь давайте сопоставим ключевые точки между картинкой 1 и картинкой 0 (т.е. в обратную сторону):


            // Теперь давайте попробуем убрать ошибочные сопоставления
            std::cout << "Filtering matches..." << std::endl;
            frameFilteringStartTime = std::chrono::steady_clock::now();
            std::vector<cv::Point2f> points0, points1; // здесь сохраним координаты ключевых точек для удобства позже

            for (int i = 0; i < keypoints0.size(); ++i) {
                cv::DMatch match = matches01[i][0];
                rassert(match.queryIdx == i, 234782749278097); // и вновь - queryIdx это откуда точки (поэтому всегда == i)
                int j = match.trainIdx; // и trainIdx - это какая точка из второго массива точек оказалась к нам (к queryIdx из первого массива точек) ближайшей
                rassert(j < keypoints1.size(), 38472957238099); // поэтому явно проверяем что индекс не вышел за пределы второго массива точек


                bool isOk = true;

                // TODO реализуйте фильтрацию на базе "достаточно ли похож дескриптор?" - как можно было бы подобрать порог? вспомните про вывод min/median/max раньше
                if (match.distance > distances[distances.size()/2]) {
                    isOk = false;
                }

                // TODO добавьте K-ratio тест (K=0.7), т.е. проверьте правда ли самая похожая точка сильно ближе к нашей точки (всмысле расстояния между дескрипторами) чем вторая по похожести?
                double k = 0.7;

                cv::DMatch match2 = matches01[i][1];
                if (match.distance > match2.distance*k) {
                    isOk = false;
                }


                if (isOk) {
                    points0.push_back(keypoints0[i].pt);
                    points1.push_back(keypoints1[j].pt);
                }
            }
            rassert(points0.size() == points1.size(), 3497282579850108);
            std::cout << points0.size() << "/" << keypoints0.size() << " good matches left" << std::endl;



            // TODO детектируйте и постройте дескрипторы у ключевых точек
            // std::cout << "Keypoints initially: " << keypoints0.size() << ", " << keypoints1.size() << "..." << std::endl;

            // TODO сопоставьте ключевые точки

            // TODO пофильтруйте сопоставления, как минимум через K-ratio test, но лучше на ваш выбор
//            std::vector<cv::Point2f> points0;
//            std::vector<cv::Point2f> points1;
//            for (int i = 0; i < keypoints0.size(); ++i) {
//                int fromKeyPoint0 = ....queryIdx;
//                int toKeyPoint1Best = ....trainIdx;
//                float distanceBest = ....distance;
//                rassert(fromKeyPoint0 == i, 348723974920074);
//                rassert(toKeyPoint1Best < keypoints1.size(), 347832974820076);

//                int toKeyPoint1SecondBest = ....trainIdx;
//                float distanceSecondBest = ....distance;
//                rassert(toKeyPoint1SecondBest < keypoints1.size(), 3482047920081);
//                rassert(distanceBest <= distanceSecondBest, 34782374920082);

//                if (TODO) {
//                    points0.push_back(keypoints0[i].pt);
//                    points1.push_back(keypoints1[toKeyPoint1Best].pt);
//                }
//            }
//            rassert(points0.size() == points1.size(), 234723947289089);
            // TODO добавьте вывод в лог - сколько ключевых точек было изначально, и сколько осталось сопоставлений после фильтрации

            // TODO findHomography(...) + рисование поверх:
            const double ransacReprojThreshold = 3.0;
            std::vector<unsigned char> inliersMask;
            cv::Mat H01 = cv::findHomography(points0, points1, cv::RANSAC, ransacReprojThreshold, inliersMask);
            if (H01.empty()) {
                // см. документацию https://docs.opencv.org/4.5.1/d9/d0c/group__calib3d.html#ga4abc2ece9fab9398f2e560d53c8c9780
                // "Note that whenever an H matrix cannot be estimated, an empty one will be returned."
                std::cout << "FAIL 24123422!" << std::endl;
            } else {
                cv::Mat overlapImg = imgToDraw.clone();
                if (drawDebug) {
                    // рисуем красный край у накладываемой картинки
                    cv::Scalar red(0, 0, 255);
                    cv::rectangle(overlapImg, cv::Point(0, 0), cv::Point(overlapImg.cols-1, overlapImg.rows-1), red, 2);
                }
                if (drawOver) {
                    cv::Mat H10 = H01.inv(); // у матрицы есть обратная матрица - находим ее, какое преобразование она делает?
                    cv::warpPerspective(overlapImg, mainWindowImage, H10, mainWindowImage.size(), cv::INTER_LINEAR,
                                        cv::BORDER_TRANSPARENT);
                }
            }
        }

        if (drawDebug) {
            int textYOffset = 0;

            auto frameProcessingEndTime = std::chrono::steady_clock::now();
            int timeForFrame = std::chrono::duration_cast<std::chrono::milliseconds>(frameProcessingEndTime - frameProcessingStartTime).count();
            int timeForFiltering = std::chrono::duration_cast<std::chrono::milliseconds>(frameProcessingEndTime - frameFilteringStartTime).count();
            int timeForMatching = std::chrono::duration_cast<std::chrono::milliseconds>(frameFilteringStartTime - frameMatchingStartTime).count();
            int timeForDetecting = std::chrono::duration_cast<std::chrono::milliseconds>(frameMatchingStartTime - frameProcessingStartTime).count();
            int fps;
            if (timeForFrame == 0) {
                fps = 99999;
            } else {
                fps = (int) std::round(1000.0 / timeForFrame);
            }
            drawText(mainWindowImage, std::to_string(fps) + " FPS", 0.5, textYOffset);

            // TODO добавьте короткую справку про кнопки управления
            drawText(mainWindowImage, "Controls: 1 - change current frame, 2 - use current frame as imgtodraw, 3 - show debug, S - change method, H - DrawOver", 0.5, textYOffset);

            // TODO добавьте разбивку сколько времени занимает детектирование, сколько матчинг, сколько фильтрация (по аналогии с тем как выше замерялось время на обработку для рассчета FPS):
            drawText(mainWindowImage, "Timings: " + std::to_string(timeForFrame) + " ms = "
                   + std::to_string(timeForDetecting) + " ms detect + " + std::to_string(timeForMatching)+" ms matching" + std::to_string(timeForFiltering) + " ms filtering",
                    0.5, textYOffset);
        }

        // Рисуем все три окошка:
        cv::imshow("Frame", mainWindowImage);
        cv::imshow("Image for detection", imgForDetection);
        cv::imshow("Image to draw", imgToDraw);

        // Смотрим нажал ли пользователь какую-нибудь кнопку
        int key = cv::waitKey(10); // число в скобочках - максимальное число миллисекунд которые мы ждем кнопки от пользователя, а иначе - считаем что ничего не нажато
        if (key == -1) {
            // прошло 5 миллисекунд но ничего не было нажато - значит идем обрабатывать следующий кадр с веб. камеры
        } else if (key == 27) { // Esc - выключаем программу
            break;
        }else if (useWebcam && key == 49) {
            imgForDetection = currentFrame.clone();
        } else if (useWebcam && key == 50) {
            imgToDraw = currentFrame.clone();
        } else if (key == 104) {
            drawOver = !drawOver;
        } else if (key == 51) {
            drawDebug = !drawDebug;
        } else if (key == 115) {
            useSIFTDescriptor = !useSIFTDescriptor;
        } else {
            std::cerr << "UNKNOWN KEY " << key << " WAS PRESSED" << std::endl;
        }
    }
}


int main() {
    try {
        run();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
