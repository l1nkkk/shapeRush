#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QVector>
#include <functional>
#include "gof/observer.h"
#include"drawview.h"
#include"shape/shapeImport.h"
#include "gof/iterator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    // enum 选择创建的类型
    enum CreateType{
        NONE = 0,
        LINE,
        ELLIPSE,
        RECT,
        BUTTON,
        COMPOSITE,
        CLONE,
        SPIN,
        TEXT,
        IMAGE
    };
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 槽
public slots:
    void onViewMouseMove(QMouseEvent *event);       // 鼠标移动，坐标显示变化
    void onViewMouseClick(QMouseEvent *event);      // 左键
    void onViewMouseRClick(QMouseEvent *event);     // 右键
    void onViewMouseRealse(QMouseEvent *event);     // 松开鼠标
    void onItemShapeCopy(Shape *cpShape);           // 收到拷贝信号时
    void onViewKeyPress(QKeyEvent* event);          // 按下键盘
    void onViewKeyRelease(QKeyEvent* event);        // 松开键盘
    void onItemComposite(bool flag);                // 组合事件发生，创建组合对象
    void onItemCancelComposite(Composite* cps);
    void onItemDelete(Shape* sp);
    void onItemSpinDeractor(Shape* sp);
    void onItemTextDeractor(Shape* sp);


    // ui func
private:
    void createAction();                                  // 创建菜单栏和工具栏
    void uiInit();                                        // UI界面初始化

    void addSelect(Shape *s);       // 添加选中
    void removeSelect(Shape *s);    // 移除选中
    void clearSelect();             // 清空选中
    void deleteItem(Shape* sp);     // 删除图元
    void initCreateFuncs();         // 初始化构造图形函数
    void updateCmdTree();           // 更新命令历史
    void createShapeItem(CreateType type, QPointF pointScene = QPointF(0,0));
    void connectSignal(Shape* sp, bool isClone = false);
    void notify(Shape *shape, Observer::Aspect aspect);
    void reduce(QQueue<YAML::ShapeData> &buildQueue, QMap<QString, YAML::ShapeData> &buildMap);

private:
    // ui
    Ui::MainWindow *ui;
    QGraphicsScene *m_scene = nullptr;      // 场景
    QHBoxLayout *m_hBoxLayout = nullptr;    // 布局
    QVBoxLayout *m_vBoxLayout = nullptr;    // 布局
    DrawView *m_view = nullptr;             // 视图
    QTableWidget *m_cmdTable = nullptr;        // 文本编辑部件
    TreeWidgetEx *m_twe;

    // 状态
    bool m_isExFactor = false;                          // 是否使用高级版标志
    bool m_isMultiSelecting = false;                    // 是否多选状态，按ctrl控制
    bool m_isReOrder = false;
    CreateType m_selectCreateType = CreateType::NONE;   // 创建的对象类型
    Shape* m_creatingShape = nullptr;                   // 已经被创建的对象
    Shape* m_cloneShape = nullptr;                      // 已经被复制的对象
    QList<Observer*> m_obs;
    QList<Shape*> m_selectShapeList;                                    // 目前视图中标所选中的图形
    QMap<QString, Shape*> m_existShape;
    QMap<CreateType, std::function<void(QPointF)> > m_createFunction;   // 创建图形的函数集
    CommandHistory* m_cmdHis;
};
#endif // MAINWINDOW_H
