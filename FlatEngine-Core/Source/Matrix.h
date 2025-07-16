#pragma once
#include "Vector2.h"


namespace FlatEngine
{
	class Matrix
	{
	public:
		Matrix(int dimension);
		~Matrix();

		//virtual Matrix operator=(Matrix& toCopy);
		//virtual Matrix operator*(Matrix& right);
		//virtual Matrix operator+(const Matrix& right);
		//virtual Matrix operator-(const Matrix& right);
		//virtual bool operator==(const Matrix& right);
		//virtual bool operator!=(const Matrix& right);
		virtual int GivesInt() { return 0; };

	private:
		int m_dimension = 0;
	};

	class Matrix2 : public Matrix
	{
	public:
		Matrix2();
		~Matrix2();
		Matrix2(float oneOne, float oneTwo, float twoOne, float twoTwo);
		//Matrix2(const Matrix2& toCopy);

		Vector2 LMultiply(Vector2 vec);

		int GivesInt();

	private:
		float m_oneOne;
		float m_oneTwo;
		float m_twoOne;
		float m_twoTwo;
	};
}