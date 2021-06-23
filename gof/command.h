#ifndef COMMAND_H
#define COMMAND_H
#include <QString>
#include "shape/shapeImport.h"
class Command
{
public:
    Command(QString name):m_name(name){
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
        m_date = current_date;
    };
    QString getName() { return m_name; }
    QString getDate() { return m_date; }
    virtual void Execute() = 0;
    virtual void undo() = 0;
protected:
    QString m_name;
    QString m_date;
};

class MoveCommand: public Command{
public:
    MoveCommand(Shape* obj, double moveX, double moveY):
        Command(QString("move shapeItem(") + QString::number(moveX) + ", " + QString::number(moveY) + ")"),
        m_obj(obj), m_moveX(moveX), m_moveY(moveY) {
    }
    void Execute() override{
        m_obj->getItem()->setPos(
                    m_obj->getStart().x() + m_moveX,
                    m_obj->getStart().y() + m_moveY);
    }
    void undo() override{
        m_obj->getItem()->setPos(
                    m_obj->getStart().x() - m_moveX,
                    m_obj->getStart().y() - m_moveY);
    }
private:
    Shape* m_obj;
    double m_moveX, m_moveY;
};

#endif // COMMAND_H
