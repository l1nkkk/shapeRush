#ifndef FACTORY_H
#define FACTORY_H

#include "shape/shapeImport.h"
#include "gof/adaptershape.h"
#include "gof/proxy.h"

class FactoryShape
{
public:
//    FactoryShape() = delete;
    virtual LineItem* CreateLine(QPointF start, QPointF end){
        return  new LineItem(start, end, "LINE-");
    }
    virtual RectItem* CreateRect(QPointF start, QPointF end){
        return  new RectItem(start, end, "RECT-");
    }
    virtual ElliItem* CreateElli(QPointF start, QPointF end){
        return  new ElliItem(start, end, "ELLI-");
    }
    virtual ShapeButton* CreateButton(QPointF start, QPointF end){
        return  new ShapeButton(start, end, "BUTTON-");
    }
    virtual ImageProxy* CreateImage(QPointF start, QPointF end){
        return new ImageProxy(start, end, "Image-");
    }
};

//抽象工厂
class ExFactoryShape: public FactoryShape{
public:
//    ExFactor() = delete;
    LineItem* CreateLine(QPointF start, QPointF end) override {
        auto rt = new LineItem(start, end, "EX_LINE-", Qt::black, Qt::gray, Qt::transparent);
        rt->setIsEnhanced(true);
        return rt;
    }
    RectItem* CreateRect(QPointF start, QPointF end) override {
        auto rt = new RectItem(start, end, "EX_RECT-", Qt::black, Qt::gray, Qt::transparent);
        rt->setIsEnhanced(true);
        return rt;
    }
    ElliItem* CreateElli(QPointF start, QPointF end) override {
        auto rt = new ElliItem(start, end, "EX_ELLI-", Qt::black, Qt::gray, Qt::transparent);
        rt->setIsEnhanced(true);
        return rt;
    }
};


#endif // FACTORY_H
