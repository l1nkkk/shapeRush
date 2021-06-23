#ifndef LINEITEM_H
#define LINEITEM_H

#include <shape/shape.h>
#include <QVariantList>
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>

class QGraphicsLineItemWrap:
        public ShapeInterface,
        public QGraphicsLineItem{
    Q_OBJECT
public:
    QGraphicsLineItemWrap(Shape *base, QGraphicsItem* itemp = nullptr)
        : m_base(base), QGraphicsLineItem(itemp){
        this->setPen(m_base->getPen());
        this->setLine(QLine(0, 0, m_base->getEnd().toPoint().x()- m_base->getStart().toPoint().x(),
                            m_base->getEnd().toPoint().y()- m_base->getStart().toPoint().y()));
        _init();
    }
    Shape* getBase() const { return m_base; }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override{

        if (m_base->isSelect())
            m_base->getPen().setColor(m_base->getColor().focusLineColor);
        else
            m_base->getPen().setColor(m_base->getColor().lineColor);


        painter->setPen(m_base->getPen());
        painter->drawLine(QLine(0, 0, m_base->getEnd().toPoint().x()- m_base->getStart().toPoint().x(),
                                m_base->getEnd().toPoint().y()- m_base->getStart().toPoint().y()));
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

class LineItem : public Shape
{
public:
    typedef std::shared_ptr<LineItem> sptr;
    typedef std::weak_ptr<LineItem> wptr;
public:
    LineItem(QPointF start, QPointF end, QString name, QColor lineColor = Qt::black, \
             QColor focusLineColor = Qt::green, QColor fillColor = QColor(219,217,216,150)) :
            Shape(Shape::LINE, start, end,name, lineColor, focusLineColor, fillColor){
        m_itemWrap = new QGraphicsLineItemWrap(this);
    }

    LineItem(const LineItem& l):Shape(l){
        m_itemWrap = new QGraphicsLineItemWrap(this);
    }


    Shape * Clone()override{
        return dynamic_cast<Shape*>(new LineItem(*this));
    }
private:
    QGraphicsLineItemWrap* m_itemWrap = nullptr;


};


#endif // LINEITEM_H
