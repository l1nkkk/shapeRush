#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "drawview.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QToolBar>
#include <QtGlobal>
#include <QTime>
#include <QTransform>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication>
#include <QQueue>
#include <memory>
#include "gof/singletom.h"
#include "gof/factory.h"
#include "gof/adaptershape.h"
#include "gof/composite.h"
#include "gof/decorator.h"



void MainWindow::onViewMouseClick(QMouseEvent *event)
{
    auto pos = event->pos();
    auto pointScene = m_view->mapToScene(pos);
    if(m_selectCreateType != CreateType::NONE){
        // 创建图元
        createShapeItem(m_selectCreateType, pointScene);
    }else{
        // 选择图元
        if(m_view->itemAt(pos)){
            // 获取该位置下图元
            auto item = m_view->itemAt(pos);

            // 选中父亲图元
            while(item->parentItem() != nullptr){
                item = item->parentItem();
            }


            // 通过Item获取到shape
            auto shape = static_cast<Shape*>(item->data(0).value<void *>());

            // 如果是代理的图片
            if(shape->getProxy() != nullptr){
                auto subject = shape->getProxy()->getSubject();
                subject->getItem()->setPos(shape->getStart());
                // 1.创造
                notify(subject, Observer::CreateShapeItem);
                // 2.连接信号
                connectSignal(subject, false);
                // 3.发送删除代理
                notify(shape, Observer::Aspect::DeleteShapeItem);

                // 4.shape设置为实体
                shape = subject;
            }


            // mutiselect?，是否按住ctrl
            if(!m_isMultiSelecting)
                notify(nullptr, Observer::Aspect::ClearSelect); // 清除当前所选

            if(std::find(m_selectShapeList.begin(),m_selectShapeList.end(), shape) == m_selectShapeList.end())
                notify(shape, Observer::Aspect::SelectItem);
            else
                notify(shape, Observer::Aspect::RemoveSelect);
        } else{
            // 清空所有选中
            notify(nullptr, Observer::Aspect::ClearSelect);
        }
    }


//    m_isCreating = true;
    // 恢复环境
    m_selectCreateType = CreateType::NONE;
    m_creatingShape = nullptr;
    notify(nullptr, Observer::Aspect::UpdateScene);
    m_view->setCursor(Qt::ArrowCursor);

}

void MainWindow::onViewMouseRClick(QMouseEvent *event)
{
    auto pos = event->pos();
    auto pointScene = m_view->mapToScene(pos);
    qDebug() << pointScene;
    QMenu menu;
    QAction *pasteAction = menu.addAction("粘贴");
    QAction *selectedAction = menu.exec(event->screenPos().toPoint());

    if(selectedAction == pasteAction)
        createShapeItem(CreateType::CLONE, pointScene);
}

void MainWindow::onViewMouseRealse(QMouseEvent *event)
{
//    if(event->buttons() == Qt::LeftButton){
        foreach(auto it, m_selectShapeList){
            notify(it,Observer::Aspect::UpdateShapePos);
            notify(it,Observer::Aspect::UpdateTreeItem);
        }
//    }
}

//
void MainWindow::onItemCancelComposite(Composite* cmp){
    notify(cmp, Observer::Aspect::CancelShapeComposite);
    delete cmp;
}


void MainWindow::onItemComposite(bool flag)
{
    createShapeItem(CreateType::COMPOSITE);
}


void MainWindow::onItemDelete(Shape *sp)
{
    foreach (auto it, m_selectShapeList) {
        deleteItem(it);
    }
    clearSelect();
}

void MainWindow::onItemSpinDeractor(Shape *sp)
{
    m_creatingShape = sp;
    createShapeItem(CreateType::SPIN, sp->getStart());
}

void MainWindow::onItemTextDeractor(Shape *sp)
{
    m_creatingShape = sp;
    createShapeItem(CreateType::TEXT, sp->getStart());
}


