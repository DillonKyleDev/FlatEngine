#include "components/Mesh.h"
#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/Controls.h"
#include "managers/SceneManager.h"
#include "Modals.h"
#include "render/Material.h"
#include "render/VulkanManager.h"

#include "imgui.h"

namespace FL = FlatEngine;


namespace FlatGui
{
	std::string newMaterialFileName = "";
	bool b_openCreateMaterialModal = false;

	void RenderMaterialEditor(bool& b_show)
	{
		if (!b_show)
			return;

		if (FL::GuiCore::BeginWindow("Material Editor", b_show))
		{		
			if (FL::VulkanManager::selectedMaterialName.size() > 0)
			{
				std::shared_ptr<FL::Material> currentMaterial = FL::VulkanManager::vulkan.GetMaterial(FL::VulkanManager::selectedMaterialName);
				std::map<std::string, std::shared_ptr<FL::Material>>& materials = FL::VulkanManager::vulkan.GetMaterials();

				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::Assets::assetManager.GetColor("innerWindow"));
				ImGui::BeginChild("Material Selection", FL::Vector2(), FL::GuiCore::headerFlags);
				ImGui::PopStyleColor();
				// {

				FL::GuiCore::MoveScreenCursor(10, 5);
				ImGui::Text("Select material to edit:");
				FL::GuiCore::MoveScreenCursor(0, 5);

				if (materials.size() > 0)
				{
					FL::GuiCore::PushComboStyles();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 144);
					if (ImGui::BeginCombo("##materials", FL::VulkanManager::selectedMaterialName.c_str()))
					{
						for (std::map<std::string, std::shared_ptr<FL::Material>>::iterator material = materials.begin(); material != materials.end(); material++)
						{						
							bool b_isSelected = (material->second->GetName() == FL::VulkanManager::selectedMaterialName);
							ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::Assets::assetManager.GetColor("outerWindow"));
							if (ImGui::Selectable(material->second->GetName().c_str(), b_isSelected))
							{
								FL::VulkanManager::selectedMaterialName = material->second->GetName();
							}
							if (b_isSelected)
							{
								ImGui::SetItemDefaultFocus();
							}
							ImGui::PopStyleColor();
						}
						ImGui::EndCombo();
					}
					FL::GuiCore::PopComboStyles();

					ImGui::SameLine();

					if (FL::Controls::mappingContexts.size() > 0)
					{
						if (FL::GuiCore::RenderButton("Save"))
						{
							FL::VulkanManager::vulkan.SaveMaterial(currentMaterial);
							//currentMaterial->Init();
							//FL::VulkanManager::vulkan.InitializeMaterials();
						}
					}
					ImGui::SameLine(0, 5);
					if (FL::GuiCore::RenderButton("New Material"))
					{
						b_openCreateMaterialModal = true;
					}
				}

				// }
				ImGui::EndChild(); // Material Selection


				FL::GuiCore::RenderSeparator(10, 10);


				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::Assets::assetManager.GetColor("innerWindow"));
				ImGui::BeginChild("Edit Material", FL::Vector2(), FL::GuiCore::headerFlags);
				ImGui::PopStyleColor();
				// {

				if (currentMaterial != nullptr)
				{
					static std::string vertexInputText = "";
					static std::string fragmentInputText = "";
					vertexInputText = currentMaterial->GetVertexPath();
					fragmentInputText = currentMaterial->GetFragmentPath();
					uint32_t textureCount = currentMaterial->GetTextureCount();
					std::map<uint32_t, FL::TexturePipelineData>* texturesShaderData = currentMaterial->GetTexturesPipelineData();
					std::map<uint32_t, std::string> uboVec4Names = currentMaterial->GetUBOVec4Names();
					FL::TexturePipelineData textureData = FL::TexturePipelineData();
					static bool b_vertexSampler = false;
					static bool b_fragmentSampler = true;
					static bool b_combinedImageSampler = true;
					static bool b_depthImage = false;

					if (FL::GuiCore::RenderInput("##VertexShaderPathInput", "Vertex Shader Path", vertexInputText))
					{					
						FL::VulkanManager::vulkan.SetMaterialVertexPath(currentMaterial->GetName(), vertexInputText);
					}

					if (FL::GuiCore::RenderInput("##FragmentShaderPathInput", "Fragment Shader Path", fragmentInputText))
					{					
						FL::VulkanManager::vulkan.SetMaterialFragmentPath(currentMaterial->GetName(), fragmentInputText);
					}

					if (FL::GuiCore::RenderCheckbox("Vertex Sampler", b_vertexSampler))
					{
						b_vertexSampler = true;
						b_fragmentSampler = false;
					}
					ImGui::SameLine();
					if (FL::GuiCore::RenderCheckbox("Fragment Sampler", b_fragmentSampler))
					{
						b_vertexSampler = false;
						b_fragmentSampler = true;					
					}

					if (FL::GuiCore::RenderCheckbox("Combined Image Sampler", b_combinedImageSampler))
					{
						b_combinedImageSampler = true;
						b_depthImage = false;					
					}
					ImGui::SameLine();
					if (FL::GuiCore::RenderCheckbox("Depth Image", b_depthImage))
					{
						b_combinedImageSampler = false;
						b_depthImage = true;					
					}

					if (FL::GuiCore::RenderButton("Add Sampler"))
					{
						if (b_fragmentSampler)
						{
							textureData.shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
						}
						else
						{
							textureData.shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
						}
						if (b_combinedImageSampler)
						{
							textureData.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						}
						else
						{
							textureData.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
						}

						FL::VulkanManager::vulkan.AddTextureToMaterial(currentMaterial->GetName(), textureCount, textureData);					
						FL::VulkanManager::vulkan.SaveMaterial(currentMaterial);
					}

					for (std::map<uint32_t, FL::TexturePipelineData>::iterator iterator = texturesShaderData->begin(); iterator != texturesShaderData->end(); iterator++)
					{
						switch (iterator->second.shaderStage)
						{
						case VK_SHADER_STAGE_VERTEX_BIT:
						{
							std::string vertexString = "";
							if (iterator->second.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
							{
								vertexString = "Vertex Sampler : layout(binding = " + std::to_string(iterator->first + 1) + ") uniform sampler2D [name])";
							}
							else
							{
								vertexString = "Vertex Depth : layout(input_attachment_index = 0, binding = " + std::to_string(iterator->first + 1) + ") uniform subpassInput inputDepth;";							
							}
							ImGui::Text("%s", vertexString.c_str());
							break;
						}

						case VK_SHADER_STAGE_FRAGMENT_BIT:
						{
							std::string fragmentString = "";
							if (iterator->second.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
							{
								fragmentString = "Fragment Sampler : layout(binding = " + std::to_string(iterator->first + 1) + ") uniform sampler2D [name])";
							}
							else
							{
								fragmentString = "Fragment Depth : layout(input_attachment_index = 0, binding = " + std::to_string(iterator->first + 1) + ") uniform subpassInput inputDepth;";
							}
							ImGui::Text("%s", fragmentString.c_str());
							break;
						}
						default:
							break;
						}
					}
					if (FL::GuiCore::RenderButton("Remove Last Texture"))
					{					
						FL::VulkanManager::vulkan.RemoveTextureFromMaterial(currentMaterial->GetName());
						FL::VulkanManager::vulkan.SaveMaterial(currentMaterial);					
					}

					// Whenever we add a new property to a materials UBO, we should make sure to recreate the commandBuffers in the Models of the Meshes that use that material, taking into account the new Uniform Buffer Size
					ImGui::Text("Vec4s");

					static std::string vec4Name = "";

					FL::GuiCore::RenderInput("##NewVec4Name", "Name", vec4Name);
				
					if (FL::GuiCore::RenderButton("Add Vec4") && vec4Name != "")
					{
						if (currentMaterial->AddUBOVec4(vec4Name))
						{
							for (std::pair<long, FL::Mesh> mesh : FL::SceneManager::loadedScene.GetAll<FL::Mesh>())
							{
								if (mesh.second.GetMaterialName() == FL::VulkanManager::selectedMaterialName)
								{
									mesh.second.SetUBOVec4(vec4Name, FL::Vector4());
								}
							}
							vec4Name = "";

							//FL::VulkanManager::vulkan.SaveMaterial(currentMaterial);						
						}
					}

					if (FL::GuiCore::RenderButton("Remove Last Vec4"))
					{
						FL::VulkanManager::vulkan.RemoveUBOVec4FromMaterial(currentMaterial->GetName());	

						//FL::VulkanManager::vulkan.SaveMaterial(currentMaterial);					
					}

					for (std::map<uint32_t, std::string>::iterator iter = uboVec4Names.begin(); iter != uboVec4Names.end(); iter++)
					{
						// TODO: Add editing of and adding of vec4 names here, then "refresh" the Meshes (emplace new std::pair<std::string, glm::vec4> in their m_uboVec4s members) that use this Material to account for the new vec4, or add a button to refresh it in the Mesh Component in inspector.
						// Currently the vec4s are global (the same for all Materials) because they and are created in constructor of Material and added to the Meshes m_uboVec4s in Mesh::SetMaterial() method. TODO: Make this dynamic
						std::string text = "vec4 vec4s[" + std::to_string(iter->first) + "]    Name: " + iter->second;
						ImGui::Text("%s", text.c_str());
					}
				}

				// }
				ImGui::EndChild();
			}


			// Create new Mapping Context modal
			if (Modals::RenderInputModal("Create New Material", "Enter a name for the new Material", newMaterialFileName, b_openCreateMaterialModal))
			{
				FL::VulkanManager::vulkan.CreateNewMaterialFile(newMaterialFileName);
				FL::VulkanManager::selectedMaterialName = newMaterialFileName;			
			}

			FL::GuiCore::EndWindow(); // Material Editor
		}
	}
}