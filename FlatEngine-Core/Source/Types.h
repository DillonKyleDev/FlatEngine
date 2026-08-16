#pragma once
#include "tools/Logger.h"

#include <function.hpp>
#include <string>
#include <unordered_map>
#include <vector>


namespace FlatEngine
{    
    template<class T>
    T GetTypeFromString(std::unordered_map<std::string, T> map, std::string typeString)
    {
        auto it = map.find(typeString);
        if (it == map.end())
        {
            Logger::log.Err("Unknown component type: {}", typeString);
            return (T)0;
        }
        return it->second;
    }

    template<class T>
    struct UMapVector {

            UMapVector(std::function<void(T&)> cleanup = nullptr)
            {
                m_cleanup = cleanup;
                vec = std::vector<T>();
                IDtoIndex = std::unordered_map<long, long>();
            }

            T* Get(long ID)
            {
                if (ID == -1)
                    return nullptr;

                auto iter = IDtoIndex.find(ID);
                if (iter == IDtoIndex.end())
                {
                    return nullptr;
                }
                return &vec[iter->second];
            }   

            std::vector<T>& GetAll()
            {
                return vec;
            }
            
            T* Add(long ID, T item)
            {
                if (IDtoIndex.count(ID))
                {
                    Logger::log.Err("UMapVec item not created, ID {} already taken", std::to_string(ID));
                    return nullptr;
                }

                IDtoIndex[ID] = vec.size();
                vec.push_back(std::move(item));
                return &vec.back();
            }

            void Remove(long ID)
            {
                auto it = IDtoIndex.find(ID);
                if (it == IDtoIndex.end()) return;

                size_t index     = it->second;
                size_t lastIndex = vec.size() - 1;

                if (index != lastIndex)
                {
                    std::swap(vec[index], vec[lastIndex]);
                    
                    // find whichever ID currently maps to lastIndex
                    for (auto& [otherID, i] : IDtoIndex)
                    {
                        if (i == lastIndex)
                        {
                            i = index;
                            break;
                        }
                    }
                }

                // Remove the last element (our target is now there)
                vec.pop_back();
                IDtoIndex.erase(it);
            }

            void Clear()
            {                
                if (m_cleanup != nullptr)
                {
                    for (T& item : vec)
                    {
                        m_cleanup(item);
                    }
                }

                vec.clear();
                IDtoIndex.clear();
            }

        private:
            std::vector<T> vec;
            std::unordered_map<long, long> IDtoIndex;
            std::function<void(T&)> m_cleanup;
    };
}