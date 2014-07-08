#ifndef _UBASE_QUEUE_H_
#define _UBASE_QUEUE_H_

#include <deque>
#include "ubase/mutex.h"

namespace ubase
{
    template <class T> 
    class Queue
    {
    public:
        Queue()
        {}

        virtual ~Queue()
        {}

        bool pop(T &msg)
        {
            ScopedLock lock(m_mutex);
            if (m_queue.empty())
                return false;
            msg = m_queue[0];
            m_queue.pop_front();
            return true;
        }

        void push(const T &msg)
        {
            ScopedLock lock(m_mutex);
            m_queue.push_back(msg);
        }

    private:
        std::deque<T> m_queue;
        Mutex m_mutex;
    };
}

#endif

