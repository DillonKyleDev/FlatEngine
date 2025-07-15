#pragma once


namespace FlatEngine
{
	class CPPScript
	{
	public:
		CPPScript() {};
		~CPPScript() {};

		virtual void Awake() {};
		virtual void Start() {};
		virtual void Update() {};
	};
}
