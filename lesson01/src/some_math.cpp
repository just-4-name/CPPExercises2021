#include <cfloat>
#include <valarray>
#include "some_math.h"
double INF = DBL_MAX;
int fibbonachiRecursive(int n) {
    // TODO 03 реализуйте функцию которая считает числа Фибоначчи - https://ru.wikipedia.org/wiki/%D0%A7%D0%B8%D1%81%D0%BB%D0%B0_%D0%A4%D0%B8%D0%B1%D0%BE%D0%BD%D0%B0%D1%87%D1%87%D0%B8
    if(n == 0) return 0;
    if(n == 1) return 1;
    return(fibbonachiRecursive(n-1) + fibbonachiRecursive(n-2));
}

int fibbonachiFast(int n) {
    // TODO 04 реализуйте быструю функцию Фибоначчи с использованием std::vector
    std::vector<int> f(std::max(3,n));
    f[0] = 0;
    f[1] = 1;
    for(int i=2;i<n;++i){
        f[i] = f[i-1] + f[i-2];
    }
    return f[n];
}

double solveLinearAXB(double a, double b) {
    // TODO 10 решите линейное уравнение a*x+b=0 а если решения нет - верните наибольшее значение double - найдите как это сделать в интернете по запросу "so cpp double max value" (so = stackoverflow = сайт где часто можно найти ответы на такие простые запросы), главное НЕ КОПИРУЙТЕ ПРОСТО ЧИСЛО, ПОЖАЛУЙСТААаа
    // если решений сколь угодно много - верните максимальное значение со знаком минус
    double x = 0.0;
    if(a==0){
        if(b==0){
            x = -INF;
        }else x = INF;
    }else x = -b/a;

    return x;
}

std::vector<double> solveSquare(double a, double b, double c) {
    // TODO 20 решите квадратное уравнение вида a*x^2+b*x+c=0
    // если корня два - они должны быть упорядочены по возрастанию
    std::vector<double> results;
    if(a!=0){
        double D = b*b - 4*a*c;
        if(D==0){
            double ans = -b/2*a;
            results.push_back(ans);
        }
        if(D>0){
            double x1,x2;
            x1 = (-b+sqrt(D))/(2*a);
            x2 = (-b-sqrt(D))/(2*a);
            if(x1>x2) std::swap(x1,x2);
            results.push_back(x1);
            results.push_back(x2);
        }
    }else{
        results.push_back(solveLinearAXB(b,c));
    }
    return results;
}
