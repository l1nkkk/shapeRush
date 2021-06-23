#ifndef SHAPE_H
#define SHAPE_H

#include <QMainWindow>
#include <QVariant>
#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QTime>
#include <QDebug>
#include <QUuid>
#include <memory>

#include <gof/prototype.h>
#include "storage/serialization.h"


class Composite;
class DecoratorItem;
class Shape;
class ImageProxy;

class ShapeInterface : public QObject{
    Q_OBJECT
signals:
    void shapeCopy(Shape *cpShape);
    void compositeEvent(bool flag);
    void cancelCompositeEvent(Composite *cps);
    void deleteEvent(Shape *sp);
    void spinDeractor(Shape *sp);
    void textDeractor(Shape *sp);
};


class Shape :public ProtoType<Shape> {
public:
    enum CreateType{
        NONE = 0,
        LINE,
        ELLIPSE,
        RECT,
        BUTTON,
        COMPOSITE,
        CLONE,
        SPIN,
        TEXT
    };
public:

    typedef std::shared_ptr<Shape> sptr;
    typedef std::weak_ptr<Shape> wptr;
    typedef std::unique_ptr<Shape> uptr;
    struct Color{
        Color()=default;
        Color(QColor alineColor, QColor afocusLineColor, QColor afillColor) :
            lineColor(alineColor), focusLineColor(afocusLineColor), fillColor(afillColor){}
        QColor lineColor = Qt::black;
        QColor focusLineColor = Qt::green;
        QColor fillColor = Qt::red;
    };
public:
    Shape(CreateType atype, QString name = "NONE-"): m_type(atype){
        m_pen = QPen(m_color.lineColor);
        m_pen.setWidth(m_penWidth);
        m_brush = QBrush(m_color.fillColor);

        // uuid
        QUuid id = QUuid::createUuid();
        QString strId = id.toString();

        m_name = name+strId;
        m_variant = QVariant::fromValue(static_cast<void *>(this));
    }

    Shape(CreateType type, QPointF start, QPointF end, QString namePre, QColor lineColor, \
          QColor focusLineColor, QColor fillColor) : \
            m_type(type), m_color(lineColor, focusLineColor, fillColor){

        setStart(start);
        setEnd(end);

        // uuid
        QUuid id = QUuid::createUuid();
        QString strId = id.toString();

        m_pen = QPen(m_color.lineColor);
        m_pen.setWidth(m_penWidth);
        m_brush = QBrush(m_color.fillColor);

        m_name = namePre + strId;
        m_variant = QVariant::fromValue(static_cast<void *>(this));

        m_width = qAbs(m_start.x()-m_end.x());
        m_height = qAbs(m_start.y()-m_end.y());
    }

    Shape(const Shape& s):Shape(s.m_type,s.m_start, s.m_end, s.m_name.split("-")[0]+'-', s.m_color.lineColor,\
                                s.m_color.focusLineColor, s.m_color.fillColor){
        m_penWidth = s.m_penWidth;
        m_variant = QVariant::fromValue(static_cast<void *>(this));
    }
//    Shape(const YAML::ShapeData& s);

    virtual ~Shape(){
        delete m_item;
    }

public:
    inline void setStart(qreal x, qreal y){m_start = QPoint(x,y);}
    inline void setStart(QPointF start){ m_start = start;}
    inline void setEnd(qreal x, qreal y){ m_end = QPointF(x,y);}
    inline void setEnd(QPointF end){ m_end = end;}
    inline void setName(QString name) { m_name = name; }
    inline void setSelect(bool isSelect){ m_isSelect = isSelect;}
    inline void setInter(ShapeInterface* inter){ m_inter = inter;}
    inline void setComposite(Composite* comp){ m_comp = comp; }
    inline void setIsDecorator(DecoratorItem *dec){ m_deco = dec; }
    inline void setIsEnhanced(bool isEnhanced) { m_isEnhanced = isEnhanced; }
    inline void setParentName(QString name) { m_parentName = name; }
    inline void setChildNum(int num) { m_childNum = num; }
    inline ImageProxy* getProxy() const{ return m_proxy; }
    inline int getChildNum() { return m_childNum; }
    inline QString getParentName(){ return m_parentName; }
    inline qreal getWidth(){ return m_width; }
    inline qreal getHeight(){ return m_height; }
    inline DecoratorItem* getDecorator(){ return m_deco; }
    inline virtual Composite* getComposite() { return m_comp;}
    inline  ShapeInterface* getInter() { return m_inter;}
    inline QString getName() const {return m_name;}
    inline QPen& getPen() { return m_pen; }
    inline QBrush& getBrush() { return m_brush; }
    inline const QPointF& getStart() const { return m_start; }
    inline const QPointF& getEnd() const { return m_end; }
    inline const Color& getColor() const { return m_color; }
    inline CreateType getType() const { return m_type; }
    inline void setItem(QGraphicsItem* item){ m_item = item; }
    inline QGraphicsItem* getItem() const { return m_item; }
    inline const QVariant& getVariant() const { return m_variant; }
    inline bool isSelect() const {return m_isSelect; }
    inline YAML::ShapeData toShapeData(){
        return YAML::ShapeData(m_name.toStdString(), m_type, double(m_start.x()), double(m_start.y()),
                               double(m_end.x()), double(m_end.y()), getChildNum(), double(m_width),
                               double(m_height), m_isEnhanced, m_parentName.toStdString());
    }
//    static Shape*
protected:
    // ui
    Color m_color;
    QPen m_pen;
    QBrush m_brush;
    QString m_name;
    QString m_parentName = "";
    QPointF m_start = QPointF(0,0), m_end = QPointF(0,0);
    int8_t m_penWidth = 4;
    qreal m_width = 0;
    qreal m_height = 0;

    // 状态
    bool m_isSelect = false;
    QGraphicsItem* m_item = nullptr;
    ShapeInterface *m_inter = nullptr;
    ImageProxy *m_proxy = nullptr;
    QVariant m_variant;
    Composite *m_comp = nullptr;
    DecoratorItem *m_deco = nullptr;
    const CreateType m_type;
    bool m_isEnhanced = false;
    int m_childNum = 0;
};



#endif // SHAPE_H
