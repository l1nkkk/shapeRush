#include "serialization.h"
#include <fstream>
#include <QDebug>

#include "../shape/shape.h"
#include "../gof/factory.h"
#include "../gof/singletom.h"
#include "../gof/composite.h"
#include "../gof/decorator.h"


namespace YAML {

QQueue<ShapeData> importFromFile(QString fileName, QMap<QString, YAML::ShapeData> &buildMap)
{
//    Q_ASSERT(existShape != nullptr);
    qDebug() <<  "test importToFile :" << fileName;
    YAML::Node node = YAML::LoadFile(fileName.toStdString());
    QQueue<ShapeData> buildQueue;

    // 反序列化
    for(auto it : node){
         auto data = it.as<YAML::ShapeData>();
         buildMap[data.name.c_str()] = data;
         if(data.type == Shape::LINE || data.type == Shape::RECT
                 || data.type == Shape::ELLIPSE || data.type == Shape::BUTTON)
             buildQueue.push_back(data);
    }

    qDebug() <<  "test importToFile : end" << fileName;
    return buildQueue;

}

void exportToFile(QString fileName, QMap<QString, Shape*> *existShape)
{
    Q_ASSERT(existShape != nullptr);
    YAML::Emitter out;
    out << YAML::BeginSeq;
    foreach (auto it, *existShape) {
        out << it->toShapeData() ;
    }
    out << YAML::EndSeq;
    std::ofstream f;
    f.open(fileName.toStdString());
    if(f.is_open() == false){
        qDebug() << " error Open File IN function exportToFile() fileName:" << fileName;
        return;
    }
    f << out.c_str();
    f.close();
}
YAML::Emitter& operator << (YAML::Emitter& out, const ShapeData& v) {
    out << YAML::BeginMap;
    out << YAML::Key << "name" << YAML::Value << v.name;
    out << YAML::Key << "type" << YAML::Value << v.type;
    out << YAML::Key << "startX" << YAML::Value << v.startX;
    out << YAML::Key << "startY" << YAML::Value << v.startY;
    out << YAML::Key << "endX" << YAML::Value << v.endX;
    out << YAML::Key << "endY" << YAML::Value << v.endY;
    out << YAML::Key << "childNum" << YAML::Value << v.childNum;
    out << YAML::Key << "width" << YAML::Value << v.width;
    out << YAML::Key << "height" << YAML::Value << v.height;
    out << YAML::Key << "isEnhanced" << YAML::Value << v.isEnhanced;
    out << YAML::Key << "parent" << YAML::Value << v.parent;
    out << YAML::EndMap;
    return out;
}


template<>
struct  convert<ShapeData> {
    static Node encode(const ShapeData& rhs){
        Node node;
        node["name"]        = rhs.name;
        node["type"]        = rhs.type;
        node["startX"]      = rhs.startX;
        node["startY"]      = rhs.startY;
        node["endX"]        = rhs.endX;
        node["endY"]        = rhs.endY;
        node["childNum"]    = rhs.childNum;
        node["width"]       = rhs.width;
        node["height"]      = rhs.height;
        node["isEnhanced"]  = rhs.isEnhanced;
        node["parent"]      = rhs.parent;
        return node;
    }
    static bool decode(const Node& node, ShapeData& rhs) {
        if(!node.IsMap() || node.size() != 11){
            return false;
        }
        rhs.name        = node["name"].as<std::string>();
        rhs.type        = node["type"].as<int>();
        rhs.startX      = node["startX"].as<double>();
        rhs.startY      = node["startY"].as<double>();
        rhs.endX        = node["endX"].as<double>();
        rhs.endY        = node["endY"].as<double>();
        rhs.childNum    = node["childNum"].as<int>();
        rhs.width       = node["width"].as<double>();
        rhs.height      = node["height"].as<double>();
        rhs.isEnhanced  = node["isEnhanced"].as<bool>();
        rhs.parent      = node["parent"].as<std::string>();
        return true;
    }
};

Shape *ShapeData::toShape(QQueue<ShapeData> &buildQueue, QMap<QString, ShapeData> &buildMap,
                           QMap<QString, Shape *> &existShape, QMap<QString,QList<QString>> &parToChi)
{
//    if(tset.find(this) != tset.end()){}
    Shape *rtn = nullptr;
    //是否增强版
    if (this->isEnhanced){
        //判断type类型
        if(type == Shape::LINE){
            rtn = Singletom<ExFactoryShape>::getInstance()->CreateLine(QPointF(startX, startY),
                                                                 QPointF(endX, endY));
        } else if (type == Shape::RECT){
            rtn = Singletom<ExFactoryShape>::getInstance()->CreateRect(QPointF(startX, startY),
                                                                 QPointF(endX, endY));
        } else if (type == Shape::ELLIPSE){
            rtn = Singletom<ExFactoryShape>::getInstance()->CreateElli(QPointF(startX, startY),
                                                                 QPointF(endX, endY));
        } else if (type ==Shape::BUTTON){
            rtn = Singletom<ExFactoryShape>::getInstance()->CreateButton(QPointF(startX, startY),
                                                                 QPointF(endX, endY));
        } else {
            qDebug() << " error in toShape: type error";
            return rtn;
        }
    } else{
        if(type == Shape::LINE){
            rtn = Singletom<FactoryShape>::getInstance()->CreateLine(QPointF(startX, startY),
                                                                 QPointF(endX, endY));
        } else if (type == Shape::RECT){
            rtn = Singletom<FactoryShape>::getInstance()->CreateRect(QPointF(startX, startY),
                                                                 QPointF(endX, endY));
        } else if (type == Shape::ELLIPSE){
            rtn = Singletom<FactoryShape>::getInstance()->CreateElli(QPointF(startX, startY),
                                                                 QPointF(endX, endY));
        } else if (type ==Shape::BUTTON){
            rtn = Singletom<FactoryShape>::getInstance()->CreateButton(QPointF(startX, startY),
                                                                 QPointF(endX, endY));
        } else if (type == Shape::COMPOSITE){
            rtn = new Composite();
            for(auto it: parToChi[name.c_str()]){
                rtn->getComposite()->addChild(existShape[it]); // 加入孩子
            }

        } else if (type == Shape::SPIN){
            rtn = new SpinItem(existShape[parToChi[name.c_str()][0]]);

        } else if (type == Shape::TEXT){
            rtn = new TextItem(existShape[parToChi[name.c_str()][0]]);
        } else {
            qDebug() << " error in toShape: type error";
            return rtn;
        }
    }
    rtn->setName(this->name.c_str());
    if(this->parent != ""){
        if(parToChi.find(this->parent.c_str()) == parToChi.end())
            parToChi[this->parent.c_str()] = QList<QString>();
        // 记录孩子
        parToChi[this->parent.c_str()].push_front(rtn->getName());
        // 如果已经记录的孩子等于孩子数
        if(parToChi[this->parent.c_str()].size() == buildMap[this->parent.c_str()].childNum)
            buildQueue.push_back(buildMap[this->parent.c_str()]);
    }

    return rtn;

}


}
