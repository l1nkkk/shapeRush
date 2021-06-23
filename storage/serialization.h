#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include <string>
#include <QString>
#include <QMap>
#include <QSet>
#include <QQueue>
#include <QCoreApplication>
#include <yaml-cpp/yaml.h>
class Shape;



namespace YAML {
struct ShapeData;
QQueue<ShapeData> importFromFile(QString fileName, QMap<QString, YAML::ShapeData> &buildMap);

void exportToFile(QString fileName, QMap<QString, Shape*> *existShape);


YAML::Emitter& operator << (YAML::Emitter& out, const ShapeData& v);
struct ShapeData{
//    friend QQueue<ShapeData> importFromFile(QString fileName, QMap<QString, Shape*> &existShape);
    ShapeData() = default;
    ShapeData(const std::string &name, int type, double startX, double startY, double endX, double endY, int childNum, double width,
              double height, bool isEnhanced, const std::string &parent) : name(name), type(type), startX(startX),
                                                                           startY(startY), endX(endX), endY(endY),
                                                                           childNum(childNum), width(width), height(height),
                                                                           isEnhanced(isEnhanced), parent(parent) {}
public:

    Shape* toShape(QQueue<ShapeData> &buildQueue, QMap<QString, ShapeData> &buildList,
                    QMap<QString, Shape *> &existShape, QMap<QString,QList<QString>> &parToChi);

    bool operator== (const ShapeData& rhs){
        return name == rhs.name
        && type == rhs.type
        && startX == rhs.startX
        && startY == rhs.startY
        && endX == rhs.endX
        && endY == rhs.endY
        && childNum == rhs.childNum
        && width == rhs.width
        && height == rhs.height
        && isEnhanced == rhs.isEnhanced
        && parent == rhs.parent;
    }

public:
    std::string name = "";
    int type = 0;
    double startX = 0;
    double startY = 0;
    double endX = 0;
    double endY = 0;
    int childNum = 0;
    double width = 0;
    double height = 0;
    bool isEnhanced = false;
    std::string parent = "";
};

}
#endif // SERIALIZATION_H
