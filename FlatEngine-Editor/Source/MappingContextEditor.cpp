#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/Controls.h"
#include "Modals.h"
#include "tools/Logger.h"

#include "imgui.h"

namespace FL = FlatEngine;


namespace FlatGui 
{
	std::string newMappingContextFileName = "";
	bool b_openCreateContextModal = false;


	void RenderInputAction(FL::Controls::MappingContext* context, FL::Controls::ActionMapping& actionMapping, float width)
	{
		std::string actionName = actionMapping.actionName;
		std::string keyCode = actionMapping.keyCode;
		FL::Controls::PressType pressType = actionMapping.pressType;

		static int currentPressType = (int)pressType;
		int selectedPressType = 0;

		for (int i = 0; i < (int)FL::Controls::PressType::PressType_Size; i++)
		{
			if (currentPressType == i)
			{
				selectedPressType = i;
			}
		}
		
		std::string textLabelID = "##EditInputActionName" + keyCode + std::to_string((int)pressType);
		
		if (FL::GuiCore::RenderInput(textLabelID.c_str(), FL::Controls::pressTypeStrings[(int)pressType], actionName, false, width))
		{			
			if (!context->InputActionNameTaken(actionName, keyCode))
			{
				actionMapping.actionName = actionName;
			}
			else
			{
				FL::Logger::log.Warn("Duplicate action names not allowed in the same Mapping Context.");
			}
		}
	}

	void RenderMappingContextEditor(bool& b_show)
	{
		if (!b_show)
			return;
		
		if (FL::GuiCore::BeginWindow("Mapping Context Editor", b_show))
		{			
			float widthAvailable = ImGui::GetContentRegionAvail().x;
			static int currentContext = 0;
					
			if (FL::Controls::mappingContexts.size() > 0)
			{
				FL::Controls::MappingContext* currentContext = FL::Controls::GetMappingContext(FL::Controls::selectedMappingContextName);


				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::Assets::assetManager.GetColor("innerWindow"));
				ImGui::BeginChild("Context Selection", FL::Vector2(0), FL::GuiCore::headerFlags);
				ImGui::PopStyleColor();
				// {
						
					FL::GuiCore::MoveScreenCursor(10, 5);
					ImGui::Text("Select mapping context to edit:");
					FL::GuiCore::MoveScreenCursor(0, 5);

					if (FL::Controls::mappingContexts.size() > 0)
					{
						FL::GuiCore::PushComboStyles();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 144);
						if (ImGui::BeginCombo("##contexts", FL::Controls::selectedMappingContextName.c_str()))
						{
							for (int i = 0; i < FL::Controls::mappingContexts.size(); i++)
							{
								bool b_isSelected = (FL::Controls::mappingContexts.at(i).GetName() == FL::Controls::selectedMappingContextName);
								ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::Assets::assetManager.GetColor("outerWindow"));
								if (ImGui::Selectable(FL::Controls::mappingContexts.at(i).GetName().c_str(), b_isSelected))
								{
									FL::Controls::selectedMappingContextName = FL::Controls::mappingContexts.at(i).GetName();
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
								SaveMappingContext(currentContext->GetPath(), *currentContext);
							}
						}
						ImGui::SameLine(0, 5);
						if (FL::GuiCore::RenderButton("New Context"))
						{
							b_openCreateContextModal = true;
						}
					}

				// }
				ImGui::EndChild(); // Context Selection
				
				
				FL::GuiCore::RenderSeparator(10, 10);


				ImGui::BeginDisabled(currentContext == nullptr);
				// {

					ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::Assets::assetManager.GetColor("innerWindow"));
					ImGui::BeginChild("Create New Input Action", FL::Vector2(), FL::GuiCore::headerFlags);
					ImGui::PopStyleColor();
					// {

						FL::GuiCore::MoveScreenCursor(10, 5);					
						ImGui::Text("Create new Input Action:");
						FL::GuiCore::MoveScreenCursor(0, 5);

						static int currentInput = 0;
						static int currentPressType = 0;
						static std::string inputText = "Input Action Name";
						float itemWidth = ImGui::GetContentRegionAvail().x / 3;

						ImGui::Text("Input Source");
						FL::GuiCore::PushComboStyles();										
						ImGui::SetNextItemWidth(itemWidth);
						if (ImGui::BeginCombo("##CreateInputActionInputSelector", FL::Controls::keyBindingsAvailable[currentInput].c_str()))
						{
							for (int n = 0; n < FL::Controls::keyBindingsAvailable.size(); n++)
							{
								bool b_isSelected = (FL::Controls::keyBindingsAvailable[currentInput] == FL::Controls::keyBindingsAvailable[n]);
								if (ImGui::Selectable(FL::Controls::keyBindingsAvailable[n].c_str(), b_isSelected))
								{
									currentInput = n;									
								}							
							}
							ImGui::EndCombo();
						}
						FL::GuiCore::PopComboStyles();


						ImGui::SameLine();
						FL::Vector2 cursorPos = ImGui::GetCursorScreenPos();
						FL::GuiCore::MoveScreenCursor(0, -25);
						ImGui::Text("Press Type");
						ImGui::SetCursorScreenPos(cursorPos);
						FL::GuiCore::PushComboStyles();
						ImGui::SetNextItemWidth(itemWidth);
						if (ImGui::BeginCombo("##NewInputActionPressTypeSelector", FL::Controls::pressTypeStrings[currentPressType].c_str()))
						{
							for (int n = 0; n < FL::Controls::pressTypeStrings.size(); n++)
							{
								bool b_isSelected = (FL::Controls::pressTypeStrings[currentPressType] == FL::Controls::pressTypeStrings[n]);
								if (ImGui::Selectable(FL::Controls::pressTypeStrings[n].c_str(), b_isSelected))
								{
									currentPressType = n;
								}
							}
							ImGui::EndCombo();
						}
						FL::GuiCore::PopComboStyles();


						ImGui::SameLine();


						FL::GuiCore::MoveScreenCursor(0, -25);
						cursorPos = ImGui::GetCursorScreenPos();
						ImGui::Text("Action Name:");
						ImGui::SetCursorScreenPos(FL::Vector2(cursorPos.x, cursorPos.y + 25));		
						ImGui::SetNextItemWidth(itemWidth);
						FL::GuiCore::RenderInput("##InputActionName", "", inputText, false, ImGui::GetContentRegionAvail().x - 40, ImGuiInputTextFlags_AutoSelectAll);


						ImGui::SameLine();
						if (FL::GuiCore::RenderButton("Add"))
						{
							currentContext->AddKeyBinding(FL::Controls::keyBindingsAvailable[currentInput].c_str(), inputText, (FL::Controls::PressType)currentPressType);
							inputText = "";
						}

					// }
					ImGui::EndChild(); // Create New Input Action


					FL::GuiCore::RenderSeparator(10, 10);


					ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::Assets::assetManager.GetColor("innerWindow"));
					ImGui::BeginChild("Existing Bindings", FL::Vector2(), FL::GuiCore::headerFlags);
					// {

						ImGui::PopStyleColor();
						FL::GuiCore::MoveScreenCursor(10, 5);					
						ImGui::Text("Existing Bindings:");
						FL::GuiCore::MoveScreenCursor(0, 5);

						if (currentContext != nullptr)
						{					
							for (std::pair<std::string, std::shared_ptr<FL::Controls::InputMapping>> inputAction : currentContext->GetInputActions())
							{
								FL::GuiCore::MoveScreenCursor(0, 5);
								ImGui::Text("%s", inputAction.second->keyCode.c_str());
								float halfWidth = ImGui::GetContentRegionAvail().x / 2;

								RenderInputAction(currentContext, inputAction.second->pressActions.downAction, halfWidth);
								ImGui::SameLine();
								RenderInputAction(currentContext, inputAction.second->pressActions.upAction, ImGui::GetContentRegionAvail().x);

								RenderInputAction(currentContext, inputAction.second->pressActions.holdAction, halfWidth);
								ImGui::SameLine();
								RenderInputAction(currentContext, inputAction.second->pressActions.doubleAction, -1);
							}
						}

					// }
					ImGui::EndChild(); // Existing Bindings

				// }
				ImGui::EndDisabled();
			}
			

			// Create new Mapping Context modal
			if (Modals::RenderInputModal("Create New Mapping Context", "Enter a name for the new Mapping Context", newMappingContextFileName, b_openCreateContextModal))
			{
				FL::Controls::CreateNewMappingContextFile(newMappingContextFileName);
				FL::Controls::selectedMappingContextName = newMappingContextFileName;				
			}			
		}	
		
		FL::GuiCore::EndWindow(); // MappingContext Editor
	}
}