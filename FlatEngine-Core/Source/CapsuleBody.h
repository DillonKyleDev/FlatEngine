#pragma once
#include "Body.h"

#include <string>


namespace FlatEngine
{
	class CapsuleBody : public Body
	{
	public:
		CapsuleBody(long myID = -1, long parentID = -1);
		~CapsuleBody();
		std::string GetData();

		void SetLength(float length);
		void SetRadius(float radius);

	private:

	};

}
