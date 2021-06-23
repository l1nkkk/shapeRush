#ifndef OBSERVER_H
#define OBSERVER_H
#include <QTreeWidget>
#include <QGraphicsScene>
#include <QDebug>
#include "../shape/shapeImport.h"

#include "decorator.h"
#include "composite.h"
#include "iterator.h"

struct TreeWidgetEx{
    QTreeWidget *treeWidget = nullptr;                // 树形部件
    QMap<QString, QTreeWidgetItem*> treeWidgetItems;  // 目前所有的WidgetItems,<ShapeName, QTreeWidgetItem>
};


class Observer
{
public:
    enum Aspect : uint64_t{
        NONE = 0x0000,
        ALL = 0Xffff,
        CreateShapeItem = 0x0001,
        CreateShapeDecorator = 0x0002,
        CreateShapeComposite = 0x0004,
        CloneShapeComposite = 0x0008,
        DeleteShapeItem = 0x0010,
        SelectItem = 0x0020,
        RemoveSelect = 0x0040,
        ClearSelect = 0x0080,
        RemoveFromScene = 0x0100,
        UpdateScene = 0x0200,
        CancelShapeComposite = 0x0800,
        UpdateShapePos = 0x1000,
        UpdateTreeItem = 0x2000,
        ClearAll = 0x4000,
        UpdateScenePos = 0x8000
    };
public:
    Observer(){}

    virtual void update(Shape*sp, Aspect aspect) = 0;
    uint64_t getMask() { return mask; }
    protected:
    uint64_t mask = 0;
};

class TreeWidgetObserver : public Observer{
public:
    TreeWidgetObserver(TreeWidgetEx *twe) : Observer(){
        m_twe = twe;
        mask = ClearSelect | CreateShapeItem | CreateShapeDecorator
                | CreateShapeComposite | CloneShapeComposite |DeleteShapeItem
                | CancelShapeComposite | SelectItem | RemoveSelect
                | UpdateTreeItem | ClearAll;
    }
    void update(Shape* sp, Aspect aspect) override{
        switch (aspect) {
        case Aspect::ClearSelect :
            doClearSelect();
            break;
        case Aspect::CreateShapeItem :
            m_twe->treeWidget->addTopLevelItem
                    (doCreateShapeItem(sp));
            break;
        case Aspect::CreateShapeDecorator :
            m_twe->treeWidget->addTopLevelItem
                    (doCreateShapeDecorator(sp));
            break;
        case Aspect::CreateShapeComposite :
            m_twe->treeWidget->addTopLevelItem
                    (doCreateShapeComposite(sp));
            break;
        case Aspect::CloneShapeComposite:
             m_twe->treeWidget->addTopLevelItem
                     (doCloneShapeComposite(sp));
            break;
        case Aspect::DeleteShapeItem :
            doDeleteShapeItem(sp);
            break;
        case Aspect::CancelShapeComposite :
            doCancelShapeComposite(sp);
            break;
        case Aspect::SelectItem:
            doSelectItem(sp);
            break;
        case Aspect::RemoveSelect:
            doRemoveSelect(sp);
            break;
        case Aspect::UpdateTreeItem:
            doUpdateTreeItem(sp);
            break;
        case Aspect::ClearAll:
            doClearAll();
            break;
        default :
           return;
        }
    };
private:
    QTreeWidgetItem* doCreateShapeItem(Shape *shape);
    QTreeWidgetItem* doCreateShapeDecorator(Shape *shape);
    QTreeWidgetItem* doCreateShapeComposite(Shape *shape);
    QTreeWidgetItem* doCloneShapeComposite(Shape *shape);
    void doDeleteShapeItem(Shape *shape);
    void doCancelShapeComposite(Shape *shape);
    void doSelectItem(Shape *shape);
    void doRemoveSelect(Shape *shape);
    void doClearSelect();
    void doUpdateTreeItem(Shape *);
    void doClearAll();
    void updateTreeItemMap(QTreeWidgetItem* item);
    void expendItem(QTreeWidgetItem* item);
private:
    TreeWidgetEx* m_twe;
};

class SelectListObserver : public Observer{
public:
    SelectListObserver(QList<Shape*>* ssl) : Observer(){
        m_ssl = ssl;
        mask = SelectItem | RemoveSelect | DeleteShapeItem
                | ClearSelect | ClearAll;
    }
    void update(Shape* sp, Aspect aspect) override{
        switch (aspect) {
        case Aspect::SelectItem:
            doSelectItem(sp);
            break;
        case Aspect::RemoveSelect:
        case Aspect::DeleteShapeItem:
            doRemoveSelect(sp);
            break;
        case Aspect::ClearSelect:
        case Aspect::ClearAll:
            doClearSelect();
            break;
        default:
            return;
        }
    }
private:
    void doSelectItem(Shape *shape) {
        shape->setSelect(true);
        m_ssl->push_back(shape);
    }
    void doRemoveSelect(Shape *shape) {
        shape->setSelect(false);
        m_ssl->removeAll(shape);
    }
    void doClearSelect() {
        for(auto it = m_ssl->begin(); it != m_ssl->end(); ++it)
            (*it)->setSelect(false);

        m_ssl->clear();
    }

private:
    QList<Shape*>* m_ssl;
};


class SceneObserver : public Observer{
public:
    SceneObserver(QGraphicsScene* sc) : Observer(){
        m_scene = sc;
        mask = CreateShapeItem | CreateShapeDecorator | CreateShapeComposite
                | CloneShapeComposite | RemoveFromScene | DeleteShapeItem
                | UpdateScene | CancelShapeComposite | ClearAll;
    }
    void update(Shape* shape, Aspect aspect) override{
        switch (aspect) {
        case Aspect::CreateShapeItem :
        case Aspect::CreateShapeDecorator :
        case Aspect::CreateShapeComposite :
        case Aspect::CloneShapeComposite :
            doCreateShapeItem(shape); break;
        case Aspect::RemoveFromScene :
        case Aspect::DeleteShapeItem :
            doRemoveFromScene(shape);   break;
        case Aspect::UpdateScene:
            m_scene->update();
            break;
        case Aspect::CancelShapeComposite :
            doCancelShapeComposite(shape);
            break;
        case Aspect::ClearAll:
            doClearAll();
            break;
        default:
            return;
        }
    }
private:
    void doCreateShapeItem(Shape *shape) {
        if( shape->getParentName() == "")
            m_scene->addItem(shape->getItem());
    }
    void doRemoveFromScene(Shape *shape) { m_scene->removeItem(shape->getItem()); }
    void doCancelShapeComposite(Shape *shape) {
        m_scene->removeItem(shape->getItem());
        for(auto& it : shape->getComposite()->m_leaf){
            m_scene->addItem(it->getItem());
            it->getItem()->setPos(it->getStart());
        }
    }
    void doClearAll() { m_scene->clear(); }
private:
    QGraphicsScene *m_scene = nullptr;
};

class ExistShapeObserver : public Observer{
public:
    ExistShapeObserver(QMap<QString, Shape*>* sc) : Observer(){
        m_ems = sc;
        mask = CreateShapeItem | CreateShapeDecorator | CreateShapeComposite
               | CloneShapeComposite | DeleteShapeItem | UpdateShapePos
               | ClearAll | UpdateScenePos;
    }
    void update(Shape* shape, Aspect aspect){
        switch (aspect) {
        case Observer::Aspect::CreateShapeItem :
        case Observer::Aspect::CreateShapeDecorator :
        case Observer::Aspect::CreateShapeComposite :
        case Observer::Aspect::CloneShapeComposite :
            doCreateItem(shape);
            break;
        case Observer::Aspect::DeleteShapeItem:
            doDeleteShapeItem(shape);
            break;
        case Observer::Aspect::UpdateShapePos :
            doUpdateShapePos(shape);
            break;
        case Observer::Aspect::ClearAll:
            (*m_ems).clear();
            break;
        case Observer::Aspect::UpdateScenePos:
            doUpdateScenePos(shape);
        default:
            return;
        }
    }
private:
    void doCreateItem(Shape* shape){
        if(shape->getComposite() != nullptr){
            foreach(auto it, shape->getComposite()->m_leaf){
                doCreateItem(it);
            }
        } else if(shape->getDecorator() != nullptr){
            (*m_ems)[shape->getDecorator()->getRef()->getName()] = shape->getDecorator()->getRef();
        }
        (*m_ems)[shape->getName()] = shape;
    }
    void doDeleteShapeItem(Shape* shape){
        if(shape->getComposite() != nullptr){
            foreach(auto it, shape->getComposite()->m_leaf){
                doDeleteShapeItem(it);
            }
        }else if(shape->getDecorator() != nullptr){
            doDeleteShapeItem(shape->getDecorator()->getRef());
        }
        m_ems->remove(shape->getName());
    }
    void doUpdateShapePos(Shape* shape){
        if (shape->getComposite() == nullptr
                && shape->getDecorator() == nullptr){
            shape->setStart(shape->getItem()->mapToScene(0,0));
//            qDebug() << shape->getItem()->mapToScene(0,0);
            shape->setEnd(QPointF(shape->getItem()->mapToScene(0,0).x() + shape->getWidth(),
                                  shape->getItem()->mapToScene(0,0).y() + shape->getHeight()));
        } else if (shape->getComposite() != nullptr){
            foreach(auto it, shape->getComposite()->m_leaf)
                doUpdateShapePos(it);

        } else if (shape->getDecorator() != nullptr){
            doUpdateShapePos(shape->getDecorator()->getRef());
        }
    }
    void doUpdateScenePos(Shape *shape){
        shape->getItem()->setPos(shape->getStart());
    }
private:
    QMap<QString, Shape*>* m_ems;
};



#endif // OBSERVER_H
