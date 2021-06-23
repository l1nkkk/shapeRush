#ifndef COMPOSITE_H
#define COMPOSITE_H
#include <QGraphicsItemGroup>
#include <QGraphicsItem>
#include "shape/shape.h"
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QVariantList>
class QGraphicsItemGroupWrap :public ShapeInterface, public QGraphicsItemGroup{
    Q_OBJECT
public:
    QGraphicsItemGroupWrap(Shape *base, QGraphicsItem* itemp = nullptr)
        : m_base(base), QGraphicsItemGroup(itemp){
        _init();
    }

    Shape* getBase() const { return m_base; }

private:
    void _init(){

        m_base->setInter(this);
        m_base->setItem(this);
        m_base->getItem()->setData(0,m_base->getVariant());

        setFlag(QGraphicsItem::ItemIsFocusable); // 可聚焦
        setFlag(QGraphicsItem::ItemIsMovable); // 可拖动
        setAcceptHoverEvents(true);
    }
signals:
//    void shapeCopy(Shape *cpShape);
//    void compositeEvent(bool flag);
//    void cancelCompositeEvent(Composite *cps);
//    void deleteEvent(Shape *sp);
//    void spinDeractor(Shape *sp);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
private:
    Shape *m_base = nullptr;
};

// QGraphicsItemGroup
class Composite:public Shape
{
public:
    Composite():Shape(Shape::COMPOSITE, QPointF(0,0),QPointF(0,0),"COMPOSITE-", \
                      Qt::black, Qt::black, Qt::black){
        m_itemWrap = new QGraphicsItemGroupWrap(this);
        setComposite(this);
    }

    Composite(const Composite& c):Shape(c){
        m_itemWrap = new QGraphicsItemGroupWrap(this);
        setComposite(this);
    }

    Shape * Clone()override{
//        auto rt = dynamic_cast<Shape*>(new Composite(*this));
        auto rt =new Composite(*this);
        for(auto &it : m_leaf){
            Shape* copyShape = it->Clone();
            copyShape->getItem()->setPos(copyShape->getStart());
            rt->m_leaf.append(copyShape);
            rt->m_itemWrap->addToGroup(copyShape->getItem());
        }
        return rt;
    }

    void addChild(Shape* child){
        this->m_itemWrap->addToGroup(child->getItem());
        m_leaf.append(child);
        setChildNum(getChildNum()+1);
    }
    void removeChild(Shape* child){
        this->m_itemWrap->removeFromGroup(child->getItem());
        m_leaf.removeOne(child);
        setChildNum(getChildNum()-1);
    }

    Composite* getComposite() override {
        return this;
    }


public:
    QList<Shape*> m_leaf;
    QGraphicsItemGroupWrap* m_itemWrap = nullptr;

};

#endif // COMPOSITE_H
