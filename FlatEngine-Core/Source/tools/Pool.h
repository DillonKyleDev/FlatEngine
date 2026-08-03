#pragma once
#include "tools/Logger.h"

#include <function.hpp>
#include <vector>



namespace FlatEngine
{
    template<class T>
    struct PoolObject {
        long index = 0;
        T* object = nullptr;
    };

    template<class T>
    class Pool {
        public:
            Pool(std::function<T()> create, std::function<void(T&)> cleanup, size_t initialSize)
            {
                m_currentObjectIndex = 0;
                m_poolObjects.resize(initialSize);
                m_create = create;
                m_cleanup = cleanup;
            }

            ~Pool()
            {
                for (T& object : m_poolObjects)
                {
                    m_cleanup(object);
                }
            }

            void Init()
            {
                for (T& object : m_poolObjects)
                {
                    object = m_create();
                }

                m_b_initialized = true;
            }

            bool Initialized()
            {
                return m_b_initialized;
            }

            PoolObject<T> Get()
            {
                PoolObject<T> poolObject;
                
                if (m_returned.size())
                {
                    poolObject.index = m_returned.back();
                    m_returned.pop_back();
                    poolObject.object = &m_poolObjects[poolObject.index];
                }        
                else if (m_currentObjectIndex >= m_poolObjects.size())
                {
                    T newObject = m_create();
                    m_currentObjectIndex = m_poolObjects.size();
                    m_poolObjects.push_back(newObject);     
                    poolObject.index = m_currentObjectIndex;
                    poolObject.object = &m_poolObjects.back();
                }
                else
                {                    
                    poolObject.index = m_currentObjectIndex;
                    poolObject.object = &m_poolObjects[m_currentObjectIndex];
                    m_currentObjectIndex++;
                }

                return poolObject;
            }

            void Return(PoolObject<T> returnedObject)
            {
                if (std::find(m_returned.begin(), m_returned.end(), returnedObject.index) != m_returned.end())
                {
                    Logger::log.Warn("Pool::Return called twice for index {}", returnedObject.index);
                    return;
                }

                m_returned.push_back(returnedObject.index);
            }

            void ReturnAll()
            {
                m_currentObjectIndex = 0;
                m_returned.clear();
            }

        private:
            std::vector<T> m_poolObjects;
            long m_currentObjectIndex;
            std::vector<long> m_returned;
            std::function<T()> m_create;
            std::function<void(T&)> m_cleanup;
            bool m_b_initialized = false;
    };
}