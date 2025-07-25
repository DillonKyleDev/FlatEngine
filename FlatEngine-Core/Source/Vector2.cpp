#include "Vector2.h"
#include "Matrix.h"

#include <cmath>


namespace FlatEngine
{
	Vector2::Vector2()
	{
		x = 0;
		y = 0;
	}

	Vector2::Vector2(float xyValue)
	{
		x = xyValue;
		y = xyValue;
	}

	Vector2::Vector2(float xValue, float yValue)
	{
		x = xValue;
		y = yValue;
	}

	Vector2::Vector2(const ImVec2& toCopy)
	{
		x = toCopy.x;
		y = toCopy.y;
	}

	Vector2::Vector2(const Vector2& toCopy)
	{
		x = toCopy.x;
		y = toCopy.y;
	}

	Vector2::~Vector2()
	{
	}

	Vector2 Vector2::Normalize(Vector2 vec)
	{		
		Vector2 temp = Vector2();

		if (vec.x == 0 && vec.y == 0)
		{
			return Vector2(0, 0);
		}

		float hypotenuse = std::sqrt((vec.x * vec.x) + (vec.y * vec.y));

		temp.x = vec.x / hypotenuse;
		temp.y = vec.y / hypotenuse;

		float newhypotenuse = std::sqrt((vec.x * vec.x) + (vec.y * vec.y));

		return temp;
	}

	// Degrees
	float Vector2::GetAngleBetween(Vector2 vec1, Vector2 vec2)
	{
		return (float)fmod(((float)std::acos(vec1.Dot(vec2) / vec2.GetMagnitude()) * 57.29578), 360.0f);
	}

	Vector2 Vector2::Rotate(Vector2 vec, float degrees)
	{
		if (degrees == 0)
		{
			return vec;
		}

		float cosA;
		float sinA;

		if (degrees == 90 || degrees == 270)
		{
			cosA = 0;
			sinA = 1;
		}
		else if (degrees == 180 || degrees == 0 || degrees == 360)
		{
			cosA = 1;
			sinA = 0;
		}
		else
		{
			cosA = cosf(degrees * 2.0f * (float)M_PI / 360.0f);
			sinA = sinf(degrees * 2.0f * (float)M_PI / 360.0f);
		}

		Matrix2 rotationMatrix(cosA, -sinA, sinA, cosA);

		return rotationMatrix.LMultiply(vec);
	}

	float Vector2::GetX()
	{
		return x;
	}

	float Vector2::GetY()
	{
		return y;
	}

	void Vector2::SetX(float newX)
	{
		x = newX;
	}

	void Vector2::SetY(float newY)
	{
		y = newY;
	}

	void Vector2::_xy(float newX, float newY)
	{
		x = newX;
		y = newY;
	}

	Vector2 Vector2::operator=(ImVec2& right)
	{
		x = right.x;
		y = right.y;

		return *this;
	}

	Vector2 Vector2::operator*(ImVec2& right)
	{
		return Vector2(x * right.x, y * right.y);
	}

	Vector2 Vector2::operator*(Vector2& right)
	{
		return Vector2(x * right.x, y * right.y);
	}

	Vector2 Vector2::operator*(float scalar)
	{
		return Vector2(x * scalar, y * scalar);
	}

	Vector2 Vector2::operator+(const Vector2& right)
	{
		return Vector2(x + right.x, y + right.y);
	}

	Vector2 Vector2::operator-(const Vector2& right)
	{
		return Vector2(x - right.x, y - right.y);
	}

	void Vector2::Normalize()
	{
		*this = Normalize(*this);
	}

	Vector2 Vector2::NormalizeCardinal()
	{
		// Normalize X
		if (x > 0)
		{
			x = 1;
		}
		else if (x < 0)
		{
			x = -1;
		}
		else
		{
			x = 0;
		}

		// Normalize Y
		if (y > 0)
		{
			y = 1;
		}
		else if (y < 0)
		{
			y = -1;
		}
		else
		{
			y = 0;
		}

		return *this;
	}

	bool Vector2::operator==(const Vector2& right)
	{
		return (x == right.x && y == right.y);
	}

	bool Vector2::operator!=(const Vector2& right)
	{
		return !(x == right.x && y == right.y);
	}

	float Vector2::Dot(Vector2 with)
	{
		return ((x * with.x) + (y * with.y));
	}

	float Vector2::AmountProjectedOnto(Vector2 projectedTo)
	{
		if (projectedTo.GetMagnitude() == 0)
		{
			return 0;
		}
		return (((x * projectedTo.x) + (y * projectedTo.y)) / projectedTo.GetMagnitude());
	}

	Vector2 Vector2::ProjectedOnto(Vector2 projectedTo)
	{
		if ((x == 0 && y == 0) || (projectedTo.x == 0 && projectedTo.y == 0))
		{
			return Vector2(0, 0);
		}

		return projectedTo * ((this->Dot(projectedTo)) / (projectedTo.Dot(projectedTo)));
	}

	float Vector2::CrossKResult(Vector2 with)
	{
		// i   j   k
		// X1  Y1  0
		// X2  Y2  0
		float kComp = (x * with.y) - (with.x * y);
		return kComp;
	}

	float Vector2::GetMagnitude()
	{
		return std::sqrt((x * x) + (y * y));
	}

	void Vector2::Rotate(float degrees)
	{
		*this = Rotate(*this, degrees);
	}

	void Vector2::NormalizeSelf()
	{
		Normalize();
	}
}