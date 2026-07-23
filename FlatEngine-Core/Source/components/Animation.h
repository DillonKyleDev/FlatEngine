#pragma once
#include "components/Component.h"
#include "managers/LuaManager.h"
#include "tools/JsonHelper.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"

#include <memory>
#include <SDL_syswm.h> // Uint32
#include <string>
#include <vector>

namespace FL = FlatEngine;


namespace FlatEngine
{
    enum PropertyType {
        PropType_None,
        PropType_Event,
        PropType_Transform,
        PropType_Sprite,
        PropType_Camera,
        PropType_Script,
        PropType_Button,
        PropType_Canvas,
        PropType_Audio,
        PropType_Text,
        PropType_CharacterController
    };

    enum InterpType {
        InterpType_Linear,
        InterpType_EaseInSine,
        InterpType_EaseOutSine,
        InterpType_EaseInOutSine,
        InterpType_EaseInElastic,
        InterpType_EaseOutElastic,
        InterpType_EaseInOutElastic,
        InterpType_EaseInBack,
        InterpType_EaseOutBack,
        InterpType_EaseInOutBack,
        InterpType_EaseInOutQuart,
        InterpType_EaseInOutCubic
    };

    struct AnimationData;
    struct AnimationProperty {
        PropertyType type = PropType_None;
        float time = 0;
        std::string name;
        bool b_fired;
        virtual ~AnimationProperty() = default;
        virtual void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData) = 0;
    };	
    struct EventProp : public AnimationProperty {			
        bool b_cppEvent = false;
        bool b_luaEvent = false;
        std::string functionName = "";
        LuaManager::LuaParameter parameters = LuaManager::LuaParameter();
        //void SetParameters(std::vector<LuaParameter> newParameters) { parameters = newParameters; };
        //void AddParameter(LuaParameter eventParam) { parameters.push_back(eventParam); };
        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {

        }
    };
    struct TransformProp : public AnimationProperty {
        InterpType positionInterpType = InterpType_Linear;			
        InterpType scaleInterpType = InterpType_Linear;			
        InterpType rotationInterpType = InterpType_Linear;
        FL::Vector3 position;
        FL::Vector3 rotation;
        FL::Vector3 scale = FL::Vector3(1);						

        bool b_posAnimated = false;
        bool b_scaleAnimated = false;
        bool b_rotationAnimated = false;

        TransformProp()
        {
            type = PropType_Transform;
            name = "Transform";
        }

        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData);
    };
    struct SpriteProp : public AnimationProperty {
        InterpType interpType = InterpType_Linear;
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

        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {

        }
    };
    struct CameraProp : public AnimationProperty {
        bool b_isPrimaryCamera = false;
        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {

        }
    };
    struct ScriptProp : public AnimationProperty {
        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {

        }
        std::string path = "";
    };
    struct ButtonProp : public AnimationProperty {
        bool b_isActive = true;
        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {

        }
    };
    struct CanvasProp : public AnimationProperty {
        bool b_isActive = true;
        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {

        }
    };
    struct AudioProp : public AnimationProperty {
        std::string soundName = "";	
        bool b_stopAllOtherSounds = false;
        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {

        }
    };
    struct TextProp : public AnimationProperty {
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
        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {

        }
    };
    struct CharacterControllerProp : public AnimationProperty {
        bool b_isActive = true;
        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {

        }
    };

    struct AnimationData {			
        std::string name = "";
        std::string path = "";
        bool b_playing = false;
        Uint32 startTime = 0;
        float length = 0.0f;
        bool b_isSorted = false;
        bool b_loop = false;
        bool b_startAtOrigin = false;
        FL::Vector3 startingPos = FL::Vector3();	

        std::vector<std::shared_ptr<AnimationProperty>> props;

        static bool CompareTime(std::shared_ptr<AnimationProperty>& frame1, std::shared_ptr<AnimationProperty>& frame2)
        {
            return frame1->time < frame2->time;
        }
        void SortFrames()
        {
            std::sort(props.begin(), props.end(), CompareTime);
            b_isSorted = true;
            
            float endTime = 0.0f;
            
            if (props.size() > 0 && props.back()->time > endTime)
            {
                endTime = props.back()->time;
            }

            length = endTime;
        }
        void RemoveKeyFrame(AnimationProperty* property)
        {
            for (auto iter = props.begin(); iter != props.end(); iter++)
            {
                if (iter->get() == property)
                {
                    props.erase(iter);
                    return;
                }
            }
        }
    };

    class Animation : public Component
    {
    public:
        Animation(long myID = -1, long parentObjectID = -1);
        std::string GetData();
        void PutData(json componentJson);

        void AddAnimation(std::string name, std::string filePath);
        bool ContainsName(std::string name);
        std::vector<AnimationData> &GetAnimations();
        void Play(std::string animationName, Uint32 startTime = 0);
        void PlayFromLua(std::string animationName);
        void Stop(std::string animationName);
        void StopAll();
        void PlayAnimation(std::string animationName, Uint32 elapsedTime);
        void PlayAnimations(Uint32 elapsedTime);
        bool IsPlaying(std::string animationName);
        bool HasAnimation(std::string animationName);
    
    private:	
        std::vector<AnimationData> m_animations;
    };
}