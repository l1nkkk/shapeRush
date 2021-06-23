#include "decorator.h"
#include <QMenu>



void TextItemWrap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    setCursor(Qt::ClosedHandCursor);
}



void TextItemWrap::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    setToolTip("test");
}

void TextItemWrap::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
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









// -----------------------SpinItem


void SpinItemWrap::animateFilp()
{
    m_angle += m_delta;

    if(180 == m_angle) {
        m_current ^= 1;
    }
    QRectF r = boundingRect();
    setTransform(QTransform()
                 .translate(r.width() / 2, r.height() /2)
                 .rotate(m_angle, Qt::YAxis)
                 .translate(-r.width() / 2, -r.height() / 2));
    if((0 == m_current && m_angle > 0) || (1 == m_current && m_angle < 360)) {
        QTimer::singleShot(25, this, &SpinItemWrap::animateFilp);

    }
}

void SpinItemWrap::flip()
{
    m_delta = (m_current == 0 ? 9 : -9);
    animateFilp();
}


void SpinItemWrap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    setCursor(Qt::ClosedHandCursor);
}



void SpinItemWrap::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    setToolTip("test");
}

void SpinItemWrap::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
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
