#include "components/Animation.h"
#include "components/Transform.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "managers/AnimationManager.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"
#include "tools/Quaternion.h"


namespace FlatEngine
{
    void TransformProp::Apply(FL::GameObject* gameObject, float percentDone, AnimationProperty* prev, AnimationData* animData) 
    {
        TransformProp* last = static_cast<TransformProp*>(prev);
        Transform* transform = gameObject->Get<Transform>();
        Vector3 lastPos    = prev ? animData->startingPos + last->position : animData->startingPos;
        Vector3 lastScale  = prev ? last->scale : transform->GetScale();
        Quaternion lastRot = prev ? Quaternion::EulerToQuaternion(last->eulerRotation) : Quaternion::EulerToQuaternion(transform->GetRotation());
        Quaternion rot = Quaternion::EulerToQuaternion(eulerRotation);
        
        Vector3 rotFinal = Quaternion::QuaternionToEuler(Quaternion::Slerp(lastRot, rot, Easing::GetT(rotationInterpType, percentDone)));
        Vector3 posFinal = lastPos + (animData->startingPos + position - lastPos) * Easing::GetT(positionInterpType, percentDone);
        Vector3 scaleFinal = lastScale + (scale - lastScale) * Easing::GetT(scaleInterpType, percentDone);

        if (b_posAnimated) 
            transform->SetPosition(posFinal);
        if (b_rotationAnimated)
            transform->SetRotation(rotFinal);
        if (b_scaleAnimated)
            transform->SetScale(scaleFinal);
    }

    Animation::Animation(long ownerID)
    {
        SetType(ComponentType_Animation);
        SetOwnerID(ownerID);        
        m_animations = std::vector<AnimationData>();
    }

    json Animation::GetData(bool b_IDOverride)
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
            { "b_isCollapsed", IsCollapsed() },
            { "b_isActive", IsActive() },
            { "animationData", animationData }
        };

        return jsonData;
    }

    void Animation::PutData(json componentJson, std::string objectName)
	{
        Component::PutData(componentJson, objectName);

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

    void Animation::Play(std::string animationName, uint32_t startTime)
    {
        for (AnimationData &animData : m_animations)
        {
            if (animData.name == animationName)
            {
                animData = AnimationManager::LoadAnimationFile(animData.path);
                animData.b_playing = true;				
                if (!animData.b_startAtOrigin)
                {
                    animData.startingPos = GetOwningObject()->Get<Transform>()->GetPosition();
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

    void Animation::PlayAnimations(uint32_t elapsedTime)
    {
        for (AnimationData& animData : m_animations)
        {
            if (animData.b_playing)
            {
                PlayAnimation(animData.name, elapsedTime);
            }
        }
    }

    void Animation::PlayAnimation(std::string animationName, uint32_t elapsedTime)
    {
        for (AnimationData& animData : m_animations)
        {
            if (animData.name == animationName)
            {
                if (!animData.b_isSorted)
                {
                    animData.SortFrames();
                }

                bool b_timeOver = animData.length < elapsedTime - animData.startTime;
                bool b_animationOver = b_timeOver && animData.b_lastFrameFired;
                
                if (!b_animationOver)
                {
                    std::vector<std::shared_ptr<AnimationProperty>>& keyframes = animData.props;

                    for (int i = 0; i < keyframes.size(); i++) 
                    {
                        auto& frame = keyframes[i];
                        AnimationProperty* prev = nullptr;//  = (i > 0) ? keyframes[i-1].get() : nullptr;

                        int j = i;
                        while (j-1 >= 0 && prev == nullptr)
                        {
                            if (keyframes[j-1].get()->type == frame->type)
                            {
                                prev = keyframes[j-1].get();
                            }
                            else {
                                j--;
                            }
                        }

                        float timeLeft = animData.startTime + frame->time - elapsedTime;
                        float percentDone = prev == nullptr ? 0 : (float)(elapsedTime - animData.startTime - prev->time) / (frame->time - prev->time);
                        bool b_shouldApply = !frame->b_fired &&        
                                            (frame->b_applyInstantly && elapsedTime > animData.startTime + frame->time) ||                                                                               
                                            (!frame->b_applyInstantly && (prev != nullptr && (elapsedTime > animData.startTime + prev->time) && (elapsedTime < animData.startTime + frame->time)));

                        if (b_shouldApply) 
                        {
                            frame->Apply(GetOwningObject(), percentDone, prev, &animData);
                        }
                    }
                }                

                // Gets us one last loop over all frames to hit the ones at the end.
                if (b_timeOver && !animData.b_lastFrameFired)
                {
                    animData.b_lastFrameFired = true;   
                }

                if (b_animationOver)
                {					
                    if (animData.b_loop)                    
                        animData.startTime = elapsedTime;                
                    else
                        Stop(animationName);

                    animData.Reset();                    
                }                
            }
        }
    }
}