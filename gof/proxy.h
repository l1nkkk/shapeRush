#ifndef PROXY_H
#define PROXY_H
#include "shape/shape.h"
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QVariantList>

class QGraphicsImageItemWrap:
        public ShapeInterface,
        public QGraphicsRectItem{
    Q_OBJECT
public:
    QGraphicsImageItemWrap(Shape *base, QGraphicsItem* itemp = nullptr)
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
        m_base->getPen().setWidth(2);


        QPixmap pixmap = QPixmap("/home/l1nkkk/project/mime/shape/shapeRush/storage/data/1.gif");
        QPixmap fitpixmap = pixmap.scaled(400, 300, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_base->getBrush().setTexture(fitpixmap);
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


class ImageItem:public Shape
{
public:
    typedef std::shared_ptr<ImageItem> sptr;
    typedef std::weak_ptr<ImageItem> wptr;
public:
    ImageItem(QPointF start, QPointF end, QString name, QColor lineColor = Qt::black, \
              QColor focusLineColor = Qt::gray, QColor fillColor = Qt::transparent) :
            Shape(Shape::RECT, start, end,name, lineColor, focusLineColor, fillColor){
        m_itemWrap = new QGraphicsImageItemWrap(this);
    }

    ImageItem(const ImageItem& r):Shape(r){
        m_itemWrap = new QGraphicsImageItemWrap(this);
    }

    Shape * Clone()override{
        return dynamic_cast<Shape*>(new ImageItem(*this));
    }
private:
    QGraphicsImageItemWrap* m_itemWrap = nullptr;

};


// -----------------------proxy

class QGraphicsImageProxyItemWrap:
        public ShapeInterface,
        public QGraphicsRectItem
{
    Q_OBJECT
public:
    QGraphicsImageProxyItemWrap(Shape *base, QGraphicsItem* itemp = nullptr)
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
        m_base->getPen().setWidth(2);
        painter->setPen(m_base->getPen());
        painter->setBrush(m_base->getBrush());
        painter->drawRect(0, 0 ,\
                          qAbs(m_base->getStart().x() - m_base->getEnd().x()), qAbs(m_base->getStart().y() - m_base->getEnd().y()));
        painter->drawText(0, 0,
                          boundingRect().width(), boundingRect().height(),
                          Qt::AlignHCenter | Qt::AlignVCenter,"点击加载皮卡丘");
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


class ImageProxy:public Shape
{
public:
    typedef std::shared_ptr<ImageProxy> sptr;
    typedef std::weak_ptr<ImageProxy> wptr;
public:
    ImageProxy(QPointF start, QPointF end, QString name, QColor lineColor = Qt::black, \
             QColor focusLineColor = Qt::gray, QColor fillColor = Qt::transparent) :
            Shape(Shape::RECT, start, end,name, lineColor, focusLineColor, fillColor){
        m_itemWrap = new QGraphicsImageProxyItemWrap(this);
        realSubject = new ImageItem(start, end, name);
        m_proxy = this;
    }

    ImageProxy(const ImageProxy& r):Shape(r){
        m_itemWrap = new QGraphicsImageProxyItemWrap(this);
    }

    Shape * Clone()override{
        return dynamic_cast<Shape*>(new ImageProxy(*this));
    }

    ImageItem* getSubject() { return realSubject; }
private:
    QGraphicsImageProxyItemWrap* m_itemWrap = nullptr;
    ImageItem* realSubject = nullptr;
};

#endif // PROXY_H
