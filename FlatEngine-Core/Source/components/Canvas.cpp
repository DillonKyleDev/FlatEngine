#include "components/Canvas.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "managers/Assets.h"
#include "render/GameView.h"
#include "render/SceneView.h"
#include "structs/SceneRenderObject.h"
#include "tools/Logger.h"
#include "tools/Vector2.h"

#include <ext/matrix_clip_space.hpp>


namespace FlatEngine 
{
	Canvas::Canvas(long ownerID)
	{
		SetType(ComponentType_Canvas);
		SetOwnerID(ownerID);
		m_layerNumber = 0;
		m_b_blocksLayers = true;
		m_dimensions = Vector2(20, 10);
		m_activeEdges = Vector4();
		m_screenPixelsPerGridSpace = 64.0f;
		m_renderOutline = std::move(CreateQuadObject());
		m_renderOutline.mesh.SetUBOVec4("color", Assets::assetManager.GetColor("canvasOutline"));
	}

	json Canvas::GetData(bool b_IDOverride)
	{
		json componentJson = {
			{ "width", m_dimensions.x },
			{ "height", m_dimensions.y },
			{ "layerNumber", m_layerNumber },
			{ "b_blocksLayers", m_b_blocksLayers }
		};
		componentJson.update(Component::GetData(b_IDOverride));

		return componentJson;
	}

	void Canvas::PutData(json componentJson, std::string objectName)
	{
		if (componentJson.empty())		
			return;	
		
        Component::PutData(componentJson, objectName);

		SetDimensions(Vector2(JsonHelper::CheckJsonFloat(componentJson, "width", objectName), JsonHelper::CheckJsonFloat(componentJson, "height", objectName)));
		SetLayerNumber(JsonHelper::CheckJsonInt(componentJson, "layerNumber", objectName));
		SetBlocksLayers(JsonHelper::CheckJsonBool(componentJson, "b_blocksLayers", objectName));
    }

	Vector2 Canvas::GetDimensions()
	{
		return m_dimensions;
	}

	void Canvas::SetDimensions(Vector2 setDimensions)
	{
		if (m_dimensions.x >= 0 && m_dimensions.y >= 0)
		{
			m_dimensions = setDimensions;
		}
		else
		{
			Logger::log.Err("Canvas width and height must be positive values.");
		}
	}

	void Canvas::CalculateActiveEdges()
	{
		Transform* transform = GetOwningObject()->Get<Transform>();
		Vector3 position = transform->GetAbsolutePosition();
		Vector3 scale = transform->GetScale();

		float activeLeft = GameView::gameViewCenter.x + ((position.x - (m_dimensions.x / 2 * scale.x)) * GameView::gameViewGridStep);
		float activeRight = GameView::gameViewCenter.x + ((position.x + (m_dimensions.x / 2 * scale.x)) * GameView::gameViewGridStep);
		float activeTop = GameView::gameViewCenter.y - ((position.y + (m_dimensions.y / 2 * scale.y)) * GameView::gameViewGridStep);
		float activeBottom = GameView::gameViewCenter.y - ((position.y - (m_dimensions.y / 2 * scale.y)) * GameView::gameViewGridStep);

		m_activeEdges = Vector4(activeTop, activeRight, activeBottom, activeLeft);
	}

	Vector4 Canvas::GetActiveEdges()
	{
		return m_activeEdges;
	}

	void Canvas::SetLayerNumber(int newLayerNumber)
	{
		m_layerNumber = newLayerNumber;
	}

	int Canvas::GetLayerNumber()
	{
		return m_layerNumber;
	}

	void Canvas::SetBlocksLayers(bool b_blocksLayers)
	{
		m_b_blocksLayers = b_blocksLayers;
	}

	bool Canvas::GetBlocksLayers()
	{
		return m_b_blocksLayers;
	}

	Vector3 Canvas::GetCanvasPlacementPosition(CanvasPlacement* canvasPlacement, Vector2 imageSize)
	{		
		Vector2 pixelPos  = Vector2(imageSize.x * canvasPlacement->percent.x, imageSize.y * canvasPlacement->percent.y) + canvasPlacement->pixel;

		Vector2 gridAdd   = pixelPos * (1.0f / m_screenPixelsPerGridSpace);
		gridAdd.y        *= -1;
		Vector2 startGrid = imageSize * (-0.5f) * (1.0f / m_screenPixelsPerGridSpace);
		startGrid.y      *= -1;
		Vector2 gridPos   = startGrid + gridAdd;			
		
		return Vector3(gridPos, 0);
	}

	glm::mat4 Canvas::GetProjection()
	{
		glm::mat4 projection;			
		
		float halfWidth  = SceneView::finalImageSize.x / m_screenPixelsPerGridSpace / 2.0f;
		float halfHeight = SceneView::finalImageSize.y / m_screenPixelsPerGridSpace / 2.0f;
		projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -2000.0f, 1000.0f);			
		projection[1][1] *= -1;

		return projection;
	}

	Vector2 Canvas::GetMousePosOnCanvas(Vector2 mousePos)
	{		
		Vector2 pixelOffsetFromCenter = mousePos - SceneView::sceneViewportCenter;
		Vector2 gridOffsetFromCenter = pixelOffsetFromCenter * (1.0f / m_screenPixelsPerGridSpace);
		gridOffsetFromCenter.y *= -1.0f;
		return gridOffsetFromCenter;
	}

	float Canvas::GetPixelsPerGridSpace()
	{
		return m_screenPixelsPerGridSpace;
	}

	void Canvas::SetPixelsPerGridSpace(float pixels)
	{
		m_screenPixelsPerGridSpace = pixels;
	}

	SceneRenderObject* Canvas::GetRenderObject()
	{
		return &m_renderOutline;
	}

	void Canvas::UpdateRenderShapes()
	{
		m_renderOutline.transform.SetScale(Vector3(SceneView::finalImageSize.x / m_screenPixelsPerGridSpace, SceneView::finalImageSize.y / m_screenPixelsPerGridSpace, 1));
		m_renderOutline.transform.SetPosition(GetOwningObject()->Get<Transform>()->GetPosition() + Vector3(0,0,-0.0001f));
	}
}