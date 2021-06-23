#include "proxy.h"
#include <QMenu>

void QGraphicsImageItemWrap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    setCursor(Qt::ClosedHandCursor);
}



void QGraphicsImageItemWrap::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    setToolTip("test");
}

void QGraphicsImageItemWrap::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *moveAction = menu.addAction("移动");
    QAction *cloneAction = menu.addAction("clone");
    QAction *compositeAction = menu.addAction("组合");
    QAction *deleteAction = menu.addAction("删除");
    QAction *spinAction = menu.addAction("旋转");
    QAction *textAction = menu.addAction("文本");

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



void QGraphicsImageProxyItemWrap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    setCursor(Qt::ClosedHandCursor);
}



void QGraphicsImageProxyItemWrap::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    setToolTip("test");
}

void QGraphicsImageProxyItemWrap::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
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
