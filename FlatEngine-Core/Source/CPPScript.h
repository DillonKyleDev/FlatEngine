#pragma once
#include "GameObject.h"


namespace FlatEngine
{
	class CPPScript
	{
	public:
		CPPScript() {};
		~CPPScript() {};

		GameObject* GetParent();
		void SetParentID(long parentID);

		virtual void Awake() {};
		virtual void Start() {};
		virtual void Update() {};
		//virtual void OnBoxCollisionEnter() {};

	private:
		long m_parentID = -1;
	};
}
