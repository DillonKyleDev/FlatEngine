#pragma once
#include "components/Component.h"
#include "managers/LuaManager.h"
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
        PropertyType_None,
        PropertyType_Event,
        PropertyType_Transform,
        PropertyType_Sprite,
        PropertyType_Text,
        PropertyType_Size
    };

    const std::vector<std::string> PropertyTypeStrings =
    {
        "None",
        "Event",
        "Transform",
        "Sprite",
        "Text",
        "Size"
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
        PropertyType type = PropertyType_None;
        float time = 0;
        std::string name;
        bool b_fired = false;
        bool b_applyInstantly = false;
        virtual ~AnimationProperty() = default;
        virtual void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData) = 0;
        virtual json GetData() = 0;
        virtual void PutData(json jsonData, std::string name) = 0;
    };	
    struct EventProp : public AnimationProperty {		
        bool b_cppEvent = false;
        bool b_luaEvent = false;
        std::string functionName = "";
        LuaManager::LuaParameterContainer eventParamContainer;        

        EventProp()
        {
            type = PropertyType_Event;
            b_applyInstantly = true;	           
        }

        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {
            if (b_luaEvent)
            {
                CallLuaAnimationEventFunction(gameObject, functionName, eventParamContainer);
            }
            // else if (b_cppEvent)
            // {
            //     if (cppAnimationEventFunctions.count(functionName))
            //     {
            //         cppAnimationEventFunctions.at(functionName)(gameObject, eventParamContainer.parameters);
            //     }
            // }

            b_fired = true;							            
        }
        json GetData()
        { 
            json jsonData = {
                { "type", (int)PropertyType_Event },
                { "functionName", functionName },
                { "time", time },
                { "b_cppEvent", b_cppEvent },
                { "b_luaEvent", b_luaEvent }
            }; 

            json parametersJson = json::array();

            for (auto& param : eventParamContainer.parameters)
            {
                parametersJson.push_back(param.second.GetData());
            }

            jsonData.emplace("parameters", parametersJson);

            return jsonData;
        }
        void PutData(json jsonData, std::string name)
        {
            functionName = JsonHelper::CheckJsonString(jsonData, "functionName", name);
            time = JsonHelper::CheckJsonFloat(jsonData, "time", name) != -1 ? JsonHelper::CheckJsonFloat(jsonData, "time", name) : 0.0;
            b_cppEvent = JsonHelper::CheckJsonBool(jsonData, "b_cppEvent", name);
            b_luaEvent = JsonHelper::CheckJsonBool(jsonData, "b_luaEvent", name);

            json parametersJson = jsonData["parameters"];

            for (json paramJson : parametersJson)
            {
                LuaManager::LuaParameter parameter;
                parameter.PutData(paramJson, name);
                eventParamContainer.Add(parameter);
            }
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
            type = PropertyType_Transform;            
        }

        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData);
        json GetData()
        {
            json jsonData = {
                { "type", (int)PropertyType_Transform },
                { "positionInterpType", (int)positionInterpType },
                { "scaleInterpType", (int)scaleInterpType },
                { "rotationInterpType", (int)rotationInterpType },											
                { "time", time },		
                { "xPosition", position.x },
                { "yPosition", position.y },
                { "zPosition", position.z },
                { "xRotation", rotation.x },
                { "yRotation", rotation.y },
                { "zRotation", rotation.z },
                { "xScale", scale.x },
                { "yScale", scale.y },
                { "zScale", scale.z },
                { "b_posAnimated", b_posAnimated },
                { "b_scaleAnimated", b_scaleAnimated },
                { "b_rotationAnimated", b_rotationAnimated }
            };

            return jsonData;
        }
        void PutData(json jsonData, std::string name)
        {        				
            positionInterpType = (InterpType)JsonHelper::CheckJsonInt(jsonData, "positionInterpType", name);
            scaleInterpType = (InterpType)JsonHelper::CheckJsonInt(jsonData, "scaleInterpType", name);
            rotationInterpType = (InterpType)JsonHelper::CheckJsonInt(jsonData, "rotationInterpType", name);						
            time = JsonHelper::CheckJsonFloat(jsonData, "time", name) != -1 ? JsonHelper::CheckJsonFloat(jsonData, "time", name) : 0.0;
            position = Vector3(JsonHelper::CheckJsonFloat(jsonData, "xPosition", name),JsonHelper::CheckJsonFloat(jsonData, "yPosition", name), JsonHelper::CheckJsonFloat(jsonData, "zPosition", name));
            rotation = Vector3(JsonHelper::CheckJsonFloat(jsonData, "xRotation", name), JsonHelper::CheckJsonFloat(jsonData, "yRotation", name), JsonHelper::CheckJsonFloat(jsonData, "zRotation", name));
            scale = Vector3(JsonHelper::CheckJsonFloat(jsonData, "xScale", name), JsonHelper::CheckJsonFloat(jsonData, "yScale", name), JsonHelper::CheckJsonFloat(jsonData, "zScale", name));
            b_posAnimated = JsonHelper::CheckJsonBool(jsonData, "b_posAnimated", name);
            b_scaleAnimated = JsonHelper::CheckJsonBool(jsonData, "b_scaleAnimated", name);
            b_rotationAnimated = JsonHelper::CheckJsonBool(jsonData, "b_rotationAnimated", name);                
        }
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

        SpriteProp()
        {
            type = PropertyType_Sprite;	           
        }

        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {
            // Sprite Animation Frames
            // if (GetParent()->GetSprite() != nullptr)
            // {
            // 	Sprite* sprite = GetParent()->GetSprite();
            // 	static Vector4 lastFrameSpriteTint = Vector4(1, 1, 1, 1);
            // 	static Vector2 lastFrameOffset = Vector2(0, 0);
            // 	static Vector2 lastFrameScale = Vector2(1, 1);

            // 	int spriteFrameCounter = 0;
            // 	for (std::vector<std::shared_ptr<S_Sprite>>::iterator frame = props->spriteProps.begin(); frame != props->spriteProps.end(); frame++)
            // 	{
            // 		float keyframeTime = (*frame)->time;
            // 		std::shared_ptr<S_Sprite> thisFrameProps = (*frame);
            // 		std::vector<std::shared_ptr<S_Sprite>>::iterator lastFrame = frame;
            // 		bool b_pathAnimated = thisFrameProps->b_pathAnimated;
            // 		bool b_scaleAnimated = thisFrameProps->b_scaleAnimated;
            // 		bool b_offsetAnimated = thisFrameProps->b_offsetAnimated;
            // 		bool b_tintColorAnimated = thisFrameProps->b_tintColorAnimated;							

            // 		float lastFrameTime = 0;
            // 		if (spriteFrameCounter > 0 && props->spriteProps.size() > 1)
            // 		{
            // 			lastFrame--;
            // 			lastFrameTime = (*lastFrame)->time;
            // 		}

            // 		if (keyframeTime == 0 && !(*frame)->b_fired)
            // 		{			
            // 			if (b_pathAnimated && !thisFrameProps->b_fired)
            // 			{
            // 				sprite->SetTexture(thisFrameProps->path);
            // 			}
            // 			if (b_offsetAnimated)
            // 			{
            // 				sprite->SetOffset(Vector2(thisFrameProps->xOffset, thisFrameProps->yOffset));
            // 			}
            // 			if (b_scaleAnimated)
            // 			{
            // 				sprite->SetScale(Vector2(thisFrameProps->xScale, thisFrameProps->yScale));
            // 			}
            // 			if (b_tintColorAnimated)
            // 			{
            // 				sprite->SetTintColor(thisFrameProps->tintColor);
            // 			}
            // 			sprite->UpdatePivotOffset();
            // 			thisFrameProps->b_fired = true;
            // 		}
            // 		else if ((elapsedTime > lastFrameTime + animData.startTime) && (elapsedTime < animData.startTime + keyframeTime))
            // 		{								
            // 			std::shared_ptr<S_Sprite> lastFrameProps = (*lastFrame);
            // 			float timeLeft = (animData.startTime + keyframeTime) - elapsedTime;
            // 			float percentDone = (float)(elapsedTime - animData.startTime - lastFrameTime) / (keyframeTime - lastFrameTime);

            // 			if (b_pathAnimated && !thisFrameProps->b_fired && thisFrameProps->path != "")
            // 			{
            // 				//LogString("Different frame: " + thisFrameProps->path);
            // 				sprite->SetTexture(thisFrameProps->path);
            // 				thisFrameProps->b_fired = true;
            // 			}

            // 			switch (thisFrameProps->interpType)
            // 			{
            // 			case InterpType_Linear:
            // 			{
            // 				if (b_tintColorAnimated)
            // 				{
            // 					Vector4 correctedTintColor = thisFrameProps->tintColor;
            // 					lastFrameSpriteTint = (*lastFrame)->tintColor;
            // 					if (!thisFrameProps->b_instantTintChange)
            // 					{
            // 						correctedTintColor = Vector4(lastFrameSpriteTint.x + (thisFrameProps->tintColor.x - lastFrameSpriteTint.x) * percentDone,
            // 							lastFrameSpriteTint.y + (thisFrameProps->tintColor.y - lastFrameSpriteTint.y) * percentDone,
            // 							lastFrameSpriteTint.z + (thisFrameProps->tintColor.z - lastFrameSpriteTint.z) * percentDone,
            // 							lastFrameSpriteTint.w + (thisFrameProps->tintColor.w - lastFrameSpriteTint.w) * percentDone);
            // 					}
            // 					sprite->SetTintColor(correctedTintColor);
            // 				}
            // 				if (b_scaleAnimated)
            // 				{
            // 					lastFrameScale = Vector2(lastFrameProps->xScale, lastFrameProps->yScale);
            // 					float correctedXScale = (lastFrameScale.x + (thisFrameProps->xScale - lastFrameScale.x) * percentDone);
            // 					float correctedYScale = (lastFrameScale.y + (thisFrameProps->yScale - lastFrameScale.y) * percentDone);
            // 					if (correctedXScale != 0 && correctedYScale != 0)
            // 					{
            // 						sprite->SetScale(Vector2(correctedXScale, correctedYScale));
            // 					}
            // 				}
            // 				if (b_offsetAnimated)
            // 				{
            // 					lastFrameOffset = Vector2(lastFrameProps->xOffset, lastFrameProps->yOffset);
            // 					sprite->SetOffset(Vector2(thisFrameProps->xOffset, thisFrameProps->yOffset));
            // 				}
            // 				break;
            // 			}
            // 			default:
            // 				break;
            // 			}
            // 		}
            // 		spriteFrameCounter++;
            // 	}
            // }
        }
        json GetData()
        {
            json jsonData = {
                { "type", (int)PropertyType_Sprite },
                { "interpType", interpType },
                { "speed", speed },
                { "time", time },
                { "path", path },
                { "xOffset", xOffset },
                { "yOffset", yOffset },
                { "tintColorX", tintColor.x },
                { "tintColorY", tintColor.y },
                { "tintColorZ", tintColor.z },
                { "tintColorW", tintColor.w },
                { "b_instantTintChange", b_instantTintChange },
                { "b_pathAnimated", b_pathAnimated },
                { "b_offsetAnimated", b_offsetAnimated },
                { "b_scaleAnimated", b_scaleAnimated },
                { "b_tintColorAnimated", b_tintColorAnimated }
            };

            return jsonData;
        }
        void PutData(json jsonData, std::string name)
        {
    		interpType = (InterpType)JsonHelper::CheckJsonInt(jsonData, "interpType", name);
    		speed = JsonHelper::CheckJsonFloat(jsonData, "speed", name);
    		time = JsonHelper::CheckJsonFloat(jsonData, "time", name) != -1 ? JsonHelper::CheckJsonFloat(jsonData, "time", name) : 0.0;
    		xOffset = JsonHelper::CheckJsonFloat(jsonData, "xOffset", name);
    		yOffset = JsonHelper::CheckJsonFloat(jsonData, "yOffset", name);
    		path = JsonHelper::CheckJsonString(jsonData, "path", name);
    		b_instantTintChange = JsonHelper::CheckJsonBool(jsonData, "b_instantTintChange", name);
    		tintColor = Vector4(
    			JsonHelper::CheckJsonFloat(jsonData, "tintColorX", name),
    			JsonHelper::CheckJsonFloat(jsonData, "tintColorY", name),
    			JsonHelper::CheckJsonFloat(jsonData, "tintColorZ", name),
    			JsonHelper::CheckJsonFloat(jsonData, "tintColorW", name)
    		);
    		b_pathAnimated = JsonHelper::CheckJsonBool(jsonData, "b_pathAnimated", name);
    		b_scaleAnimated = JsonHelper::CheckJsonBool(jsonData, "b_scaleAnimated", name);
    		b_offsetAnimated = JsonHelper::CheckJsonBool(jsonData, "b_offsetAnimated", name);
    		b_tintColorAnimated = JsonHelper::CheckJsonBool(jsonData, "b_tintColorAnimated", name);
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

        TextProp()
        {
            type = PropertyType_Text;	           
        }
        
        void Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData)
        {
            // Text Animation Frames
            // if (GetParent()->GetText() != nullptr)
            // {
            // 	Text* text = GetParent()->GetText();
            // 	static Vector4 lastFrameTextTint = Vector4(1, 1, 1, 1);
            // 	static Vector2 lastFrameOffset = Vector2(0, 0);
            // 	int textFrameCounter = 0;

            // 	for (std::vector<std::shared_ptr<S_Text>>::iterator frame = props->textProps.begin(); frame != props->textProps.end(); frame++)
            // 	{
            // 		float keyframeTime = (*frame)->time;
            // 		std::shared_ptr<S_Text> thisFrameProps = (*frame);
            // 		std::vector<std::shared_ptr<S_Text>>::iterator lastFrame = frame;
            // 		bool b_fontPathAnimated = thisFrameProps->b_fontPathAnimated;
            // 		bool b_textAnimated = thisFrameProps->b_textAnimated;
            // 		bool b_tintColorAnimated = thisFrameProps->b_tintColorAnimated;
            // 		bool b_offsetAnimated = thisFrameProps->b_offsetAnimated;

            // 		float lastFrameTime = 0;
            // 		if (textFrameCounter > 0 && props->textProps.size() > 1)
            // 		{
            // 			lastFrame--;
            // 			lastFrameTime = (*lastFrame)->time;
            // 		}

            // 		if (keyframeTime == 0 && !(*frame)->b_fired)
            // 		{
            // 			if (thisFrameProps->b_textAnimated)
            // 			{
            // 				text->SetText(thisFrameProps->text);
            // 			}
            // 			if (b_offsetAnimated)
            // 			{
            // 				text->SetOffset(Vector2(thisFrameProps->xOffset, thisFrameProps->yOffset));
            // 			}
            // 			if (b_fontPathAnimated && thisFrameProps->fontPath != "")
            // 			{
            // 				text->SetFontPath(thisFrameProps->fontPath);
            // 			}

            // 			if (b_tintColorAnimated)
            // 			{
            // 				text->SetColor(thisFrameProps->tintColor);
            // 			}
            // 			thisFrameProps->b_fired = true;
            // 		}
            // 		else if ((elapsedTime > lastFrameTime + animData.startTime) && (elapsedTime < animData.startTime + keyframeTime))
            // 		{
            // 			std::shared_ptr<S_Text> lastFrameProps = (*lastFrame);
            // 			float timeLeft = (animData.startTime + keyframeTime) - elapsedTime;
            // 			float percentDone = (float)(elapsedTime - animData.startTime - lastFrameTime) / (keyframeTime - lastFrameTime);

            // 			if (!thisFrameProps->b_fired && b_textAnimated)
            // 			{
            // 				text->SetText(thisFrameProps->text);
            // 				text->LoadText();
            // 				thisFrameProps->b_fired = true;
            // 			}
            // 			if (b_tintColorAnimated)
            // 			{
            // 				Vector4 correctedTintColor = thisFrameProps->tintColor;
            // 				lastFrameTextTint = (*lastFrame)->tintColor;
            // 				if (!thisFrameProps->b_instantTintChange)
            // 				{
            // 					correctedTintColor = Vector4(lastFrameTextTint.x + (thisFrameProps->tintColor.x - lastFrameTextTint.x) * percentDone,
            // 						lastFrameTextTint.y + (thisFrameProps->tintColor.y - lastFrameTextTint.y) * percentDone,
            // 						lastFrameTextTint.z + (thisFrameProps->tintColor.z - lastFrameTextTint.z) * percentDone,
            // 						lastFrameTextTint.w + (thisFrameProps->tintColor.w - lastFrameTextTint.w) * percentDone);
            // 				}
            // 				text->SetColor(correctedTintColor);
            // 			}
            // 			if (b_fontPathAnimated && thisFrameProps->fontPath != "")
            // 			{
            // 				text->SetFontPath(thisFrameProps->fontPath);
            // 			}
            // 			if (b_offsetAnimated)
            // 			{
            // 				lastFrameOffset = Vector2(lastFrameProps->xOffset, lastFrameProps->yOffset);
            // 				text->SetOffset(Vector2(thisFrameProps->xOffset, thisFrameProps->yOffset));
            // 			}
            // 		}
            // 		textFrameCounter++;
            // 	}
            // }
        }
        json GetData()
        {
            json jsonData = {
                { "type", (int)PropertyType_Text },
                { "time", time },
                { "fontPath", fontPath },
                { "text", text },
                { "xOffset", xOffset },
                { "yOffset", yOffset },
                { "tintColorX", tintColor.x },
                { "tintColorY", tintColor.y },
                { "tintColorZ", tintColor.z },
                { "tintColorW", tintColor.w },
                { "b_instantTintChange", b_instantTintChange },
                { "b_fontPathAnimated", b_fontPathAnimated },
                { "b_textAnimated", b_textAnimated },
                { "b_tintColorAnimated", b_tintColorAnimated },
                { "b_offsetAnimated", b_offsetAnimated }
            };

            return jsonData;
        }
        void PutData(json jsonData, std::string name)
        {            
    		time = JsonHelper::CheckJsonFloat(jsonData, "time", name) != -1 ? JsonHelper::CheckJsonFloat(jsonData, "time", name) : 0.0;
    		fontPath = JsonHelper::CheckJsonString(jsonData, "fontPath", name);
    		text = JsonHelper::CheckJsonString(jsonData, "text", name);
    		tintColor = Vector4(
    			JsonHelper::CheckJsonFloat(jsonData, "tintColorX", name),
    			JsonHelper::CheckJsonFloat(jsonData, "tintColorY", name),
    			JsonHelper::CheckJsonFloat(jsonData, "tintColorZ", name),
    			JsonHelper::CheckJsonFloat(jsonData, "tintColorW", name)
    		);
    		b_fontPathAnimated = JsonHelper::CheckJsonBool(jsonData, "b_fontPathAnimated", name);
    		b_textAnimated = JsonHelper::CheckJsonBool(jsonData, "b_textAnimated", name);
    		b_tintColorAnimated = JsonHelper::CheckJsonBool(jsonData, "b_tintColorAnimated", name);
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
        bool b_lastFrameFired = false;
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
        void Reset()
        {
            // Unfire Animation Frames
            for (auto& frame : props)
            {
                frame->b_fired = false;
            }	

            b_lastFrameFired = false;  
        }
        void AddKeyFrame(PropertyType type)
        {
            switch (type)
            {                                    
                case PropertyType_Event:     { std::shared_ptr<EventProp> prop = std::make_shared<EventProp>(); props.push_back(prop); break; }
                case PropertyType_Transform: { std::shared_ptr<TransformProp> prop = std::make_shared<TransformProp>(); props.push_back(prop); break; }
                case PropertyType_Sprite:    { std::shared_ptr<SpriteProp> prop = std::make_shared<SpriteProp>(); props.push_back(prop); break; }
                case PropertyType_Text:      { std::shared_ptr<TextProp> prop = std::make_shared<TextProp>(); props.push_back(prop); break; }
                default: break;                               
            }
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
        void RemoveAll(PropertyType type)
        {
            std::vector<std::shared_ptr<AnimationProperty>> trimmed;

            for (int i = 0; i < props.size(); i++)
            {
                if (props[i]->type != type)
                {
                    trimmed.push_back(props[i]);
                }
            }

            props = trimmed;
        }   
    };

    class Animation : public Component
    {
    public:
        Animation(long ownerID = -1);
        json GetData(bool b_IDOverride = false);
        void PutData(json componentJson, std::string objectName);

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