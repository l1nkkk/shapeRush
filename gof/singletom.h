#ifndef SINGLETOM_H
#define SINGLETOM_H

template <typename T, typename SRCT = T,int type = 0>
class Singletom
{
public:
    Singletom() = delete;
    static T* getInstance(){
        static SRCT instance;
        return &instance;
    }

};

#endif // SINGLETOM_H
