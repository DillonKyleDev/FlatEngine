#pragma once
#include "Component.h"
#include "Vector2.h"
#include "Vector4.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <SDL_syswm.h> // Uint32


namespace FlatEngine
{
	class GameObject;

	class Animation : public Component
	{
	public:
		enum Properties {
			P_Transform,
			P_Sprite,
			P_Camera,
			P_Script,
			P_Button,
			P_Canvas,
			P_Audio,
			P_Text,
			P_CharacterController
		};

		enum InterpType {
			I_Lerp,
			I_Slerp
		};

		struct S_Property {
			float time = 0;
			std::string name;
			bool b_fired;
		};	
		struct S_EventFunctionParam {
			int e_int = 0;
			long e_long = 0;
			float e_float = 0.0f;
			double e_double = 0;		
			bool e_boolean = false;
			Vector2 e_Vector2 = Vector2(0, 0);
			std::string e_string = "";
			void SetString(std::string newString) { e_string = newString; };
			void SetInt(int newInt) { e_int = newInt; };
			void SetLong(long newLong) { e_long = newLong; };
			void SetFloat(float newFloat) { e_float = newFloat; };
			void SetDouble(double newDouble) { e_double = newDouble; };
			void SetBool(bool newBoolean) { e_boolean = newBoolean; };
			void SetVector2(Vector2 newVector2) { e_Vector2 = newVector2; };
			std::string GetString() { return e_string; };
			int GetInt() { return e_int; };
			long GetLong() { return e_long; };
			float GetFloat() { return e_float; };
			double GetDouble() { return e_double; };
			bool GetBool() { return e_boolean; };
			Vector2 GetVector2() { return e_Vector2; };
		};
		struct S_Event : public S_Property {
			bool b_cppEvent = false;
			bool b_luaEvent = false;
			std::string functionName = "";
			S_EventFunctionParam parameters = S_EventFunctionParam();
			//void SetParameters(std::vector<S_EventFunctionParam> newParameters) { parameters = newParameters; };
			//void AddParameter(S_EventFunctionParam eventParam) { parameters.push_back(eventParam); };
		};
		struct S_Transform : public S_Property {
			InterpType transformInterpType = I_Lerp;
			float transformSpeed = 0.1f;
			InterpType scaleInterpType = I_Lerp;
			float scaleSpeed = 0.1f;
			float xPos = 0;
			float yPos = 0;
			float xScale = 1;
			float yScale = 1;

			bool b_posAnimated = false;
			bool b_scaleAnimated = false;
		};
		struct S_Sprite : public S_Property {
			InterpType interpType = I_Lerp;
			float speed = 0.1f;
			std::string path = "";
			float xScale = 1;
			float yScale = 1;
			float xOffset = 0;
			float yOffset = 0;
			Vector4 tintColor = Vector4(1, 1, 1, 1);
			bool b_instantTintChange = false;
			bool b_pathAnimated = false;
			bool b_scaleAnimated = false;
			bool b_offsetAnimated = false;
			bool b_tintColorAnimated = false;
		};
		struct S_Camera : public S_Property {
			bool b_isPrimaryCamera = false;
		};
		struct S_Script : public S_Property {
			std::string path = "";
		};
		struct S_Button : public S_Property {
			bool b_isActive = true;
		};
		struct S_Canvas : public S_Property {
			bool b_isActive = true;
		};
		struct S_Audio : public S_Property {
			std::string soundName = "";	
			bool b_stopAllOtherSounds = false;
		};
		struct S_Text : public S_Property {
			std::string fontPath = "";
			std::string text = "";
			Vector4 tintColor = Vector4(1, 1, 1, 1);
			bool b_instantTintChange = false;
			float xOffset = 0;
			float yOffset = 0;
			bool b_fontPathAnimated = false;
			bool b_textAnimated = false;
			bool b_tintColorAnimated = false;
			bool b_offsetAnimated = false;
		};
		struct S_CharacterController : public S_Property {
			bool b_isActive = true;
		};

		struct S_AnimationProperties {
			std::string animationName = "";
			std::string animationPath = "";
			float animationLength = 0.0f;
			bool b_isSorted = false;
			bool b_loop = false;

			std::vector<std::shared_ptr<S_Event>> eventProps = std::vector< std::shared_ptr<S_Event>>();
			std::vector<std::shared_ptr<S_Transform>> transformProps = std::vector<std::shared_ptr<S_Transform>>();
			std::vector<std::shared_ptr<S_Sprite>> spriteProps = std::vector<std::shared_ptr<S_Sprite>>();
			std::vector<std::shared_ptr<S_Camera>> cameraProps = std::vector<std::shared_ptr<S_Camera>>();
			std::vector<std::shared_ptr<S_Canvas>> canvasProps = std::vector<std::shared_ptr<S_Canvas>>();
			std::vector<std::shared_ptr<S_Audio>> audioProps = std::vector<std::shared_ptr<S_Audio>>();
			std::vector<std::shared_ptr<S_Text>> textProps = std::vector<std::shared_ptr<S_Text>>();
			std::vector<std::shared_ptr<S_CharacterController>> characterControllerProps = std::vector<std::shared_ptr<S_CharacterController>>();

			static bool CompareTime(std::shared_ptr<S_Property> frame1, std::shared_ptr<S_Property> frame2)
			{
				return frame1->time < frame2->time;
			}
			void SortFrames()
			{
				std::sort(eventProps.begin(), eventProps.end(), CompareTime);
				std::sort(transformProps.begin(), transformProps.end(), CompareTime);
				std::sort(spriteProps.begin(), spriteProps.end(), CompareTime);
				std::sort(cameraProps.begin(), cameraProps.end(), CompareTime);
				std::sort(canvasProps.begin(), canvasProps.end(), CompareTime);
				std::sort(audioProps.begin(), audioProps.end(), CompareTime);
				std::sort(textProps.begin(), textProps.end(), CompareTime);
				std::sort(characterControllerProps.begin(), characterControllerProps.end(), CompareTime);

				b_isSorted = true;
				FindAnimationLength();
			}
			void FindAnimationLength() 
			{
				float endTime = 0.0f;
				
				if (eventProps.size() > 0 && eventProps.back()->time > endTime)
				{
					endTime = eventProps.back()->time;
				}
				if (transformProps.size() > 0 && transformProps.back()->time > endTime)
				{
					endTime = transformProps.back()->time;
				}
				if (spriteProps.size() > 0 && spriteProps.back()->time > endTime)
				{
					endTime = spriteProps.back()->time;
				}
				if (cameraProps.size() > 0 && cameraProps.back()->time > endTime)
				{
					endTime = cameraProps.back()->time;
				}
				if (canvasProps.size() > 0 && canvasProps.back()->time > endTime)
				{
					endTime = canvasProps.back()->time;
				}
				if (audioProps.size() > 0 && audioProps.back()->time > endTime)
				{
					endTime = audioProps.back()->time;
				}
				if (textProps.size() > 0 && textProps.back()->time > endTime)
				{
					endTime = textProps.back()->time;
				}
				if (characterControllerProps.size() > 0 && characterControllerProps.back()->time > endTime)
				{
					endTime = characterControllerProps.back()->time;
				}

				animationLength = endTime;
			};
			void RemoveKeyFrame(std::shared_ptr<Animation::S_Property> property)
			{
				std::string type = property->name;
				if (type == "Event")
				{
					for (std::vector<std::shared_ptr<Animation::S_Event>>::iterator prop = eventProps.begin(); prop != eventProps.end(); prop++)
					{
						if ((*prop) == property)
						{
							eventProps.erase(prop);
							return;
						}
					}
				}
				else if (type == "Transform")
				{
					for (std::vector<std::shared_ptr<Animation::S_Transform>>::iterator prop = transformProps.begin(); prop != transformProps.end(); prop++)
					{
						if ((*prop) == property)
						{
							transformProps.erase(prop);
							return;
						}
					}
				}
				else if (type == "Sprite")
				{
					for (std::vector<std::shared_ptr<Animation::S_Sprite>>::iterator prop = spriteProps.begin(); prop != spriteProps.end(); prop++)
					{
						if ((*prop) == property)
						{
							spriteProps.erase(prop);
							return;
						}
					}
				}
				else if (type == "Camera")
				{
					for (std::vector<std::shared_ptr<Animation::S_Camera>>::iterator prop = cameraProps.begin(); prop != cameraProps.end(); prop++)
					{
						if ((*prop) == property)
						{
							cameraProps.erase(prop);
							return;
						}
					}
				}
				else if (type == "Canvas")
				{
					for (std::vector<std::shared_ptr<Animation::S_Canvas>>::iterator prop = canvasProps.begin(); prop != canvasProps.end(); prop++)
					{
						if ((*prop) == property)
						{
							canvasProps.erase(prop);
							return;
						}
					}
				}
				else if (type == "Audio")
				{
					for (std::vector<std::shared_ptr<Animation::S_Audio>>::iterator prop = audioProps.begin(); prop != audioProps.end(); prop++)
					{
						if ((*prop) == property)
						{
							audioProps.erase(prop);
							return;
						}
					}
				}
				else if (type == "Text")
				{
					for (std::vector<std::shared_ptr<Animation::S_Text>>::iterator prop = textProps.begin(); prop != textProps.end(); prop++)
					{
						if ((*prop) == property)
						{
							textProps.erase(prop);
							return;
						}
					}
				}
				else if (type == "CharacterController")
				{
					for (std::vector<std::shared_ptr<Animation::S_CharacterController>>::iterator prop = characterControllerProps.begin(); prop != characterControllerProps.end(); prop++)
					{
						if ((*prop) == property)
						{
							characterControllerProps.erase(prop);
							return;
						}
					}
				}
			}
		};

		struct AnimationData {
			std::shared_ptr<S_AnimationProperties> animationProperties = nullptr;
			std::string name = "";
			std::string path = "";
			bool b_playing = false;
			Uint32 startTime = 0;
		};

		Animation(long myID = -1, long parentID = -1);
		~Animation();
		std::string GetData();

		void AddAnimation(std::string name, std::string filePath);
		bool ContainsName(std::string name);
		std::vector<AnimationData> &GetAnimations();
		void Play(std::string animationName, Uint32 startTime = 0);
		void PlayFromLua(std::string animationName);
		void Stop(std::string animationName);
		void StopAll();
		void PlayAnimation(std::string animationName, Uint32 ellapsedTime);
		bool IsPlaying(std::string animationName);
		bool HasAnimation(std::string animationName);
	
	private:	
		std::vector<AnimationData> m_animations;
	};
}

