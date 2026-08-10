#pragma once
#include "tools/Vector3.h"




namespace FlatEngine
{
    class Quaternion 
    {
    public:
        Quaternion(float scaler = 1, Vector3 vector = Vector3(0));
        
        void CreateRotation(Vector3 eulerRotations);
        void Normalize();
        Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
        Quaternion operator*(const Quaternion& right) const;

        float s;
        Vector3 v;
    };
}