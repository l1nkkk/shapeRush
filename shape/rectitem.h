#ifndef RECTITEM_H
#define RECTITEM_H
#include "shape/shape.h"
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QVariantList>
class QGraphicsRectItemWrap:
        public ShapeInterface,
        public QGraphicsRectItem
{
    Q_OBJECT
public:
    QGraphicsRectItemWrap(Shape *base, QGraphicsItem* itemp = nullptr)
        : m_base(base), QGraphicsRectItem(itemp){

        this->setPen(m_base->getPen());
        this->setBrush(m_base->getColor().fillColor);
        this->setRect(0, 0, qAbs(m_base->getStart().x() - m_base->getEnd().x()),\
                                      qAbs(m_base->getStart().y() - m_base->getEnd().y()));

        _init();
    }

    Shape* getBase() const { return m_base; }


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override{

        if (m_base->isSelect())
            m_base->getPen().setColor(m_base->getColor().focusLineColor);
        else
            m_base->getPen().setColor(m_base->getColor().lineColor);

//        if(m_base->isSelect())
//            m_base->getBrush().setColor(QColor(219,217,216,150));
//        else
//            m_base->getBrush().setColor(m_base->getColor().fillColor);

        painter->setPen(m_base->getPen());
        painter->setBrush(m_base->getBrush());
        painter->drawRect(0, 0 ,\
                          qAbs(m_base->getStart().x() - m_base->getEnd().x()), qAbs(m_base->getStart().y() - m_base->getEnd().y()));

    }
private:
    void _init(){
        m_base->setInter(this);
        m_base->setItem(this);
        m_base->getItem()->setData(0,m_base->getVariant());

        setFlag(QGraphicsItem::ItemIsFocusable); // 可聚焦
        setFlag(QGraphicsItem::ItemIsMovable); // 可拖动
        setAcceptHoverEvents(true);
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
private:
    Shape *m_base = nullptr;
};



class RectItem : public Shape
{
public:
    typedef std::shared_ptr<RectItem> sptr;
    typedef std::weak_ptr<RectItem> wptr;
public:
    RectItem(QPointF start, QPointF end, QString name, QColor lineColor = Qt::black, \
             QColor focusLineColor = Qt::green, QColor fillColor = QColor(219,217,216,150)) :
            Shape(Shape::RECT, start, end,name, lineColor, focusLineColor, fillColor){
        m_itemWrap = new QGraphicsRectItemWrap(this);
    }

    RectItem(const RectItem& r):Shape(r){
        m_itemWrap = new QGraphicsRectItemWrap(this);
    }

    Shape * Clone()override{
        return dynamic_cast<Shape*>(new RectItem(*this));
    }
private:
    QGraphicsRectItemWrap* m_itemWrap = nullptr;


};


#endif // RECTITEM_H
