#include "Matrix.h"


namespace FlatEngine
{
	Matrix::Matrix(int dimension)
	{
		m_dimension = dimension;
	}
	Matrix::~Matrix()
	{

	}






	Matrix2::Matrix2() : Matrix(2)
	{
		m_oneOne = 0;
		m_oneTwo = 0;
		m_twoOne = 0;
		m_twoTwo = 0;
	}
	Matrix2::~Matrix2()
	{
	}
	Matrix2::Matrix2(float oneOne, float oneTwo, float twoOne, float twoTwo) : Matrix(2)
	{
		m_oneOne = oneOne;
		m_oneTwo = oneTwo;
		m_twoOne = twoOne;
		m_twoTwo = twoTwo;
	}

	Vector2 Matrix2::LMultiply(Vector2 vec)
	{
		float rowOne;
		float rowTwo;

		rowOne = (m_oneOne * vec.x) + (m_oneTwo * vec.y);
		rowTwo = (m_twoOne * vec.x) + (m_twoTwo * vec.y);

		return Vector2(rowOne, rowTwo);
	}

	int Matrix2::GivesInt()
	{
		return 5;
	}

}