#pragma once
#include "CompositeCollider.h"

#include <vector>
#include <map>
#include <string>


namespace FlatEngine
{
	class Component;
	class GameObject;
	class Transform;
	class Sprite;
	class Script;
	class Canvas;
	class Audio;
	class Text;
	class Camera;
	class Collider;
	class BoxCollider;
	class CircleCollider;
	class Animation;
	class Button;
	class RigidBody;
	class CompositeCollider;
	class CharacterController;
	class TileMap;

	class ECSManager
	{
	public:
		ECSManager();
		~ECSManager();

		void Cleanup();
		Transform* AddTransform(Transform transform, long ownerID);
		Sprite* AddSprite(Sprite sprite, long ownerID);
		Camera* AddCamera(Camera camera, long ownerID);
		Script* AddScript(Script script, long ownerID);
		Canvas* AddCanvas(Canvas canvas, long ownerID);
		Audio* AddAudio(Audio audio, long ownerID);
		Text* AddText(Text text, long ownerID);
		RayCast* AddRayCast(RayCast rayCast, long ownerID);
		CompositeCollider* AddCompositeCollider(CompositeCollider collider, long ownerID);
		BoxCollider* AddBoxCollider(BoxCollider collider, long ownerID);
		CircleCollider* AddCircleCollider(CircleCollider collider, long ownerID);
		Animation* AddAnimation(Animation animation, long ownerID);
		Button* AddButton(Button button, long ownerID);		
		RigidBody* AddRigidBody(RigidBody rigidBody, long ownerID);
		CharacterController* AddCharacterController(CharacterController characterController, long ownerID);	
		TileMap* AddTileMap(TileMap tileMap, long ownerID);

		bool RemoveComponent(Component* component, long ownerID = -1);
		bool RemoveTransform(long ownerID);
		bool RemoveSprite(long ownerID);
		bool RemoveCamera(long ownerID);
		bool RemoveScript(long ownerID, long scriptID);
		bool RemoveCanvas(long ownerID);
		bool RemoveAudio(long ownerID);
		bool RemoveText(long ownerID);
		bool RemoveRayCast(long ownerID);
		bool RemoveCompositeCollider(long ownerID);
		bool RemoveBoxCollider(long componentID, long ownerID);
		bool RemoveCircleCollider(long componentID, long ownerID);
		bool RemoveAnimation(long ownerID);
		bool RemoveButton(long ownerID);
		bool RemoveRigidBody(long ownerID);
		bool RemoveCharacterController(long ownerID);
		bool RemoveTileMap(long ownerID);

		Transform* GetTransformByOwner(long ownerID);
		Sprite* GetSpriteByOwner(long ownerID);
		Camera* GetCameraByOwner(long ownerID);
		std::vector<Script*> GetScriptsByOwner(long ownerID);		
		Canvas* GetCanvasByOwner(long ownerID);
		Audio* GetAudioByOwner(long ownerID);
		Text* GetTextByOwner(long ownerID);
		CompositeCollider* GetCompositeColliderByOwner(long ownerID);
		std::vector<BoxCollider*> GetBoxCollidersByOwner(long ownerID);
		std::vector<CircleCollider*> GetCircleCollidersByOwner(long ownerID);
		Animation* GetAnimationByOwner(long ownerID);
		Button* GetButtonByOwner(long ownerID);
		RigidBody* GetRigidBodyByOwner(long ownerID);
		CharacterController* GetCharacterControllerByOwner(long ownerID);
		TileMap* GetTileMapByOwner(long ownerID);

		std::map<long, Transform>& GetTransforms();
		std::map<long, Sprite>& GetSprites();
		std::map<long, Camera>& GetCameras();
		std::map<long, std::map<long, Script>>& GetScripts();
		std::map<long, std::vector<std::string>>& GetLuaScriptsByOwner();
		std::map<long, Button>& GetButtons();
		std::map<long, Canvas>& GetCanvases();
		std::map<long, Animation>& GetAnimations();
		std::map<long, Audio>& GetAudios();
		std::map<long, Text>& GetTexts();
		std::vector<Collider*> GetColliders();
		std::map<long, RayCast>& GetRayCasts();
		std::map<long, CompositeCollider>& GetCompositeColliders();
		std::map<long, std::map<long, BoxCollider>>& GetBoxColliders();
		std::map<long, std::map<long, CircleCollider>>& GetCircleColliders();
		std::map<long, RigidBody>& GetRigidBodies();
		std::map<long, CharacterController>& GetCharacterControllers();
		std::map<long, TileMap>& GetTileMaps();		

	private:
		std::map<long, Transform> m_Transforms;
		std::map<long, Sprite> m_Sprites;
		std::map<long, Camera> m_Cameras;
		std::map<long, std::map<long, Script>> m_Scripts;		
		std::map<long, std::vector<std::string>> m_LuaScriptsByOwner;
		std::map<long, Button> m_Buttons;
		std::map<long, Canvas> m_Canvases;
		std::map<long, Animation> m_Animations;
		std::map<long, Audio> m_Audios;
		std::map<long, Text> m_Texts;		
		std::map<long, CompositeCollider> m_CompositeColliders;
		std::map<long, RayCast> m_RayCasts;
		std::map<long, std::map<long, BoxCollider>> m_BoxColliders;
		std::map<long, std::map<long, CircleCollider>> m_CircleColliders;
		std::map<long, RigidBody> m_RigidBodies;
		std::map<long, CharacterController> m_CharacterControllers;
		std::map<long, TileMap> m_TileMaps;		
	};
}