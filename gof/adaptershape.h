#ifndef ADAPTERSHAPE_H
#define ADAPTERSHAPE_H
#include <QPushButton>
#include <QPointF>
#include <QGraphicsItem>
#include <memory>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QMenu>
#include <QSize>
#include <QVariantList>
#include "shape/shape.h"

class QGraphicsProxyWidgetWrap : public ShapeInterface, public QGraphicsProxyWidget{
public:
    QGraphicsProxyWidgetWrap(Shape *base, QGraphicsItem* itemp = nullptr)
        : m_base(base), QGraphicsProxyWidget(itemp){
        btn = new QPushButton("l1nkkk");
        btn->setFixedSize(QSize(qAbs(m_base->getEnd().x() - m_base->getStart().x()),
                                     qAbs(m_base->getEnd().y() - m_base->getStart().y())));
        this->setWidget(btn);
        this->setPos(m_base->getStart().toPoint());
        _init();
    }
    Shape* getBase() const { return m_base; }
    ~QGraphicsProxyWidgetWrap(){
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

signals:
//    void shapeCopy(Shape *cpShape);
//    void compositeEvent(bool flag);
//    void cancelCompositeEvent(Composite *cps);
//    void deleteEvent(Shape *sp);
//    void spinDeractor(Shape *sp);
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override{
        QGraphicsProxyWidget::mouseMoveEvent(event);
        QPointF y = mapToScene(event->pos());
        QPointF x = y - m_z;

        if(widget() != nullptr)
            setGeometry(QRectF(x.rx(), x.ry(), widget()->width(), widget()->height()));
    }
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override{
        QGraphicsProxyWidget::mousePressEvent(event);
        QPointF y = mapToScene(event->pos());
        QPointF x = scenePos();
        m_z = y - x;


    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override{
        QGraphicsProxyWidget::mouseReleaseEvent(event);
        m_z = QPointF();
    }
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override{
        QMenu menu;
        QAction *moveAction = menu.addAction("移动");
        QAction *cloneAction = menu.addAction("clone");
        QAction *compositeAction = menu.addAction("组合");
        QAction *deleteAction = menu.addAction("删除");
        QAction *spinAction = menu.addAction("旋转");
        QAction *textAction = menu.addAction("添加文本");

        QAction *selectedAction = menu.exec(event->screenPos());

        if(selectedAction == moveAction){
            setPos(0,0);
        } else if(selectedAction == cloneAction){
            emit shapeCopy(this->m_base);
        } else if(selectedAction == compositeAction){
            emit compositeEvent(true);
        } else if(selectedAction == deleteAction){
            emit deleteEvent(this->m_base);
        } else if(selectedAction == spinAction){
            emit spinDeractor(this->m_base);
        } else if(selectedAction == textAction){
            emit textDeractor(this->m_base);
        }
    }

private:
    Shape *m_base = nullptr;
    QPushButton *btn = nullptr;
    QPointF m_z;
};

class ShapeButton:public Shape{
public:
    ShapeButton(QPointF start, QPointF end, QString name, QGraphicsItem *parent = nullptr, \
                Qt::WindowFlags wFlags = nullptr):Shape(Shape::BUTTON, start, end, name, Qt::black, Qt::green, Qt::red){
         m_itemWrap = new QGraphicsProxyWidgetWrap(this);
    }

    ShapeButton(const ShapeButton& s): Shape(s){
        m_itemWrap = new QGraphicsProxyWidgetWrap(this);
    }

    Shape *Clone() override{
        return dynamic_cast<Shape*>(new ShapeButton(*this));
    }

private:
    QGraphicsProxyWidgetWrap* m_itemWrap = nullptr;
};

#endif // ADAPTERSHAPE_H
