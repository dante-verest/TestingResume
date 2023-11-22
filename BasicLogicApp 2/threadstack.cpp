#include "threadstack.h"

ThreadStack::ThreadStack()
{
}

/*
 * убраны мьютексы за ненадобностью
 */

void ThreadStack::push(int n)
{
    m_stackInt.push(n);
}

int ThreadStack::pop()
{
    return m_stackInt.empty() ? 0 : m_stackInt.pop();
}

bool ThreadStack::isEmpty()
{
    return m_stackInt.isEmpty();
}
