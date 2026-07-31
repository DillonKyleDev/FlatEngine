#pragma once
#include "tools/Logger.h"

#include <string>
#include <unordered_map>
#include <vector>


namespace FlatEngine
{    
    template<class T>
    struct UMapVector {

            UMapVector()
            {
                vec = std::vector<T>();
                IDtoIndex = std::unordered_map<long, long>();
            }

            T* Get(long ID)
            {
                auto iter = IDtoIndex.find(ID);
                if (iter == IDtoIndex.end())
                {
                    return nullptr;
                }
                return &vec[iter->second];
            }   
            
            T* Add(long ID, T item)
            {
                if (IDtoIndex.count(ID))
                {
                    Logger::log.Err("UMapVec item not created, ID {} already taken", std::to_string(ID));
                    return nullptr;
                }

                IDtoIndex[ID] = vec.size();
                vec.push_back(item);
                return &vec.back();
            }

            void Remove(long ID)
            {
                auto it = IDtoIndex.find(ID);
                if (it == IDtoIndex.end()) return;

                size_t index     = it->second;
                size_t lastIndex = vec.size() - 1;

                long movedKey = IDtoIndex.end()->first;

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
                vec.clear();
                IDtoIndex.clear();
            }

            std::vector<T>& GetAll()
            {
                return vec;
            }

        private:
            std::vector<T> vec;
            std::unordered_map<long, long> IDtoIndex;
    };
}