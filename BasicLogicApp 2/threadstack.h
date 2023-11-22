#ifndef THREADSTACK_H
#define THREADSTACK_H

#include <QStack>

class ThreadStack
{
private:
    QStack<int> m_stackInt;

public:
    ThreadStack();
    void push(int n);
    int pop();
    bool isEmpty();
};

#endif // THREADSTACK_H
