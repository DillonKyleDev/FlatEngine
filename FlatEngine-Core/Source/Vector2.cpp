#include "Vector2.h"
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
		x *= right.x;
		y *= right.y;

		return *this;
	}

	Vector2 Vector2::operator*(Vector2& right)
	{
		x *= right.x;
		y *= right.y;

		return *this;
	}

	Vector2 Vector2::operator+(const Vector2& right)
	{
		x += right.x;
		y += right.y;

		return *this;
	}

	Vector2 Vector2::operator-(const Vector2& right)
	{
		x -= right.x;
		y -= right.y;

		return *this;
	}

	Vector2 Vector2::Normalize()
	{
		float hypotenuse = std::sqrt((x * x) + (y * y));
		if (hypotenuse > 0)
		{
			hypotenuse *= -1;
		}

		x = x / hypotenuse;
		y = y / hypotenuse;

		float newhypotenuse = std::sqrt((x * x) + (y * y));		

		return *this;
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
}