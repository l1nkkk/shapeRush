#ifndef DRAWVIEW_H
#define DRAWVIEW_H
#include<QGraphicsView>
#include<QMouseEvent>

class DrawView : public QGraphicsView
{
    Q_OBJECT

public:
    DrawView(QWidget *parent = nullptr);

protected:

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;


signals:
    void mouseMove(QMouseEvent *event);
    void mouseLeftClick(QMouseEvent *event);
    void mouseRightClick(QMouseEvent *event);
    void mouseRelease(QMouseEvent *event);
    void keyPress(QKeyEvent *event);
    void keyRelease(QKeyEvent *event);
};
#endif // DRAWVIEW_H
