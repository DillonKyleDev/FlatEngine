#include "FlatEngine.h"
#include "FlatGui.h"
#include "MappingContext.h"

#include "imgui.h"


namespace FL = FlatEngine;

using MappingContext = FL::MappingContext;

namespace FlatGui 
{
	std::string fileName = "";
	bool b_openCreateContextModal = false;


	void RenderInputAction(MappingContext* context, FL::ActionMapping& actionMapping, float width)
	{
		std::string actionName = actionMapping.actionName;
		std::string keyCode = actionMapping.keyCode;
		FL::PressType pressType = actionMapping.pressType;

		static int currentPressType = (int)pressType;
		int selectedPressType = 0;

		for (int i = 0; i < (int)FL::PressType::PT_Size; i++)
		{
			if (currentPressType == i)
			{
				selectedPressType = i;
			}
		}
		
		std::string textLabelID = "##EditInputActionName" + keyCode + std::to_string((int)pressType);
		
		if (FL::RenderInput(textLabelID.c_str(), FL::F_PressTypeStrings[(int)pressType], actionName, false, width))
		{			
			if (!context->InputActionNameTaken(actionName, keyCode))
			{
				actionMapping.actionName = actionName;
			}
			else
			{
				FL::LogString("Duplicate action names not allowed in the same Mapping Context.");
			}
		}
	}

	void RenderMappingContextEditor()
	{
		FL::BeginWindow("Mapping Context Editor", FG_b_showMappingContextEditor);
		// {
			
			float widthAvailable = ImGui::GetContentRegionAvail().x;
			static int currentContext = 0;
					
			if (FL::F_MappingContexts.size() > 0)
			{
				MappingContext* currentContext = FL::GetMappingContext(FL::F_selectedMappingContextName);


				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
				ImGui::BeginChild("Context Selection", Vector2(0), FL::F_headerFlags);
				ImGui::PopStyleColor();
				// {
						
					FL::MoveScreenCursor(10, 5);
					ImGui::Text("Select mapping context to edit:");
					FL::MoveScreenCursor(0, 5);

					if (FL::F_MappingContexts.size() > 0)
					{
						FL::PushComboStyles();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 144);
						if (ImGui::BeginCombo("##contexts", FL::F_selectedMappingContextName.c_str()))
						{
							for (int i = 0; i < FL::F_MappingContexts.size(); i++)
							{
								bool b_isSelected = (FL::F_MappingContexts.at(i).GetName() == FL::F_selectedMappingContextName);
								ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("outerWindow"));
								if (ImGui::Selectable(FL::F_MappingContexts.at(i).GetName().c_str(), b_isSelected))
								{
									FL::F_selectedMappingContextName = FL::F_MappingContexts.at(i).GetName();
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
								SaveMappingContext(currentContext->GetPath(), *currentContext);
							}
						}
						ImGui::SameLine(0, 5);
						if (FL::RenderButton("New Context"))
						{
							b_openCreateContextModal = true;
						}
					}

				// }
				ImGui::EndChild(); // Context Selection
				
				
				FL::RenderSeparator(10, 10);


				ImGui::BeginDisabled(currentContext == nullptr);
				// {

					ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
					ImGui::BeginChild("Create New Input Action", Vector2(0), FL::F_headerFlags);
					ImGui::PopStyleColor();
					// {

						FL::MoveScreenCursor(10, 5);					
						ImGui::Text("Create new Input Action:");
						FL::MoveScreenCursor(0, 5);

						static int currentInput = 0;
						static int currentPressType = 0;
						static std::string inputText = "Input Action Name";
						float itemWidth = ImGui::GetContentRegionAvail().x / 3;

						ImGui::Text("Input Source");
						FL::PushComboStyles();										
						ImGui::SetNextItemWidth(itemWidth);
						if (ImGui::BeginCombo("##CreateInputActionInputSelector", FL::F_KeyBindingsAvailable[currentInput].c_str()))
						{
							for (int n = 0; n < FL::F_KeyBindingsAvailable.size(); n++)
							{
								bool b_isSelected = (FL::F_KeyBindingsAvailable[currentInput] == FL::F_KeyBindingsAvailable[n]);
								if (ImGui::Selectable(FL::F_KeyBindingsAvailable[n].c_str(), b_isSelected))
								{
									currentInput = n;									
								}							
							}
							ImGui::EndCombo();
						}
						FL::PopComboStyles();


						ImGui::SameLine();
						Vector2 cursorPos = ImGui::GetCursorScreenPos();
						FL::MoveScreenCursor(0, -25);
						ImGui::Text("Press Type");
						ImGui::SetCursorScreenPos(cursorPos);
						FL::PushComboStyles();
						ImGui::SetNextItemWidth(itemWidth);
						if (ImGui::BeginCombo("##NewInputActionPressTypeSelector", FL::F_PressTypeStrings[currentPressType].c_str()))
						{
							for (int n = 0; n < FL::F_PressTypeStrings.size(); n++)
							{
								bool b_isSelected = (FL::F_PressTypeStrings[currentPressType] == FL::F_PressTypeStrings[n]);
								if (ImGui::Selectable(FL::F_PressTypeStrings[n].c_str(), b_isSelected))
								{
									currentPressType = n;
								}
							}
							ImGui::EndCombo();
						}
						FL::PopComboStyles();


						ImGui::SameLine();


						FL::MoveScreenCursor(0, -25);
						cursorPos = ImGui::GetCursorScreenPos();
						ImGui::Text("Action Name:");
						ImGui::SetCursorScreenPos(Vector2(cursorPos.x, cursorPos.y + 25));		
						ImGui::SetNextItemWidth(itemWidth);
						FL::RenderInput("##InputActionName", "", inputText, false, ImGui::GetContentRegionAvail().x - 40, ImGuiInputTextFlags_AutoSelectAll);


						ImGui::SameLine();
						if (FL::RenderButton("Add"))
						{
							currentContext->AddKeyBinding(FL::F_KeyBindingsAvailable[currentInput].c_str(), inputText, (FL::PressType)currentPressType);
							inputText = "";
						}

					// }
					ImGui::EndChild(); // Create New Input Action


					FL::RenderSeparator(10, 10);


					ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
					ImGui::BeginChild("Existing Bindings", Vector2(0), FL::F_headerFlags);
					// {

						ImGui::PopStyleColor();
						FL::MoveScreenCursor(10, 5);					
						ImGui::Text("Existing Bindings:");
						FL::MoveScreenCursor(0, 5);

						if (currentContext != nullptr)
						{					
							for (std::pair<std::string, std::shared_ptr<FL::InputMapping>> inputAction : currentContext->GetInputActions())
							{
								FL::MoveScreenCursor(0, 5);
								ImGui::Text(inputAction.second->keyCode.c_str());
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
			if (FL::RenderInputModal("Create New Mapping Context", "Enter a name for the new Mapping Context", fileName, b_openCreateContextModal))
			{
				FL::CreateNewMappingContextFile(fileName);
				FL::F_selectedMappingContextName = fileName;
				FG_b_showMappingContextEditor = true;
			}

		// }
		FL::EndWindow();
	}
}