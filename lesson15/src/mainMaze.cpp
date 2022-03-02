#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include<set>

#include <libutils/rasserts.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>

#define int long long
using namespace std;



struct Edge {
    int u, v; // номера вершин которые это ребро соединяет
    int w; // длина ребра (т.е. насколько длинный путь предстоит преодолеть переходя по этому ребру между вершинами)

    Edge(int u, int v, int w) : u(u), v(v), w(w)
    {}
};

// Эта биективная функция по координате пикселя (строчка и столбик) + размерам картинки = выдает номер вершины
int encodeVertex(int row, int column, int nrows, int ncolumns) {
    rassert(row < nrows, 348723894723980017);
    rassert(column < ncolumns, 347823974239870018);
    int vertexId = row * ncolumns + column;
    return vertexId;
}

// Эта биективная функция по номеру вершины говорит какой пиксель этой вершине соовтетствует (эта функция должна быть симметрична предыдущей!)
cv::Point2i decodeVertex(int vertexId, int nrows, int ncolumns) {

    // TODO: придумайте как найти номер строки и столбика пикселю по номеру вершины (просто поймите предыдущую функцию и эта функция не будет казаться сложной)
    int row = vertexId/ncolumns;
    int column = vertexId % ncolumns;

    // сверим что функция симметрично сработала:
    rassert(encodeVertex(row, column, nrows, ncolumns) == vertexId, 34782974923035);

    rassert(row < nrows, 34723894720027);
    rassert(column < ncolumns, 3824598237592030);
    return cv::Point2i(column, row);
}

void run(int mazeNumber) {
    cv::Mat maze = cv::imread("lesson15/data/mazesImages/maze" + std::to_string(mazeNumber) + ".png");
    rassert(!maze.empty(), 324783479230019);
    rassert(maze.type() == CV_8UC3, 3447928472389020);
    std::cout << "Maze resolution: " << maze.cols << "x" << maze.rows << std::endl;

    int nvertices = maze.cols * maze.rows; // TODO
    vector<vector<pair<int, int>>> g(nvertices);

    std::vector<std::vector<Edge>> edges_by_vertex(nvertices);
    for (int j = 0; j < maze.rows; ++j) {
        for (int i = 0; i < maze.cols; ++i) {
            cv::Vec3b color = maze.at<cv::Vec3b>(j, i);
            unsigned char blue = color[0];
            unsigned char green = color[1];
            unsigned char red = color[2];

            // TODO добавьте соотвтетсвующие этому пикселю ребра

            if(j + 1 < maze.rows){
                cv::Vec3b color1 = maze.at<cv::Vec3b>(j+1,i);
                unsigned char blue1 = color1[0];
                unsigned char green1 = color1[1];
                unsigned char red1 = color1[2];
                int d = round(sqrt((blue - blue1) * (blue - blue1) + (red - red1) * (red - red1) + (green - green1) * (green - green1))) + 1;
                g[encodeVertex(j,i,maze.rows,maze.cols)].push_back({encodeVertex(j+1,i,maze.rows,maze.cols), d});
                g[encodeVertex(j+1,i,maze.rows,maze.cols)].push_back({encodeVertex(j,i,maze.rows,maze.cols), d});
            }if(i+1 < maze.cols){
                cv::Vec3b color1 = maze.at<cv::Vec3b>(j,i+1);
                unsigned char blue1 = color1[0];
                unsigned char green1 = color1[1];
                unsigned char red1 = color1[2];
                int d = round(sqrt((blue - blue1) * (blue - blue1) + (red - red1) * (red - red1) + (green - green1) * (green - green1))) + 1;
                g[encodeVertex(j,i+1,maze.rows,maze.cols)].push_back({encodeVertex(j,i,maze.rows,maze.cols), d});
                g[encodeVertex(j,i,maze.rows,maze.cols)].push_back({encodeVertex(j,i + 1,maze.rows,maze.cols), d});
            }if(j>0){
                cv::Vec3b color1 = maze.at<cv::Vec3b>(j-1,i);
                unsigned char blue1 = color1[0];
                unsigned char green1 = color1[1];
                unsigned char red1 = color1[2];
                int d = round(sqrt((blue - blue1) * (blue - blue1) + (red - red1) * (red - red1) + (green - green1) * (green - green1))) + 1;
                g[encodeVertex(j,i,maze.rows,maze.cols)].push_back({encodeVertex(j-1,i,maze.rows,maze.cols), d});
                g[encodeVertex(j-1,i,maze.rows,maze.cols)].push_back({encodeVertex(j,i,maze.rows,maze.cols), d});
            }if(i>0){
                cv::Vec3b color1 = maze.at<cv::Vec3b>(j,i-1);
                unsigned char blue1 = color1[0];
                unsigned char green1 = color1[1];
                unsigned char red1 = color1[2];
                int d = round(sqrt((blue - blue1) * (blue - blue1) + (red - red1) * (red - red1) + (green - green1) * (green - green1))) + 1;
                g[encodeVertex(j,i,maze.rows,maze.cols)].push_back({encodeVertex(j,i-1,maze.rows,maze.cols), d});
                g[encodeVertex(j,i-1,maze.rows,maze.cols)].push_back({encodeVertex(j,i,maze.rows,maze.cols), d});
            }
        }
    }

    int start, finish;
    if (mazeNumber >= 1 && mazeNumber <= 3) { // Первые три лабиринта очень похожи но кое чем отличаются...
        start = encodeVertex(300, 300, maze.rows, maze.cols);
        finish = encodeVertex(0, 305, maze.rows, maze.cols);
    } else if (mazeNumber == 4) {
        start = encodeVertex(154, 312, maze.rows, maze.cols);
        finish = encodeVertex(477, 312, maze.rows, maze.cols);
    } else if (mazeNumber == 5) { // Лабиринт в большом разрешении, добровольный (на случай если вы реализовали быструю Дейкстру с приоритетной очередью)
        start = encodeVertex(1200, 1200, maze.rows, maze.cols);
        finish = encodeVertex(1200, 1200, maze.rows, maze.cols);
    } else {
        rassert(false, 324289347238920081);
    }



    cv::Mat window = maze.clone(); // на этой картинке будем визуализировать до куда сейчас дошла прокладка маршрута


    // TODO СКОПИРУЙТЕ СЮДА ДЕЙКСТРУ ИЗ ПРЕДЫДУЩЕГО ИСХОДНИКА
    set<pair<int, int>> edges;
    edges.insert({0,start});
    int timer = 1000;
    const int INF = 1e18;
    vector<int> d(nvertices,INF);
    d[start] = 0;
    vector<int> p(nvertices);
    while(!edges.empty()){
        pair<int, int> e = *edges.begin();
        edges.erase(e);
        int len = e.first, v = e.second;
        if(timer == 1000) {
            cv::Point2i pp = decodeVertex(v, maze.rows, maze.cols);
            window.at<cv::Vec3b>(pp.y, pp.x) = cv::Vec3b(0, 255, 0);
            cv::imshow("Maze", window);
            cv::waitKey(1);
        }
        timer--;
        if(timer<0) timer = 1000;
        for(auto to: g[v]){
            if(to.second + len < d[to.first]){
                edges.erase({d[to.first], to.first});
                edges.insert({to.second + len, to.first});
                d[to.first] = to.second + len;
                p[to.first] = v;
            }
        }
    }
    cout<<"kek\n";
    vector<int> ans;
    ans.reserve(nvertices);
    ans.push_back(finish);
    int cur = finish;
    while(p[cur] != start){
        ans.push_back(p[cur]);
        cur = p[cur];
    }ans.push_back(start);
    for(int i = ans.size()-1;i>=0;--i) {
        cv::Point pp = decodeVertex(ans[i], maze.rows,maze.cols);
        window.at<cv::Vec3b>(pp.y, pp.x) = {0,0,255};
    }


    // TODO в момент когда вершина становится обработанной - красьте ее на картинке window в зеленый цвет и показывайте картинку:
    //    cv::Point2i p = decodeVertex(the_chosen_one, maze.rows, maze.cols);
    //    window.at<cv::Vec3b>(p.y, p.x) = cv::Vec3b(0, 255, 0);
    //    cv::imshow("Maze", window);
    //    cv::waitKey(1);
    // TODO это может тормозить, в таком случае показывайте window только после обработки каждой сотой вершины

    // TODO обозначьте найденный маршрут красными пикселями

    // TODO сохраните картинку window на диск

    std::cout << "Finished!" << std::endl;

    // Показываем результат пока пользователь не насладиться до конца и не нажмет Escape
    while (cv::waitKey(10) != 27) {
        cv::imshow("Maze", window);
    }
    cv::imwrite("lesson15/resultsData/maze" + to_string(mazeNumber) + ".png", window);
}

int32_t main() {
    try {
        //int mazeNumber = 1;
        for(int mazeNumber = 5;mazeNumber<=5;++mazeNumber) run(mazeNumber);

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
