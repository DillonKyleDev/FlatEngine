#include "tools/Numbers.h"
#include "tools/Quaternion.h"
#include "tools/Vector3.h"

#include <fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>


namespace FlatEngine
{
    // (degrees) Euler xyz-axis rotation Vector3 to Quaternion rotation
    Quaternion Quaternion::EulerToQuaternion(const Vector3& eulerRotation)
    {
        Quaternion q;
        q.CreateRotation(eulerRotation);
        return q;
    }
    // (degrees) Quaternion rotation to Euler xyz-axis Vector3
    Vector3 Quaternion::QuaternionToEuler(const Quaternion& q)
    {
        float w = q.s, x = q.x, y = q.y, z = q.z;

        float r02 = 2.0f*(x*z + w*y);
        float r12 = 2.0f*(y*z - w*x);
        float r22 = 1.0f - 2.0f*(x*x + y*y);
        float r01 = 2.0f*(x*y - w*z);
        float r00 = 1.0f - 2.0f*(y*y + z*z);

        float sinY = Numbers::Clamp(r02, -1.0f, 1.0f);

        float angleX, angleY, angleZ;

        if (std::abs(sinY) > 0.9999f)
        {
            angleY = std::asin(sinY);
            angleZ = 0.0f;
            angleX = std::atan2(2.0f*(x*y + w*z), 1.0f - 2.0f*(x*x + z*z));
        }
        else
        {
            angleY = std::asin(sinY);
            angleX = std::atan2(-r12, r22);
            angleZ = std::atan2(-r01, r00);
        }

        return Vector3(
            Numbers::RadiansToDegrees(angleX),
            Numbers::RadiansToDegrees(angleY),
            Numbers::RadiansToDegrees(angleZ)
        );
    }

    Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
    {
        float dot = a.s * b.s + a.x * b.x + a.y * b.y + a.z * b.z;

        Quaternion bAdjusted = b;
        if (dot < 0.0f)
        {
            dot = -dot;
            bAdjusted = Quaternion(-b.s, Vector3(-b.x, -b.y, -b.z));
        }

        if (dot > 0.9995f)
        {
            // a and b are nearly identical; the sin(theta)-based formula becomes
            // unstable as theta approaches 0 (dividing by almost zero), so fall back
            // to linear interpolation
            float w = a.s + t * (bAdjusted.s - a.s);
            float x = a.x + t * (bAdjusted.x - a.x);
            float y = a.y + t * (bAdjusted.y - a.y);
            float z = a.z + t * (bAdjusted.z - a.z);
            Quaternion result(w, Vector3(x, y, z));
            result.Normalize();
            return result;
        }

        float theta =    Numbers::ACosR(dot);
        float sinTheta = Numbers::SinR(theta);
        float scaleA =   Numbers::SinR((1.0f - t) * theta) / sinTheta;
        float scaleB =   Numbers::SinR(t * theta) / sinTheta;

        float s = scaleA * a.s + scaleB * bAdjusted.s;
        float x = scaleA * a.x + scaleB * bAdjusted.x;
        float y = scaleA * a.y + scaleB * bAdjusted.y;
        float z = scaleA * a.z + scaleB * bAdjusted.z;

        return Quaternion(s, Vector3(x, y, z));
    }

    Quaternion::Quaternion(float scaler, Vector3 vector)
    {
        s = scaler;
        x = vector.x;
        y = vector.y;
        z = vector.z;
    }
        
    void Quaternion::CreateRotation(const Vector3& eulerRotations)
    {
        Quaternion qx = Quaternion(Numbers::Cos(eulerRotations.x / 2), Vector3(Numbers::Sin(eulerRotations.x / 2), 0, 0));
        Quaternion qy = Quaternion(Numbers::Cos(eulerRotations.y / 2), Vector3(0, Numbers::Sin(eulerRotations.y / 2), 0));
        Quaternion qz = Quaternion(Numbers::Cos(eulerRotations.z / 2), Vector3(0, 0, Numbers::Sin(eulerRotations.z / 2)));

        Quaternion q = qx * qy * qz;
        q.Normalize();

        s = q.s;
        x = q.x;
        y = q.y;
        z = q.z;
    }

    void Quaternion::Normalize()
    {
        float magnitude = Numbers::Sqrt(s*s + x*x + y*y + z*z);

        s /= magnitude;
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
    }

    Quaternion Quaternion::Inverse()
    {
        return Quaternion(s, Vector3(x, y, z) * -1);
    }

    Quaternion Quaternion::operator*(const Quaternion& right) const
    {
        float scaler = (s * right.s) - (x * right.x) - (y * right.y) - (z * right.z);
        float newX      = (s * right.x) + (right.s * x) + (y * right.z) - (z * right.y); 
        float newY      = (s * right.y) + (right.s * y) + (z * right.x) - (x * right.z);
        float newZ      = (s * right.z) + (right.s * z) + (x * right.y) - (y * right.x);

        Quaternion q = Quaternion(scaler, Vector3(newX, newY, newZ));
        return q;
    }
}