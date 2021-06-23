#include "observer.h"

QTreeWidgetItem* TreeWidgetObserver::doCreateShapeItem(Shape *shape)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    // Type:CreateShapeItem
    item->setText(0, "name");
    item->setText(1, shape->getName());
    m_twe->treeWidgetItems[shape->getName()] = item;

    auto subitemX = new QTreeWidgetItem(item);
    subitemX->setText(0, "start");
    subitemX->setText(1, "(" + QString::number(shape->getStart().x())
                      + "," + QString::number(shape->getStart().y()) + ")");

    auto subitemY = new QTreeWidgetItem(item);
    subitemY->setText(0, "end");
    subitemY->setText(1, "(" + QString::number(shape->getEnd().x())
                      + "," + QString::number(shape->getEnd().y()) + ")");

    auto subitemAddr = new QTreeWidgetItem(item);
    subitemAddr->setText(0, "addr");
    subitemAddr->setText(1, QString::number(qulonglong(shape->getItem()), 16));

    return item;
}

QTreeWidgetItem* TreeWidgetObserver::doCreateShapeDecorator(Shape *shape)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "name");
    item->setText(1, shape->getName());
    m_twe->treeWidgetItems[shape->getName()]= item;

    // 非clone
    if(m_twe->treeWidgetItems.find(shape->getDecorator()->getRef()->getName())
            != m_twe->treeWidgetItems.end()){
        QTreeWidgetItem *clone =  m_twe->treeWidgetItems[shape->getDecorator()->getRef()->getName()]->clone();
        delete m_twe->treeWidgetItems[shape->getDecorator()->getRef()->getName()];
        m_twe->treeWidgetItems[shape->getDecorator()->getRef()->getName()] = clone;
        item->addChild(clone);
    } else{
        auto it = shape->getDecorator()->getRef();
        if(it->getComposite() != nullptr)
            m_twe->treeWidgetItems[it->getName()] = doCloneShapeComposite(it);
        else if(it->getDecorator() != nullptr)
            m_twe->treeWidgetItems[it->getName()] = doCreateShapeDecorator(it);
        else
            m_twe->treeWidgetItems[it->getName()] = doCreateShapeItem(it);
       item->addChild(m_twe->treeWidgetItems[it->getName()]);
    }
    updateTreeItemMap(item);

    return item;
}

QTreeWidgetItem* TreeWidgetObserver::doCreateShapeComposite(Shape *shape)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "name");
    item->setText(1, shape->getName());
    m_twe->treeWidgetItems[shape->getName()]= item;


    // 加入所有孩子
    for(auto it : shape->getComposite()->m_leaf){
        QTreeWidgetItem *clone =  m_twe->treeWidgetItems[it->getName()]->clone();
        delete m_twe->treeWidgetItems[it->getName()];
        m_twe->treeWidgetItems[it->getName()] = clone;

        item->addChild(clone);
    }

    updateTreeItemMap(item);
    return item;
}

QTreeWidgetItem* TreeWidgetObserver::doCloneShapeComposite(Shape *shape)
{
    // Type:CloneShapeComposite
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "name");
    item->setText(1, shape->getName());
    m_twe->treeWidgetItems[shape->getName()]= item;


    // 加入所有孩子
    for(auto it : shape->getComposite()->m_leaf){
        if(it->getComposite() != nullptr)
            m_twe->treeWidgetItems[it->getName()] = doCloneShapeComposite(it);
        else if(it->getDecorator() != nullptr)
            m_twe->treeWidgetItems[it->getName()] = doCreateShapeDecorator(it);
        else
            m_twe->treeWidgetItems[it->getName()] = doCreateShapeItem(it);

        item->addChild(m_twe->treeWidgetItems[it->getName()]);
    }

    updateTreeItemMap(item);

    return item;
}

void TreeWidgetObserver::doDeleteShapeItem(Shape *shape)
{
    // Type: DeleteShapeItem
    // 判断是否是组合
    if(shape->getComposite() != nullptr){
        foreach(auto it, shape->getComposite()->m_leaf){
            doDeleteShapeItem(it);
        }
    } else if(shape->getDecorator() != nullptr){
        doDeleteShapeItem(shape->getDecorator()->getRef());
    }
    delete m_twe->treeWidgetItems[shape->getName()];
    m_twe->treeWidgetItems.remove(shape->getName());
}

void TreeWidgetObserver::doCancelShapeComposite(Shape *shape)
{
    for(auto& it : shape->getComposite()->m_leaf){
        m_twe->treeWidgetItems[shape->getComposite()->getName()]
                ->removeChild(m_twe->treeWidgetItems[it->getName()]);
        m_twe->treeWidget->addTopLevelItem(m_twe->treeWidgetItems[it->getName()]);
    }
    delete m_twe->treeWidgetItems[shape->getComposite()->getName()];
    m_twe->treeWidgetItems.remove(shape->getComposite()->getName());
}

void TreeWidgetObserver::doSelectItem(Shape *shape)
{
    for(auto parentItem = m_twe->treeWidgetItems[shape->getName()];
        parentItem != nullptr; parentItem = parentItem->parent()) {
        parentItem->setSelected(true);
        parentItem->setExpanded(true);
        expendItem(parentItem);
    }
}

void TreeWidgetObserver::doRemoveSelect(Shape *shape)
{
    m_twe->treeWidgetItems[shape->getName()]->setSelected(false);
    m_twe->treeWidgetItems[shape->getName()]->setExpanded(false);
}

void TreeWidgetObserver::doClearSelect()
{
    for(auto it = m_twe->treeWidgetItems.begin();
        it != m_twe->treeWidgetItems.end(); ++it){
        m_twe->treeWidgetItems[it.key()]->setSelected(false);
        m_twe->treeWidgetItems[it.key()]->setExpanded(false);
    }

}

void TreeWidgetObserver::doUpdateTreeItem(Shape *shape){
    if(shape->getComposite() == nullptr
            && shape->getDecorator() == nullptr){
        for(int i = 0; i != m_twe->treeWidgetItems[shape->getName()]->childCount(); ++i){
            if (m_twe->treeWidgetItems[shape->getName()]->child(i)->text(0) == "start") {
                m_twe->treeWidgetItems[shape->getName()]->child(i)->setText(
                            1, "(" + QString::number(shape->getStart().x())
                         + "," + QString::number(shape->getStart().y()) + ")"
                            );
            }else if(m_twe->treeWidgetItems[shape->getName()]->child(i)->text(0) == "end"){
                m_twe->treeWidgetItems[shape->getName()]->child(i)->setText(
                            1, "(" + QString::number(shape->getEnd().x())
                         + "," + QString::number(shape->getEnd().y()) + ")"
                            );
            }
        }
    } else if (shape->getComposite() != nullptr){
        foreach(auto it, shape->getComposite()->m_leaf){
            doUpdateTreeItem(it);
        }
    } else if (shape->getDecorator() != nullptr){
            doUpdateTreeItem(shape->getDecorator()->getRef());

    }else{
        static_assert (1,"doUpdateTreeItem error" );
    }


}

void TreeWidgetObserver::doClearAll()
{
//    for(auto it = m_twe->treeWidgetItems.begin(); it != m_twe->treeWidgetItems.end(); ++it){
//        qDebug() << it.value();
//        it.value()->
//        delete it.value();
//    }
    m_twe->treeWidget->clear();
    m_twe->treeWidgetItems.clear();
}

void TreeWidgetObserver::updateTreeItemMap(QTreeWidgetItem* item){
    if(item->text(0) != "name")
        return;
    m_twe->treeWidgetItems[item->text(1)] = item;
    for(int i = 0; i < item->childCount(); ++i){
        updateTreeItemMap(item->child(i));
    }
}

void TreeWidgetObserver::expendItem(QTreeWidgetItem *item)
{
    for(int i = 0; i != item->childCount();++i){
        if ( item->child(i)->text(0) == "name" ) {
            item->child(i)->setExpanded(true);
            expendItem(item->child(i));
        }
    }
}
