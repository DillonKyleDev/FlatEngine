#pragma once
#include "Body.h"

#include <string>


namespace FlatEngine
{
	class CircleBody : public Body
	{
	public:
		CircleBody(long myID = -1, long parentID = -1);
		~CircleBody();
		std::string GetData();

		void SetRadius(float radius);

	private:

	};
}

