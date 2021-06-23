#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "drawview.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QToolBar>
#include <QtGlobal>
#include <QTime>
#include <QSet>
#include <QTransform>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication>
#include <QFileDialog>
#include <QHeaderView>
#include <memory>
#include "gof/singletom.h"
#include "gof/factory.h"
#include "gof/adaptershape.h"
#include "gof/composite.h"
#include "gof/decorator.h"
#include "storage/serialization.h"
#include "gof/command.h"
#include "gof/strategy.h"



void MainWindow::createShapeItem(MainWindow::CreateType type, QPointF pointScene)
{
    if(type != CreateType::NONE ) {
        // a.创建Item对象，创建后的对象指针存在m_creatingShape中
        m_createFunction[type](pointScene);

        // b.更新图元信息树，m_treeWidget 和 m_treeWidgetItems
        if(m_creatingShape == nullptr)
            return;

        if(m_creatingShape->getComposite() == nullptr
                && m_creatingShape->getDecorator() == nullptr){
            notify(m_creatingShape, Observer::Aspect::CreateShapeItem);

        }else if(m_creatingShape->getDecorator() != nullptr){
            notify(m_creatingShape, Observer::Aspect::CreateShapeDecorator);
        }
        else{
            Q_ASSERT(type == CreateType::CLONE ||
                     type == CreateType::COMPOSITE );
            if(type == CreateType::CLONE)
                notify(m_creatingShape, Observer::Aspect::CloneShapeComposite);
            else
                notify(m_creatingShape, Observer::Aspect::CreateShapeComposite);
        }


        notify(nullptr, Observer::Aspect::ClearSelect);
        notify(m_creatingShape, Observer::Aspect::SelectItem);

        // c.连接信号
        connectSignal(m_creatingShape, type == CreateType::CLONE);
    }

    m_creatingShape = nullptr;
}


void MainWindow::reduce(QQueue<YAML::ShapeData> &buildQueue, QMap<QString, YAML::ShapeData> &buildMap)
{

    QMap<QString,QList<QString>> parToChi;
    while(!buildQueue.empty() ){
        // 构建
        auto data = buildQueue.front();
        buildQueue.pop_front();
        auto rtn = data.toShape(buildQueue, buildMap, m_existShape, parToChi);
        m_existShape[rtn->getName()] = rtn;
        if(rtn->getComposite() != nullptr)
            notify(rtn, Observer::CreateShapeComposite);
        else if(rtn->getDecorator() != nullptr)
            notify(rtn, Observer::CreateShapeDecorator);
        else
            notify(rtn, Observer::CreateShapeItem);
        rtn->getItem()->setPos(rtn->getStart());
        connectSignal(rtn, false);

    }
}

void MainWindow::connectSignal(Shape *sp, bool isClone)
{

    QObject::connect(sp->getInter(), &ShapeInterface::shapeCopy, this, &MainWindow::onItemShapeCopy);
    QObject::connect(sp->getInter(), &ShapeInterface::compositeEvent, this, &MainWindow::onItemComposite);
    QObject::connect(sp->getInter(), &ShapeInterface::deleteEvent, this, &MainWindow::onItemDelete);
    QObject::connect(sp->getInter(), &ShapeInterface::spinDeractor, this, &MainWindow::onItemSpinDeractor);
    QObject::connect(sp->getInter(), &ShapeInterface::textDeractor, this, &MainWindow::onItemTextDeractor);
    if(sp->getComposite() != nullptr) {

        QObject::connect(sp->getInter(), &ShapeInterface::cancelCompositeEvent, this, &MainWindow::onItemCancelComposite);
        if(isClone)
            for(auto it : sp->getComposite()->m_leaf){
                connectSignal(it);
            }
    }

}

// 添加选中
void MainWindow::addSelect(Shape *s)
{
    notify(nullptr, Observer::Aspect::SelectItem);
    notify(nullptr, Observer::Aspect::UpdateScene);
}

// 删除选中
void MainWindow::removeSelect(Shape *s)
{
    notify(nullptr, Observer::Aspect::RemoveSelect);
    notify(nullptr, Observer::Aspect::UpdateScene);
}

// 取消所有选中
void MainWindow::clearSelect()
{
    notify(nullptr, Observer::Aspect::ClearSelect);
    notify(nullptr, Observer::Aspect::UpdateScene);
}


// 删除图元
void MainWindow::deleteItem(Shape *sp)
{
    // 递归删除m_treeWidgetItems
//    deleteTreeWidgetItems(sp);
    notify(sp, Observer::Aspect::DeleteShapeItem);
    delete sp;
    // remove select
    notify(nullptr, Observer::Aspect::ClearSelect);
    notify(nullptr, Observer::Aspect::UpdateScene);
}

// ---------------观察者模式

void MainWindow::notify(Shape *shape, Observer::Aspect aspect){
    foreach(auto it, m_obs){
        if((*it).getMask() & aspect)
            it->update(shape, aspect);
    }
}


// ---------------信号处理


void MainWindow::onViewMouseMove(QMouseEvent *event)
{
    auto point = event->pos();
    auto pointScene = m_view->mapToScene(point);
    QString msg;
    QGraphicsItem* item = m_view->scene()->itemAt(pointScene,QTransform());
    if(item)
        msg = QString::asprintf("View: (%d, %d); Scene: (%.0f, %.0f); Item: (%.0f, %.0f)",\
                                point.x(), point.y(), pointScene.x(), pointScene.y(),\
                                item->mapFromScene(pointScene).x(), item->mapFromScene(pointScene).y());
    else
        msg = QString::asprintf("View: %d, %d  Scene: %.0f, %.0f", point.x(), point.y(), pointScene.x(), pointScene.y());
    statusBar()->showMessage(msg);
}

void MainWindow::onItemShapeCopy(Shape *cpShape)
{
     m_cloneShape = cpShape;
}



void MainWindow::onViewKeyPress(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
    {
        m_isMultiSelecting = true;
    }
}

void MainWindow::onViewKeyRelease(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
    {
        m_isMultiSelecting = false;
    }
}



// ---------------初始化创建对象函数

void MainWindow::initCreateFuncs()
{
    // 线段
    m_createFunction[CreateType::LINE] = [&](QPointF pointScene){
        if(m_isExFactor)
            m_creatingShape = Singletom<ExFactoryShape>::getInstance()\
                ->CreateLine(pointScene, QPointF(pointScene.x() \
                                                 +(qrand()%200),pointScene.y()+(qrand()%200)));
        else
            m_creatingShape = Singletom<FactoryShape>::getInstance()\
                ->CreateLine(pointScene, QPointF(pointScene.x() \
                                                 +(qrand()%200),pointScene.y()+(qrand()%200)));
        m_creatingShape->getItem()->setPos(pointScene);
    };

    //矩形
    m_createFunction[CreateType::RECT] = [&](QPointF pointScene){
        if(m_isExFactor)
            m_creatingShape = Singletom<ExFactoryShape>::getInstance()\
                    ->CreateRect(pointScene, QPointF(pointScene.x() \
                                                     +(qrand()%200),pointScene.y()+(qrand()%200)));
        else
            m_creatingShape = Singletom<FactoryShape>::getInstance()\
                ->CreateRect(pointScene, QPointF(pointScene.x() \
                                                 +(qrand()%200),pointScene.y()+(qrand()%200)));

        m_creatingShape->getItem()->setPos(pointScene);
    };

    // 椭圆
    m_createFunction[CreateType::ELLIPSE] = [&](QPointF pointScene){
        if(m_isExFactor)
            m_creatingShape = Singletom<ExFactoryShape>::getInstance()\
                    ->CreateElli(pointScene, QPointF(pointScene.x() \
                                                     +(qrand()%200),pointScene.y()+(qrand()%200)));
        else
            m_creatingShape = Singletom<FactoryShape>::getInstance()\
                    ->CreateElli(pointScene, QPointF(pointScene.x() \
                                                     +(qrand()%200),pointScene.y()+(qrand()%200)));

        m_creatingShape->getItem()->setPos(pointScene);
    };


    // 按钮
    m_createFunction[CreateType::BUTTON] = [&](QPointF pointScene){
        m_creatingShape = Singletom<FactoryShape>::getInstance()\
                ->CreateButton(pointScene, QPointF(pointScene.x() \
                                                 + 100 +(qrand()%200),pointScene.y()+ 30 + (qrand()%200)));

    };

    // CLONE
    m_createFunction[CreateType::CLONE] = [&](QPointF pointScene){
        if(m_cloneShape == nullptr)
            return;
        m_creatingShape = m_cloneShape->Clone();
        m_creatingShape->getItem()->setPos(pointScene);
    };

    // COMPOSITE
    m_createFunction[CreateType::COMPOSITE] = [&](QPointF pointScene){
        // a.创建组合对象
        auto compo = new Composite;
        m_creatingShape = compo;
        for(auto &it : m_selectShapeList){
            // b.删
            notify(it, Observer::RemoveFromScene);
            // c 添加
            it->setParentName(compo->getName());
            compo->addChild(it);
        }
        notify(nullptr, Observer::ClearSelect);
        //m_creatingShape->getItem()->setPos(pointScene);

    };

    // SPIN
    m_createFunction[CreateType::SPIN] = [&](QPointF pointScene){
        // 删原来的信息
        notify(m_creatingShape, Observer::RemoveFromScene);

        auto spinItem = new SpinItem(m_creatingShape);
        m_creatingShape->setParentName(spinItem->getName());

        spinItem->getItem()->setPos(m_creatingShape->getItem()->pos());
        m_creatingShape->getItem()->setPos(0,0);
        m_creatingShape->getItem()->setFlag(QGraphicsItem::ItemIsMovable,false); // 不可拖动
        m_creatingShape = spinItem;

    };

    // TEXT
    m_createFunction[CreateType::TEXT] = [&](QPointF pointScene){
        // 删原来的信息
        notify(m_creatingShape, Observer::RemoveFromScene);

        auto textItem = new TextItem(m_creatingShape);
        m_creatingShape->setParentName(textItem->getName());

        textItem->getItem()->setPos(m_creatingShape->getItem()->pos());
        m_creatingShape->getItem()->setPos(0,0);
        m_creatingShape->getItem()->setFlag(QGraphicsItem::ItemIsMovable,false); // 可拖动
        m_creatingShape = textItem;

    };

    // Image
    m_createFunction[CreateType::IMAGE] = [&](QPointF pointScene){

        m_creatingShape = Singletom<FactoryShape>::getInstance()
            ->CreateImage(pointScene, QPointF(pointScene.x()
                                             +400, pointScene.y()+300));
        m_creatingShape->getItem()->setPos(pointScene);
    };
}



// --------------------ui操作

void MainWindow::updateCmdTree(){
    std::shared_ptr<LIterator<Command> > itp;

    if(m_isReOrder == false)
        itp = m_cmdHis->createCmdIterFromFirst();
    else
        itp = m_cmdHis->createReCmdIterFromFirst();

    m_cmdTable->clearContents();
    m_cmdTable->setRowCount(m_cmdHis->size()+1);
    m_cmdTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_cmdTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    int i = 0;
    for(;!itp->IsDone(); itp->Next(),++i){
        qDebug() << itp->Current()->getName();
        m_cmdTable->setItem(i,0, new QTableWidgetItem(itp->Current()->getName()));
        m_cmdTable->setItem(i,1, new QTableWidgetItem(itp->Current()->getDate()));
    }

}

void MainWindow::uiInit()
{
    createAction();

    setFixedSize(1280, 720);
    // layout initializaton
    m_hBoxLayout= new QHBoxLayout(this);
    m_vBoxLayout = new QVBoxLayout(this);


    // textedit initialiazation
    m_cmdTable = new QTableWidget(this);
    m_cmdTable->setWindowTitle("Command history");
    m_cmdTable->setColumnCount(2);
    QStringList header;
    header << "Command" << "Time";
    m_cmdTable->setHorizontalHeaderLabels(header);

    // treeWidget initialization
    m_twe->treeWidget = new QTreeWidget(this);
    QStringList header1;
    header1 << "Property" << "Value";
    m_twe->treeWidget->setHeaderLabels(header1);


    // view initialization
    m_view = new DrawView(this);
    m_view->setDragMode(QGraphicsView::NoDrag);
    m_view->setCursor(Qt::ArrowCursor);
    m_view->setMouseTracking(true);
    m_view->show();

    // scene initialization
    m_scene = new QGraphicsScene(m_view->rect());
    m_view->setScene(m_scene);

    // 布局
    m_vBoxLayout->addWidget(m_twe->treeWidget);
    m_vBoxLayout->addWidget(m_cmdTable);
    m_vBoxLayout->setStretch(0,3);
    m_vBoxLayout->setStretch(3,4);
    m_hBoxLayout->addItem(m_vBoxLayout);
    m_hBoxLayout->addWidget(m_view);
    m_hBoxLayout->setStretch(0, 1);
    m_hBoxLayout->setStretch(1, 3);

    QWidget *placeholderWidget = new QWidget;
    placeholderWidget->setLayout(m_hBoxLayout);
    setCentralWidget(placeholderWidget);

    // 状态栏
    statusBar()->showMessage("Write by l1nkkk");
}


// 创建菜单栏和工具栏
void MainWindow::createAction()
{
    // 添加菜单和工具栏
    QMenu *menu = menuBar()->addMenu(tr("工具"));
    QToolBar *toolBar = addToolBar("工具");

    QAction *createCircleAction = new QAction("椭圆");
    connect(createCircleAction, &QAction::triggered, this,[&](){
        m_view->setCursor(Qt::CrossCursor);
        m_selectCreateType = CreateType::ELLIPSE;
    });
    menu->addAction(createCircleAction);
    toolBar->addAction(createCircleAction);

    QAction *createRectAction = new QAction("长方形");
    connect(createRectAction, &QAction::triggered, this,[&](){
        m_view->setCursor(Qt::CrossCursor);
        m_selectCreateType = CreateType::RECT;
    });
    menu->addAction(createRectAction);
    toolBar->addAction(createRectAction);

    QAction *createLineAction = new QAction("线");
    connect(createLineAction, &QAction::triggered, this,[&](){
        m_view->setCursor(Qt::CrossCursor);
        m_selectCreateType = CreateType::LINE;
    });
    menu->addAction(createLineAction);
    toolBar->addAction(createLineAction);

    // 设配器
    QAction *createButtonAction = new QAction("按钮");
    connect(createButtonAction, &QAction::triggered, this,[&](){
        m_view->setCursor(Qt::CrossCursor);
        m_selectCreateType = CreateType::BUTTON;
    });
    menu->addAction(createButtonAction);
    toolBar->addAction(createButtonAction);

    // 抽象工厂
    toolBar->addSeparator();
    menu->addSeparator();
    QAction * triggerExFactorAction =new QAction("高级版");
    triggerExFactorAction->setCheckable(true);
    connect(triggerExFactorAction, &QAction::triggered, this,[&](){
        if(m_isExFactor == true) m_isExFactor = false;
            else m_isExFactor = true;
    });
    menu->addAction(triggerExFactorAction);
    toolBar->addAction(triggerExFactorAction);


    // 序列化 持久化
    toolBar->addSeparator();
    menu->addSeparator();

    QAction * clearButtonAction =new QAction("清除");
    connect(clearButtonAction, &QAction::triggered, this,[&](){
        notify(nullptr, Observer::Aspect::ClearAll);
        m_cmdTable->clearContents();
        m_cmdHis->clear();
    });
    menu->addAction(clearButtonAction);
    toolBar->addAction(clearButtonAction);

    QAction * loadAction =new QAction("导入");
    connect(loadAction, &QAction::triggered, this,[&](){
        QFileDialog *qfd = new QFileDialog(this);
        if(!qfd->exec()) return;
        QString path;
        if (!qfd->selectedFiles().empty())
        {
            path = qfd->selectedFiles()[0];
        }
        QMap<QString, YAML::ShapeData> buildMap;
        auto buildQueue = YAML::importFromFile(path, buildMap);
        reduce(buildQueue, buildMap);
    });
    menu->addAction(loadAction);
    toolBar->addAction(loadAction);

    QAction * unloadAction =new QAction("导出");
    connect(unloadAction, &QAction::triggered, this,[&](){
        QFileDialog *qfd = new QFileDialog(this);
        if(!qfd->exec()) return;
        QString path;
        if (!qfd->selectedFiles().empty())
        {
            path = qfd->selectedFiles()[0];
        }
        YAML::exportToFile(path, &m_existShape);

    });
    menu->addAction(unloadAction);
    toolBar->addAction(unloadAction);

    // 命令模式
    toolBar->addSeparator();
    menu->addSeparator();

    // 上
    QAction * upAction =new QAction("↑");
    connect(upAction, &QAction::triggered, this,[&](){
        if(m_selectShapeList.size() != 1) return;
        Command* cmd = new MoveCommand(m_selectShapeList[0], 0,-30);
        cmd->Execute();
        notify(m_selectShapeList[0], Observer::Aspect::UpdateShapePos);

        m_cmdHis->add(cmd);
        updateCmdTree();
    });
    menu->addAction(upAction);
    toolBar->addAction(upAction);
    // 下
    QAction * downAction =new QAction("↓");
    connect(downAction, &QAction::triggered, this,[&](){
        if(m_selectShapeList.size() != 1) return;
        Command* cmd = new MoveCommand(m_selectShapeList[0], 0,30);
        cmd->Execute();
        notify(m_selectShapeList[0], Observer::Aspect::UpdateShapePos);

        m_cmdHis->add(cmd);
        updateCmdTree();
    });
    menu->addAction(downAction);
    toolBar->addAction(downAction);

    QAction * leftAction =new QAction("←");
    connect(leftAction, &QAction::triggered, this,[&](){
        if(m_selectShapeList.size() != 1) return;
        Command* cmd = new MoveCommand(m_selectShapeList[0], -30, 0);
        cmd->Execute();
        notify(m_selectShapeList[0], Observer::Aspect::UpdateShapePos);

        m_cmdHis->add(cmd);
        updateCmdTree();
    });
    menu->addAction(leftAction);
    toolBar->addAction(leftAction);

    QAction * rightAction =new QAction("→");
    connect(rightAction, &QAction::triggered, this,[&](){
        if(m_selectShapeList.size() != 1) return;
        Command* cmd = new MoveCommand(m_selectShapeList[0], 30, 0);
        cmd->Execute();
        notify(m_selectShapeList[0], Observer::Aspect::UpdateShapePos);

        m_cmdHis->add(cmd);
        updateCmdTree();
    });
    menu->addAction(rightAction);
    toolBar->addAction(rightAction);

    // 迭代器模式
    toolBar->addSeparator();
    menu->addSeparator();
    QAction * reOrderAction =new QAction("ReOrder");
    reOrderAction->setCheckable(true);
    connect(reOrderAction, &QAction::triggered, this,[&](){
        if(m_isReOrder == true) m_isReOrder = false;
            else m_isReOrder = true;
        updateCmdTree();
    });
    menu->addAction(reOrderAction);
    toolBar->addAction(reOrderAction);

    // undo
    QAction * undoAction =new QAction("undo");
    connect(undoAction, &QAction::triggered, this,[&](){
        if(m_cmdHis->size() == -1) return;

        auto itp = m_cmdHis->createReCmdIterFromCur();
        if(!itp->IsDone()){
            itp->Current()->undo();
        }
        m_cmdHis->decCur();
        notify(m_selectShapeList[0], Observer::Aspect::UpdateShapePos);
        updateCmdTree();
    });
    menu->addAction(undoAction);
    toolBar->addAction(undoAction);

    // redo
    QAction * redoAction =new QAction("redo");
    connect(redoAction, &QAction::triggered, this,[&](){
        if(!m_cmdHis->redoAble())
            return;
        auto itp = m_cmdHis->createCmdIterFromCur();
        itp->Next();
        if(!itp->IsDone()){
            itp->Current()->Execute();
        }
        m_cmdHis->addCur();
        notify(m_selectShapeList[0], Observer::Aspect::UpdateShapePos);
        updateCmdTree();
    });
    menu->addAction(redoAction);
    toolBar->addAction(redoAction);

    //--------代理模式
    toolBar->addSeparator();
    menu->addSeparator();
    QAction * proxyAction =new QAction("皮卡丘");
    connect(proxyAction, &QAction::triggered, this,[&](){
        m_view->setCursor(Qt::CrossCursor);
        m_selectCreateType = CreateType::IMAGE;
    });
    menu->addAction(proxyAction);
    toolBar->addAction(proxyAction);

    // --------策略模式
    // 上
    toolBar->addSeparator();
    menu->addSeparator();
    QAction * upStrategyAction =new QAction("上对齐");
    connect(upStrategyAction, &QAction::triggered, this,[&](){
        auto strategy = new UpStrategy();
        strategy->align(m_selectShapeList);
        foreach(auto it, m_selectShapeList){
            notify(it,Observer::Aspect::UpdateScenePos);
            notify(it, Observer::Aspect::UpdateShapePos);
        }
    });
    menu->addAction(upStrategyAction);
    toolBar->addAction(upStrategyAction);
    // 下
    QAction * downStrategyAction =new QAction("下对齐");
    connect(downStrategyAction, &QAction::triggered, this,[&](){
        auto strategy = new DownStrategy();
        strategy->align(m_selectShapeList);
        foreach(auto it, m_selectShapeList){
            notify(it,Observer::Aspect::UpdateScenePos);
            notify(it, Observer::Aspect::UpdateShapePos);
        }
    });
    menu->addAction(downStrategyAction);
    toolBar->addAction(downStrategyAction);
    // 左
    QAction * leftStrategyAction =new QAction("左对齐");
    connect(leftStrategyAction, &QAction::triggered, this,[&](){
        auto strategy = new LeftStrategy();
        strategy->align(m_selectShapeList);
        foreach(auto it, m_selectShapeList){
            notify(it,Observer::Aspect::UpdateScenePos);
            notify(it, Observer::Aspect::UpdateShapePos);
        }
    });
    menu->addAction(leftStrategyAction);
    toolBar->addAction(leftStrategyAction);
    // 右
    QAction * rightStrategyAction =new QAction("右对齐");
    connect(rightStrategyAction, &QAction::triggered, this,[&](){
        auto strategy = new RightStrategy();
        strategy->align(m_selectShapeList);
        foreach(auto it, m_selectShapeList){
            notify(it,Observer::Aspect::UpdateScenePos);
            notify(it, Observer::Aspect::UpdateShapePos);
        }
    });
    menu->addAction(rightStrategyAction);
    toolBar->addAction(rightStrategyAction);
}




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    initCreateFuncs();

    m_twe = new TreeWidgetEx;
    m_cmdHis = CommandHistory::Instance();
    uiInit();

    // Observer初始化
    m_obs.push_back(new TreeWidgetObserver(m_twe));
    m_obs.push_back(new SelectListObserver(&m_selectShapeList));
    m_obs.push_back(new SceneObserver(m_scene));
    m_obs.push_back(new ExistShapeObserver(&m_existShape));

    // connect 信号与槽
    QObject::connect(m_view,SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(onViewMouseMove(QMouseEvent*)));
    QObject::connect(m_view,SIGNAL(mouseLeftClick(QMouseEvent*)), this, SLOT(onViewMouseClick(QMouseEvent*)));
    QObject::connect(m_view, &DrawView::mouseRightClick, this, &MainWindow::onViewMouseRClick );
    QObject::connect(m_view, &DrawView::mouseRelease, this, &MainWindow::onViewMouseRealse);
    QObject::connect(m_view, &DrawView::keyPress, this, &MainWindow::onViewKeyPress);
    QObject::connect(m_view, &DrawView::keyRelease, this, &MainWindow::onViewKeyRelease);

}

MainWindow::~MainWindow()
{
    delete ui;
}
