#pragma once
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"

#include <string>
#include <vector>


namespace FlatGui
{
    namespace GuiEditor
    {
        extern void RenderTransformTable(std::string ID, FlatEngine::Vector3& position, FlatEngine::Vector3& rotation, FlatEngine::Vector3& scale);
        extern void RenderVector2Table(std::string ID, std::string label, FlatEngine::Vector2& vec2, FlatEngine::Vector2 tableSize, float labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light = true);
        extern void RenderVector3Table(std::string ID, std::string label, FlatEngine::Vector3& vec3, FlatEngine::Vector2 tableSize, float labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light = true);
        extern void RenderVector4Table(std::string ID, std::string label, FlatEngine::Vector4& vec4, FlatEngine::Vector2 tableSize, float labelWidth, std::string labelColor, std::vector<std::string> valueLabelColors, bool b_light = true);
        extern void BeginToolTip(std::string title);
		extern void EndToolTip();
		extern void RenderToolTipText(std::string label, std::string text);
		extern void RenderToolTipFloat(std::string label, float data);
		extern void RenderToolTipLong(std::string label, long data);
		extern void RenderToolTipLongVector(std::string label, std::vector<long> data);
    }
}