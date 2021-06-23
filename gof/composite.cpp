#include "composite.h"
#include <QMenu>

void QGraphicsItemGroupWrap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    setCursor(Qt::ClosedHandCursor);
}



void QGraphicsItemGroupWrap::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    setToolTip("test");
}

void QGraphicsItemGroupWrap::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *moveAction = menu.addAction("移动");
    QAction *cloneAction = menu.addAction("clone");
    QAction *compositeAction = menu.addAction("组合");
    QAction *cancelCompositeAction = menu.addAction("取消组合");
    QAction *deleteAction = menu.addAction("删除");
    QAction *spinAction = menu.addAction("旋转");

    QAction *selectedAction = menu.exec(event->screenPos());

    if(selectedAction == moveAction){
        setPos(0,0);
    } else if(selectedAction == cloneAction){
        emit shapeCopy(this->m_base);
    } else if(selectedAction == compositeAction){
        emit compositeEvent(true);
    } else if(selectedAction == deleteAction){
        emit deleteEvent(this->m_base);
    } else if(selectedAction == cancelCompositeAction){
        emit cancelCompositeEvent(this->m_base->getComposite());
    } else if(selectedAction == spinAction){
        emit spinDeractor(this->m_base);
    }
}

