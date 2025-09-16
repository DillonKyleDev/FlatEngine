#include "FlatEngine.h"
#include "FlatGui.h"
#include "Material.h"
#include "GraphicsPipeline.h"
#include "VulkanManager.h"

#include "imgui.h"


namespace FL = FlatEngine;

using Material = FL::Material;

namespace FlatGui
{
	std::string newMaterialFileName = "";
	bool b_openCreateMaterialModal = false;
	bool FG_b_showMaterialEditor = false;

	void RenderMaterialEditor()
	{
		FL::BeginWindow("Material Editor", FG_b_showMaterialEditor);
		// {		

		if (FL::F_selectedMaterialName.size() > 0)
		{
			std::shared_ptr<Material> currentMaterial = FL::F_VulkanManager->GetMaterial(FL::F_selectedMaterialName);
			std::vector<std::shared_ptr<Material>> materials = FL::F_VulkanManager->GetMaterials();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
			ImGui::BeginChild("Material Selection", Vector2(0), FL::F_headerFlags);
			ImGui::PopStyleColor();
			// {

			FL::MoveScreenCursor(10, 5);
			ImGui::Text("Select material to edit:");
			FL::MoveScreenCursor(0, 5);

			if (materials.size() > 0)
			{
				FL::PushComboStyles();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 144);
				if (ImGui::BeginCombo("##materials", FL::F_selectedMaterialName.c_str()))
				{
					for (std::shared_ptr<Material> material : materials)
					{						
						bool b_isSelected = (material->GetName() == FL::F_selectedMaterialName);
						ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("outerWindow"));
						if (ImGui::Selectable(material->GetName().c_str(), b_isSelected))
						{
							FL::F_selectedMaterialName = material->GetName();
						}
						if (b_isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
						ImGui::PopStyleColor();
					}
					ImGui::EndCombo();
				}
				FL::PopComboStyles();

				ImGui::SameLine();

				if (FL::F_MappingContexts.size() > 0)
				{
					if (FL::RenderButton("Save"))
					{
						FL::F_VulkanManager->SaveMaterial(currentMaterial);
						//currentMaterial->Init();
						FL::F_VulkanManager->InitializeMaterials();
					}
				}
				ImGui::SameLine(0, 5);
				if (FL::RenderButton("New Material"))
				{
					b_openCreateMaterialModal = true;
				}
			}

			// }
			ImGui::EndChild(); // Material Selection


			FL::RenderSeparator(10, 10);


			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
			ImGui::BeginChild("Edit Material", Vector2(0), FL::F_headerFlags);
			ImGui::PopStyleColor();
			// {

			if (currentMaterial != nullptr)
			{
				static std::string vertexInputText = "";
				static std::string fragmentInputText = "";
				vertexInputText = currentMaterial->GetVertexPath();
				fragmentInputText = currentMaterial->GetFragmentPath();
				uint32_t textureCount = currentMaterial->GetTextureCount();

				if (FL::RenderInput("##VertexShaderPathInput", "Vertex Shader Path", vertexInputText))
				{
					currentMaterial->SetVertexPath(vertexInputText);
				}

				if (FL::RenderInput("##FragmentShaderPathInput", "Fragment Shader Path", fragmentInputText))
				{
					currentMaterial->SetFragmentPath(fragmentInputText);
				}

				if (FL::RenderButton("Add Texture"))
				{
					Texture newTexture = Texture();
					currentMaterial->SetTextureCount(textureCount + 1);
				}

				std::string textureCountText = "Textures: " + std::to_string(textureCount);
				ImGui::Text(textureCountText.c_str());

				//int textureCounter = 0;
				//for (Texture& texture : currentMaterial->GetTextures())
				//{
				//	std::string texturePath = texture.GetTexturePath();

				//	if (FL::RenderInput("##TexturePathInput" + std::to_string(textureCounter), "Texture Path", texturePath))
				//	{
				//		texture.SetTexturePath(texturePath);
				//	}
				//	textureCounter++;
				//}
			}

			// }
			ImGui::EndChild();
		}


		// Create new Mapping Context modal
		if (FL::RenderInputModal("Create New Material", "Enter a name for the new Material", newMaterialFileName, b_openCreateMaterialModal))
		{
			FL::F_VulkanManager->CreateNewMaterialFile(newMaterialFileName);
			FL::F_selectedMaterialName = newMaterialFileName;
			FG_b_showMaterialEditor = true;
		}

		// }
		FL::EndWindow();
	}
}