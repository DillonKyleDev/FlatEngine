#pragma once
#include "Body.h"
#include "Vector2.h"

#include <string>


namespace FlatEngine
{
	class BoxBody : public Body
	{
	public:
		BoxBody(long myID = -1, long parentID = -1);
		~BoxBody();
		std::string GetData();
		
		void SetDimensions(Vector2 dimensions);
		void UpdateCorners();
		Vector2* GetCornersWorld();
		std::vector<Vector2> GetCornersScreen();

	private:
		void SetCorners(Vector2 newCorners[4]);

		Vector2 m_corners[4];
	};
}
