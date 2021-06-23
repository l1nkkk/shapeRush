#ifndef ITERATOR_H
#define ITERATOR_H
#include "command.h"
#include "singletom.h"
#include <QList>
#include <memory>

template <typename T>
class LIterator
{
public:
    LIterator() = default;
    virtual void Next() = 0;
    virtual void First() = 0;
    virtual T* Current() = 0;
    virtual bool IsDone() = 0;
public:
//    LIterator(const LIterator&) = delete;
//    LIterator& operator= (const LIterator&) = delete;
//    LIterator(const LIterator&&) = delete;

};


class CmdIter: public LIterator<Command>{
public:
    // 初始位置
    CmdIter(QList<Command*> *cmdlist, int endlist):LIterator(), m_cmdList(cmdlist), m_endList(endlist){};
    // 任意位置
    CmdIter(QList<Command*> *cmdlist, int endlist, int index)
        : LIterator(), m_cmdList(cmdlist), m_endList(endlist){ m_now = index; };
    void First() override { m_now = 0; }
    bool IsDone() override {
        // 大于最后的那个，就是迭代器到头了
        return m_now > m_endList;
    }
    void Next() override { ++m_now; }
    Command * Current() override { return (*m_cmdList)[m_now]; }

private:
    QList<Command* > *m_cmdList;
    int m_now = 0;
    int m_endList;  // 包括的
};


class ReCmdIter: public LIterator<Command>{
public:
    // 初始位置
    ReCmdIter(QList<Command*> *cmdlist) : LIterator(), m_cmdList(cmdlist){ m_now = m_cmdList->size()-1;};

    // 任意位置
    ReCmdIter(QList<Command*> *cmdlist, int index) : LIterator(), m_cmdList(cmdlist){ m_now = index; };
    void First() override {
        m_now = m_cmdList->size()-1;
    }
    bool IsDone() override {
        return m_now <= -1;
    }
    void Next() override { --m_now; }
    Command * Current() override {
        return (*m_cmdList)[m_now];
    }

private:
    QList<Command* > *m_cmdList;
    int m_now = 0;
};


class CommandHistory{
    friend class ReCmdIter;
    friend class CmdIter;
public:
    static CommandHistory* Instance(){
        return Singletom<CommandHistory>::getInstance();
    }


    std::shared_ptr<LIterator<Command> > createCmdIterFromFirst(){
        return std::shared_ptr<LIterator<Command> >(new CmdIter(&m_cmdList, m_cur));
    }
    std::shared_ptr<LIterator<Command> >
    createReCmdIterFromFirst(){
        return std::shared_ptr<LIterator<Command> >(new ReCmdIter(&m_cmdList));
    }


    std::shared_ptr<LIterator<Command> > createCmdIterFromCur(){
        return std::shared_ptr<LIterator<Command> >(new CmdIter(&m_cmdList, m_cmdList.size()-1, m_cur));
    }
    std::shared_ptr<LIterator<Command> > createReCmdIterFromCur(){
        return std::shared_ptr<LIterator<Command> >(new ReCmdIter(&m_cmdList, m_cur));
    }
    void add(Command* cmd){
        if(m_cur != m_cmdList.size()){
            auto times = m_cmdList.size() - 1 - m_cur;
            while(times--)
                m_cmdList.removeLast();
        }
        m_cmdList.append(cmd);
        ++m_cur;
    }
    void decCur(){
        --m_cur;
    }
    void addCur(){
        ++m_cur;
    }
    bool redoAble(){
        return m_cur+1 < m_cmdList.size();
    }
    int size(){ return m_cur; }
    void clear(){
        m_cmdList.clear();
        m_cur = -1;
    }
private:
    QList<Command* > m_cmdList;
    int m_cur = -1;      //当前命令历史指针
};




#endif // ITERATOR_H
