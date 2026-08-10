#include "tools/Numbers.h"
#include "tools/Vector3.h"
#include "tools/Quaternion.h"




namespace FlatEngine
{
    Quaternion::Quaternion(float scaler, Vector3 vector)
    {
        s = scaler;
        v = vector;
    }
        
    void Quaternion::CreateRotation(Vector3 eulerRotations)
    {
        Quaternion q1 = Quaternion(Numbers::CosD(eulerRotations.x / 2), Vector3(Numbers::SinD(eulerRotations.x / 2), 0, 0));
        Quaternion q2 = Quaternion(Numbers::CosD(eulerRotations.y / 2), Vector3(0, Numbers::SinD(eulerRotations.y / 2), 0));
        Quaternion q3 = Quaternion(Numbers::CosD(eulerRotations.z / 2), Vector3(0, 0, Numbers::SinD(eulerRotations.z / 2)));

        Quaternion q = q1 * q2 * q3;
        q.Normalize();

        s = q.s;
        v = q.v;
    }

    void Quaternion::Normalize()
    {
        float magnitude = Numbers::Sqrt(s*s + v.x*v.x + v.y*v.y + v.z*v.z);

        s /= magnitude;
        v.x /= magnitude;
        v.y /= magnitude;
        v.z /= magnitude;
    }

    Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
    {
        float dot = a.s * b.s + a.v.x * b.v.x + a.v.y * b.v.y + a.v.z * b.v.z;

        Quaternion bAdjusted = b;
        if (dot < 0.0f)
        {
            dot = -dot;
            bAdjusted = Quaternion(-b.s, Vector3(-b.v.x, -b.v.y, -b.v.z));
        }

        if (dot > 0.9995f)
        {
            // a and b are nearly identical; the sin(theta)-based formula becomes numerically
            // unstable as theta approaches 0 (dividing by something near zero), so fall back
            // to plain linear interpolation, which is accurate enough at this small a separation
            float w = a.s + t * (bAdjusted.s - a.s);
            float x = a.v.x + t * (bAdjusted.v.x - a.v.x);
            float y = a.v.y + t * (bAdjusted.v.y - a.v.y);
            float z = a.v.z + t * (bAdjusted.v.z - a.v.z);
            Quaternion result(w, Vector3(x, y, z));
            result.Normalize();
            return result;
        }

        float theta = Numbers::ACos(dot);
        float sinTheta = Numbers::Sin(theta);
        float scaleA = Numbers::Sin((1.0f - t) * theta) / sinTheta;
        float scaleB = Numbers::Sin(t * theta) / sinTheta;

        float w = scaleA * a.s + scaleB * bAdjusted.s;
        float x = scaleA * a.v.x + scaleB * bAdjusted.v.x;
        float y = scaleA * a.v.y + scaleB * bAdjusted.v.y;
        float z = scaleA * a.v.z + scaleB * bAdjusted.v.z;

        return Quaternion(w, Vector3(x, y, z));
    }

    Quaternion Quaternion::operator*(const Quaternion& right) const
    {
        float scaler = (s * right.s)   - (v.x * right.v.x) - (v.y * right.v.y) - (v.z * right.v.z);
        float x      = (s * right.v.x) + (right.s * v.x)   + (v.y * right.v.z) - (v.z * right.v.y); 
        float y      = (s * right.v.y) + (right.s * v.y)   + (v.z * right.v.x) - (v.x * right.v.z);
        float z      = (s * right.v.z) + (right.s * v.z)   + (v.x * right.v.y) - (v.y * right.v.x);

        Quaternion q = Quaternion(scaler, Vector3(x,y,z));
        return q;
    }
}