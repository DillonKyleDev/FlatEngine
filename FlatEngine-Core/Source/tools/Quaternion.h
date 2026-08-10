#pragma once
#include "tools/Vector3.h"




namespace FlatEngine
{
    class Quaternion 
    {
    public:        
        static Quaternion EulerToQuaternion(const Vector3& eulerRotation);
        static Vector3 QuaternionToEuler(const Quaternion& q);
        static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

        Quaternion(float scaler = 1, Vector3 vector = Vector3(0));
        void CreateRotation(const Vector3& eulerRotations);
        void Normalize();
        Quaternion Inverse();
        Quaternion operator*(const Quaternion& right) const;

        float s;
        float x;
        float y;
        float z;
    };
}