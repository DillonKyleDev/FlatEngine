#include "components/Animation.h"
#include "components/Transform.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "managers/AnimationManager.h"
#include "tools/Easing.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"


namespace FlatEngine
{
    void TransformProp::Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData) 
    {
        Transform* transform = gameObject->Get<Transform>();
        Vector3 lastPos = Vector3(0);
        Vector3 lastScale = transform->GetScale();
        Vector3 lastRot = transform->GetRotations();
        TransformProp* last = static_cast<TransformProp*>(prev);
        lastPos = animData->startingPos;

        if (prev) 
        {					
            lastPos   = animData->startingPos + last->position;
            lastScale = last->scale;
            lastRot   = last->rotation;
        }

        Vector3 posFinal;
        Vector3 rotFinal;
        Vector3 scaleFinal;

        float posTime;
        float scaleTime;
        float rotTime;
        switch (positionInterpType)
        {
            case InterpType_Linear:           posTime = percentDone; break;
            case InterpType_EaseInSine:       posTime = Easing::EaseInSine(percentDone); break;
            case InterpType_EaseOutSine:      posTime = Easing::EaseOutSine(percentDone); break;
            case InterpType_EaseInOutSine:    posTime = Easing::EaseInOutSine(percentDone); break;
            case InterpType_EaseInElastic:    posTime = Easing::EaseInElastic(percentDone); break;
            case InterpType_EaseOutElastic:   posTime = Easing::EaseOutElastic(percentDone); break;
            case InterpType_EaseInOutElastic: posTime = Easing::EaseInOutElastic(percentDone); break;
            case InterpType_EaseInBack:       posTime = Easing::EaseInBack(percentDone); break;
            case InterpType_EaseOutBack:      posTime = Easing::EaseOutBack(percentDone); break;
            case InterpType_EaseInOutBack:    posTime = Easing::EaseInOutBack(percentDone); break;
            case InterpType_EaseInOutQuart:   posTime = Easing::EaseInOutQuart(percentDone); break;
            case InterpType_EaseInOutCubic:   posTime = Easing::EaseInOutCubic(percentDone); break;
            default: break;
        }
        switch (scaleInterpType)
        {
            case InterpType_Linear:           scaleTime = percentDone; break;
            case InterpType_EaseInSine:       scaleTime = Easing::EaseInSine(percentDone); break;
            case InterpType_EaseOutSine:      scaleTime = Easing::EaseOutSine(percentDone); break;
            case InterpType_EaseInOutSine:    scaleTime = Easing::EaseInOutSine(percentDone); break;
            case InterpType_EaseInElastic:    scaleTime = Easing::EaseInElastic(percentDone); break;
            case InterpType_EaseOutElastic:   scaleTime = Easing::EaseOutElastic(percentDone); break;
            case InterpType_EaseInOutElastic: scaleTime = Easing::EaseInOutElastic(percentDone); break;
            case InterpType_EaseInBack:       scaleTime = Easing::EaseInBack(percentDone); break;
            case InterpType_EaseOutBack:      scaleTime = Easing::EaseOutBack(percentDone); break;
            case InterpType_EaseInOutBack:    scaleTime = Easing::EaseInOutBack(percentDone); break;
            case InterpType_EaseInOutQuart:   scaleTime = Easing::EaseInOutQuart(percentDone); break;
            case InterpType_EaseInOutCubic:   scaleTime = Easing::EaseInOutCubic(percentDone); break;
            default: break;
        }
        switch (rotationInterpType)
        {
            case InterpType_Linear:           rotTime = percentDone; break;
            case InterpType_EaseInSine:       rotTime = Easing::EaseInSine(percentDone); break;
            case InterpType_EaseOutSine:      rotTime = Easing::EaseOutSine(percentDone); break;
            case InterpType_EaseInOutSine:    rotTime = Easing::EaseInOutSine(percentDone); break;
            case InterpType_EaseInElastic:    rotTime = Easing::EaseInElastic(percentDone); break;
            case InterpType_EaseOutElastic:   rotTime = Easing::EaseOutElastic(percentDone); break;
            case InterpType_EaseInOutElastic: rotTime = Easing::EaseInOutElastic(percentDone); break;
            case InterpType_EaseInBack:       rotTime = Easing::EaseInBack(percentDone); break;
            case InterpType_EaseOutBack:      rotTime = Easing::EaseOutBack(percentDone); break;
            case InterpType_EaseInOutBack:    rotTime = Easing::EaseInOutBack(percentDone); break;
            case InterpType_EaseInOutQuart:   rotTime = Easing::EaseInOutQuart(percentDone); break;
            case InterpType_EaseInOutCubic:   rotTime = Easing::EaseInOutCubic(percentDone); break;
            default: break;
        }

        posFinal = lastPos + (animData->startingPos + position - lastPos) * posTime;
        rotFinal = lastRot + (rotation - lastRot) * rotTime;
        scaleFinal = lastScale + (scale - lastScale) * scaleTime;

        if (!b_fired)
        {
            if (b_posAnimated) 
            {
                transform->SetPosition(posFinal);
            }
            if (b_rotationAnimated)
            {
                transform->SetRotation(rotFinal);
            }
            if (b_scaleAnimated)
            {
                transform->SetScale(scaleFinal);
            }

            if (time == 0)
            {
                b_fired = true;
            }
        }
    }


    Animation::Animation(long myID, long parentObjectID)
    {
        SetType(ComponentType_Animation);
        SetID(myID);
        SetParentObjectID(parentObjectID);
        m_animations = std::vector<AnimationData>();
    }

    json Animation::GetData()
    {
        json animationData = json::array();

        for (AnimationData& animData : m_animations)
        {
            json animationJson = {
                { "path", animData.path },
                { "name", animData.name },
            };

            animationData.push_back(animationJson);
        }

        json jsonData = {
            { "type", (int)GetType() },
            { "id", GetID() },
            { "b_isCollapsed", IsCollapsed() },
            { "b_isActive", IsActive() },
            { "animationData", animationData }
        };

        return jsonData;
    }

    void Animation::PutData(json componentJson, std::string objectName)
	{
		SetID(JsonHelper::CheckJsonLong(componentJson, "id", objectName));
		SetActive(JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName));
		SetCollapsed(JsonHelper::CheckJsonBool(componentJson, "b_isCollapsed", objectName));

        if (JsonHelper::JsonContains(componentJson, "animationData", objectName))
        {
            for (int anim = 0; anim < componentJson.at("animationData").size(); anim++)
            {
                try
                {
                    json animationJson = componentJson.at("animationData").at(anim);
                    std::string path = JsonHelper::CheckJsonString(animationJson, "path", objectName);
                    std::string animationName = JsonHelper::CheckJsonString(animationJson, "name", objectName);
                    if (!FileHelper::DoesFileExist(path))
                    {
                        Logger::log.Err("Animation file not found for GameObject: {} - on Animation: {}. This may lead to unexpected behavior.  \npath: {}", objectName, animationName, path);
                    }
                    AddAnimation(animationName, path);
                }
                catch (const json::out_of_range& e)
                {
                    Logger::log.Err("{}", e.what());
                }
            }
        }
    }

    void Animation::AddAnimation(std::string name, std::string filePath)
    {
        AnimationData animData;
        animData.name = name;
        animData.path = filePath;
        animData.startTime = 0;
        animData.b_playing = false;

        // Add a RemoveAnimation() function instead of this...
        for (std::vector<AnimationData>::iterator iter = m_animations.begin(); iter != m_animations.end(); iter++)
        {
            if (iter->name == name)
            {
                m_animations.erase(iter);
                break;
            }
        }

        m_animations.push_back(std::move(animData));
    }

    bool Animation::ContainsName(std::string name)
    {
        for (AnimationData& animData : m_animations)
        {
            if (animData.name == name)
            {
                return true;
            }
        }
        return false;
    }

    std::vector<AnimationData>& Animation::GetAnimations()
    {
        return m_animations;
    }

    void Animation::Play(std::string animationName, Uint32 startTime)
    {
        for (AnimationData &animData : m_animations)
        {
            if (animData.name == animationName)
            {
                animData = AnimationManager::LoadAnimationFile(animData.path);
                animData.b_playing = true;				
                if (!animData.b_startAtOrigin)
                {
                    animData.startingPos = GetParentObject()->Get<Transform>()->GetPosition();
                }
                else 
                {
                    animData.startingPos = Vector3(0);
                }

                if (startTime != 0)
                {
                    animData.startTime = startTime;
                }
                else
                {
                    animData.startTime = GetElapsedGameTimeInMs();
                }
            }
        }
    }

    void Animation::PlayFromLua(std::string animationName)
    {	
        Play(animationName);
    }

    void Animation::StopAll()
    {
        for (AnimationData& animData : m_animations)
        {
            animData.b_playing = false;
        }
    }

    void Animation::Stop(std::string animationName)
    {
        for (AnimationData& animData : m_animations)
        {
            if (animData.name == animationName)
            {
                animData.b_playing = false;
            }
        }
    }

    bool Animation::IsPlaying(std::string animationName)
    {
        for (AnimationData& animData : m_animations)
        {
            if (animData.name == animationName)
            {
                return animData.b_playing;
            }
        }

        return false;
    }

    bool Animation::HasAnimation(std::string animationName)
    {
        for (AnimationData& animData : m_animations)
        {
            if (animData.name == animationName)
            {
                return true;
            }
        }
        return false;
    }

    void Animation::PlayAnimations(Uint32 elapsedTime)
    {
        for (AnimationData& animData : m_animations)
        {
            if (animData.b_playing)
            {
                PlayAnimation(animData.name, elapsedTime);
            }
        }
    }

    void Animation::PlayAnimation(std::string animationName, Uint32 elapsedTime)
    {
        for (AnimationData& animData : m_animations)
        {
            if (animData.name == animationName)
            {
                if (!animData.b_isSorted)
                {
                    animData.SortFrames();
                }

                // While the animation is not over (with a 1 second buffer to catch the last animation keyframes)
                if (animData.length > elapsedTime - animData.startTime)
                {
                    std::vector<std::shared_ptr<AnimationProperty>>& keyframes = animData.props;

                    for (int i = 0; i < keyframes.size(); i++) 
                    {
                        auto& frame = keyframes[i];
                        AnimationProperty* prev = (i > 0) ? keyframes[i-1].get() : nullptr;

                        float timeLeft = animData.startTime + frame->time - elapsedTime;
                        float percentDone = prev == nullptr ? 0 : (float)(elapsedTime - animData.startTime - prev->time) / (frame->time - prev->time);
                        bool b_shouldApply = (frame->time == 0 && !frame->b_fired) || ((prev != nullptr) && (elapsedTime > prev->time + animData.startTime) && (elapsedTime < animData.startTime + frame->time));

                        if (b_shouldApply) 
                        {
                            frame->Apply(GetParentObject(), percentDone, prev, &animData);
                        }
                    }



                    // Transform Animation Frames
                    // int transformFrameCounter = 0;
                    // for (std::vector<std::shared_ptr<TransformProp>>::iterator frame = props->transformProps.begin(); frame != props->transformProps.end(); frame++)
                    // {
                    // 	static Vector2 lastFramePosition = Vector2(0, 0);
                    // 	static Vector2 lastFrameScale = Vector2(0, 0);
                    // 	static float lastFrameRotation = 0.0f;
                    // 	float keyframeTime = (*frame)->time;
                    // 	Transform* transform = GetParent()->Get<Transform>();
                    // 	std::shared_ptr<TransformProp> thisFrameProps = (*frame);
                    // 	std::vector<std::shared_ptr<TransformProp>>::iterator lastFrame = frame;
                    // 	bool b_posAnimated = thisFrameProps->b_posAnimated;
                    // 	bool b_scaleAnimated = thisFrameProps->b_scaleAnimated;
                    // 	bool b_rotationAnimated = thisFrameProps->b_rotationAnimated;
                    // 	float lastFrameTime = 0;
                    // 	if (transformFrameCounter > 0 && props->transformProps.size() > 1)
                    // 	{
                    // 		lastFrame--;
                    // 		lastFrameTime = (*lastFrame)->time;
                    // 	}

                    // 	if (keyframeTime == 0 && !(*frame)->b_fired)
                    // 	{
                    // 		if (b_posAnimated)
                    // 		{
                    // 			transform->SetPosition(Vector3(thisFrameProps->xPos, thisFrameProps->yPos, 1));
                    // 		}
                    // 		if (b_scaleAnimated)
                    // 		{
                    // 			transform->SetScale(Vector3(thisFrameProps->xScale, thisFrameProps->yScale, 1));
                    // 		}
                    // 		if (b_rotationAnimated)
                    // 		{
                    // 			transform->SetZRotation(thisFrameProps->rotation);
                    // 		}
                    // 		(*frame)->b_fired = true;
                    // 	}
                    // 	else if ((elapsedTime > lastFrameTime + animData.startTime) && (elapsedTime < animData.startTime + keyframeTime))
                    // 	{
                    // 		std::shared_ptr<TransformProp> lastFrameProps = (*lastFrame);
                    // 		float timeLeft = (animData.startTime + keyframeTime) - elapsedTime;
                    // 		float percentDone = (float)(elapsedTime - animData.startTime - lastFrameTime) / (keyframeTime - lastFrameTime);
                    // 		lastFramePosition = Vector2(lastFrameProps->xPos, lastFrameProps->yPos);
                    // 		lastFrameScale = Vector2(lastFrameProps->xScale, lastFrameProps->yScale);
                    // 		lastFrameRotation = lastFrameProps->rotation;

                    // 		switch (thisFrameProps->positionInterpType)
                    // 		{
                    // 		case InterpType_Linear:
                    // 		{
                    // 			if (b_posAnimated)
                    // 			{
                    // 				float correctedX = (lastFramePosition.x + (thisFrameProps->xPos - lastFramePosition.x) * percentDone);
                    // 				float correctedY = (lastFramePosition.y + (thisFrameProps->yPos - lastFramePosition.y) * percentDone);
                    // 				transform->SetPosition(Vector3(correctedX, correctedY, 1));
                    // 			}
                    // 			if (b_scaleAnimated)
                    // 			{
                    // 				float correctedXScale = (lastFrameScale.x + (thisFrameProps->xScale - lastFrameScale.x) * percentDone);
                    // 				float correctedYScale = (lastFrameScale.y + (thisFrameProps->yScale - lastFrameScale.y) * percentDone);
                    // 				if (correctedXScale != 0 && correctedYScale != 0)
                    // 				{
                    // 					transform->SetScale(Vector3(correctedXScale, correctedYScale, 1));
                    // 				}
                    // 			}
                    // 			if (b_rotationAnimated)
                    // 			{
                    // 				float correctedRotation = (lastFrameRotation + (thisFrameProps->rotation - lastFrameRotation) * percentDone);
                    // 				transform->SetZRotation(correctedRotation);
                    // 			}
                    // 			break;
                    // 		}
                    // 		default:
                    // 			break;
                    // 		}
                    // 	}
                    // 	transformFrameCounter++;
                    // }

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

                    // Audio Animation Frames
                    // if (GetParent()->GetAudio() != nullptr)
                    // {
                    // 	Audio* audio = GetParent()->GetAudio();
                    // 	int audioFrameCounter = 0;

                    // 	for (std::vector<std::shared_ptr<S_Audio>>::iterator frame = props->audioProps.begin(); frame != props->audioProps.end(); frame++)
                    // 	{
                    // 		float keyframeTime = (*frame)->time;
                    // 		std::shared_ptr<S_Audio> thisFrameProps = (*frame);
                    // 		std::vector<std::shared_ptr<S_Audio>>::iterator nextFrame = frame;
                    // 		bool b_stopsAllOtherSounds = thisFrameProps->b_stopAllOtherSounds;

                    // 		float nextFrameTime = 0;
                    // 		if (audioFrameCounter > 0 && props->audioProps.size() > 1)
                    // 		{
                    // 			nextFrame++;
                    // 			if (nextFrame != props->audioProps.end())
                    // 			{
                    // 				nextFrameTime = (*nextFrame)->time;
                    // 			}
                    // 		}

                    // 		if (keyframeTime == 0 && !(*frame)->b_fired)
                    // 		{
                    // 			if (b_stopsAllOtherSounds)
                    // 			{
                    // 				audio->StopAll();
                    // 			}

                    // 			audio->Play(thisFrameProps->soundName);
                    // 			thisFrameProps->b_fired = true;
                    // 		}
                    // 		else if (!thisFrameProps->b_fired && (elapsedTime > animData.startTime + keyframeTime))
                    // 		{
                    // 			if (b_stopsAllOtherSounds)
                    // 			{
                    // 				audio->StopAll();
                    // 			}
                    // 			audio->Play(thisFrameProps->soundName);
                    // 			thisFrameProps->b_fired = true;
                    // 		}
                    // 		audioFrameCounter++;
                    // 	}
                    // }

                    // // Event Animation Frames (Do last because if we delete the object, we don't want to do any other Animation things with it.)
                    // for (const std::shared_ptr<S_Event>& eventFrame : props->eventProps)
                    // {
                    // 	if ((eventFrame->time == 0 && !eventFrame->b_fired) || (!eventFrame->b_fired && (elapsedTime >= animData.startTime + eventFrame->time || eventFrame->time == 0)))
                    // 	{
                    // 		if (eventFrame->b_luaEvent)
                    // 		{
                    // 			CallLuaAnimationEventFunction(GetParent(), eventFrame->functionName, eventFrame->parameters);
                    // 		}
                    // 		else if (eventFrame->b_cppEvent)
                    // 		{
                    // 			if (cppAnimationEventFunctions.count(eventFrame->functionName))
                    // 			{
                    // 				cppAnimationEventFunctions.at(eventFrame->functionName)(GetParent(), eventFrame->parameters);
                    // 			}
                    // 		}

                    // 		eventFrame->b_fired = true;							
                    // 	}
                    // }
                }
                else if (animData.b_loop)
                {					
                    animData.startTime = elapsedTime;

                    // Unfire Animation Frames
                    for (auto& frame : animData.props)
                    {
                        frame->b_fired = false;
                    }		
                }
                else
                {
                    Stop(animationName);
                }
            }
        }
    }

}