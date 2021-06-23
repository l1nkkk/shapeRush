#include "drawview.h"
#include <QDebug>
#include <QMenu>
DrawView::DrawView(QWidget *parent)
    : QGraphicsView(parent)
{

}
void DrawView::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMove(event);
    QGraphicsView::mouseMoveEvent(event);
}

void DrawView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        auto scenePos = mapToScene(event->pos());
        auto item = scene()->itemAt(scenePos,QTransform());
        if(!item)
            emit mouseRightClick(event);
    }
    else
        emit mouseLeftClick(event);
    QGraphicsView::mousePressEvent(event);
}

void DrawView::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseRelease(event);
    QGraphicsView::mouseReleaseEvent(event);
}

void DrawView::keyPressEvent(QKeyEvent *event)
{
    emit keyPress(event);
    QGraphicsView::keyPressEvent(event);
}

void DrawView::keyReleaseEvent(QKeyEvent *event)
{
    emit keyRelease(event);
    QGraphicsView::keyReleaseEvent(event);
}

