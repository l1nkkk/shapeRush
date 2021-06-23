#ifndef PROTOTYPE_H
#define PROTOTYPE_H

template <typename T>
class ProtoType
{
public:
    virtual T* Clone() = 0;
};

#endif // PROTOTYPE_H
