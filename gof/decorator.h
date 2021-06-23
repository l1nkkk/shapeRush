#ifndef DECORATOR_H
#define DECORATOR_H
#include "../shape/shapeImport.h"
#include <QTimer>
#include <QVariantList>
class DecoratorItem: public Shape
{
public:
    DecoratorItem(Shape *ref, Shape::CreateType type,QString name):
        Shape(type, name), m_ref(ref){
        setIsDecorator(this);
        setChildNum(1);
    }
    Shape* getRef() { return m_ref; }
    void setRef(Shape* shape){ m_ref = shape; }
    virtual QGraphicsItemGroup* getGroupItem() { return nullptr; }
protected:
    Shape *m_ref;
};


class TextItemWrap :public ShapeInterface,public QGraphicsItemGroup{
    Q_OBJECT
public:
    TextItemWrap(DecoratorItem *base, QGraphicsItem *item = nullptr):
        QGraphicsItemGroup(item){
        m_base = base;

        _init();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override{

        if (hasFocus())
            m_base->getPen().setColor(m_base->getRef()->getColor().focusLineColor);
        else
            m_base->getPen().setColor(m_base->getRef()->getColor().lineColor);

        if(m_base->isSelect())
            m_base->getBrush().setColor(QColor(219,217,216,150));
        else
            m_base->getBrush().setColor(m_base->getRef()->getColor().fillColor);

        painter->setPen(m_base->getPen());
        painter->setBrush(m_base->getBrush());
        painter->drawText(0, 0,
                          boundingRect().width(), boundingRect().height(),
                          Qt::AlignHCenter | Qt::AlignVCenter,"l1nkkk");
    };
    QRectF boundingRect () const override {
        return m_base->getDecorator()->getRef()->getItem()->boundingRect();
    }

private:
    void _init(){
        m_base->setInter(this);
        m_base->setItem(this);
        m_base->getItem()->setData(0, m_base->getVariant());
        setFlag(QGraphicsItem::ItemIsFocusable); // 可聚焦
        setFlag(QGraphicsItem::ItemIsMovable); // 可拖动
        setAcceptHoverEvents(true);
        if(nullptr != m_base->getRef())
            addToGroup(m_base->getRef()->getItem());
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
private:
    DecoratorItem *m_base;
};



class TextItem : public DecoratorItem{
public:
    TextItem(Shape *ref):DecoratorItem(ref, CreateType::TEXT,"TEXT-"){
        m_itermWrap = new TextItemWrap(this);
    }

    Shape * Clone()override{
        auto refClone = m_ref->Clone();
        auto rtn = new TextItem(refClone);
        return rtn;
    }
    QGraphicsItemGroup* getGroupItem() override { return m_itermWrap; }
private:
    TextItemWrap* m_itermWrap = nullptr;
};








class SpinItemWrap :public ShapeInterface,public QGraphicsItemGroup{
    Q_OBJECT
public:
    SpinItemWrap(DecoratorItem *base, QGraphicsItem *item = nullptr):
        QGraphicsItemGroup(item){
        m_base = base;

        m_angle = 0;
        m_delta = 0;
        m_current = 0;

        tm = new QTimer;
        tm->setInterval(2000);
        QObject::connect(tm, &QTimer::timeout, this, &SpinItemWrap::flip);
        tm->start();

        //m_base->getRef()->getItem()->setParentItem(this);

        _init();
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override{
        if (hasFocus())
            m_base->getPen().setColor(m_base->getRef()->getColor().focusLineColor);
        else
//            m_base->getPen().setColor(m_base->getRef()->getColor().lineColor);
            m_base->getPen().setColor(Qt::white);

        if(m_base->isSelect())
            m_base->getBrush().setColor(QColor(219,217,216,0));
        else
            m_base->getBrush().setColor(QColor(219,217,216,0));

        painter->setPen(m_base->getPen());
        painter->setBrush(m_base->getBrush());
        painter->drawRect(0, 0 ,\
                          qAbs(m_base->getRef()->getStart().x() - m_base->getRef()->getEnd().x()), qAbs(m_base->getRef()->getStart().y() - m_base->getRef()->getEnd().y()));
    };

    QRectF boundingRect () const override {
        return m_base->getDecorator()->getRef()->getItem()->boundingRect();
    }
private:
    void _init(){
        m_base->setInter(this);
        m_base->setItem(this);
        m_base->getItem()->setData(0, m_base->getVariant());
        setFlag(QGraphicsItem::ItemIsFocusable); // 可聚焦
        setFlag(QGraphicsItem::ItemIsMovable); // 可拖动
        setAcceptHoverEvents(true);
        if(m_base->getRef() != nullptr)
            addToGroup(m_base->getRef()->getItem());
        // m_base->getRef()->getItem()->stackBefore(this);
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;


// 添加特性
public slots:
    void flip();
private slots:
    void animateFilp();
private:
    DecoratorItem *m_base;
    QTimer *tm;
    int m_angle;   //rotation angle
    int m_delta;
    int m_current; //store rotation direction
};


class SpinItem :public DecoratorItem{
public:
    SpinItem(Shape *ref):DecoratorItem(ref,CreateType::SPIN, "SPIN-"){
        m_itermWrap = new SpinItemWrap(this);
    }

    Shape * Clone()override{
        auto refClone = m_ref->Clone();
        auto rtn = new SpinItem(refClone);
        return rtn;
    }
    QGraphicsItemGroup* getGroupItem() override { return m_itermWrap; }
private:
    SpinItemWrap* m_itermWrap = nullptr;

};


#endif // DECORATOR_H
