#ifndef STRATEGY_H
#define STRATEGY_H
#include "shape/shape.h"
#include <QList>

class Strategy
{
public:
    Strategy() = default;
    virtual void align(QList<Shape*> &data) = 0;
};


class LeftStrategy : public Strategy{
public:
    void align(QList<Shape*> &data){
        // 找x最小
        qreal minx = 1000;
        foreach(auto it, data){
            minx = minx > it->getStart().x() ? it->getStart().x() : minx;
        }
        // 全部设置为最小
        foreach(auto it, data){
            auto ty = it->getStart().y();
            it->setStart( {minx, ty} );
        }
    }
};


class RightStrategy : public Strategy{
public:
    void align(QList<Shape*> &data){
        // 找x最大
        qreal maxx = -1000;
        foreach(auto it, data){
            maxx = maxx < it->getEnd().x() ? it->getEnd().x() : maxx;
        }
        // 全部设置为最大
        foreach(auto it, data){
            auto ty = it->getStart().y();
            auto tex = it->getEnd().x();
            auto tx = it->getStart().x() + maxx - tex;
            it->setStart( {tx, ty} );
        }
    }
};

class UpStrategy : public Strategy{
public:
    void align(QList<Shape*> &data){
        // 找y最小
        qreal miny = 1000;
        foreach(auto it, data){
            miny = miny > it->getStart().y() ? it->getStart().y() : miny;
        }
        // 全部设置为最小
        foreach(auto it, data){
            auto tx = it->getStart().x();
            it->setStart( {tx, miny} );
        }
    }
};

class DownStrategy  : public Strategy{
public:
    void align(QList<Shape*> &data){
        // 找y最大
        qreal maxy = -1000;
        foreach(auto it, data){
            maxy = maxy < it->getEnd().y() ? it->getEnd().y() : maxy;
        }
        // 全部设置为最大
        foreach(auto it, data){
            auto tx = it->getStart().x();
            auto tey = it->getEnd().y();
            auto ty = it->getStart().y() + maxy - tey;
            it->setStart( {tx, ty} );
        }
    }
};

#endif // STRATEGY_H
