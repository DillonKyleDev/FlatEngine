#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/Controls.h"
#include "Modals.h"
#include "tools/Time.h"

#include "imgui.h"
#include "tools/Vector2.h"
#include <cstdint>

namespace FL = FlatEngine;


namespace FlatGui 
{
	void RenderInputActionName(std::string ID, std::string& actionName, FL::Vector2 tableSize)
	{
		FlatEngine::GuiCore::TableProps actionNameProps("##InputActionName" + ID, "Action Name", tableSize);
		actionNameProps.labelColor = "mappingContextLabelColor";
		actionNameProps.labelTextColor = "mappingContextLabelTextColor";
		actionNameProps.labelWidth = 88;
		actionNameProps.b_light = false;
		actionNameProps.b_lightSet = true;		
		FL::GuiCore::RenderStringTable(actionNameProps, actionName);
	}
	bool RenderInputActionPressType(std::string ID, FL::Controls::PressType& pressType)
	{
		bool b_changed = false;

		int currentPressType = (int)pressType;	
		FlatEngine::GuiCore::TableProps pressTypeProps("##PressTypeCombo" + ID, "Press Type", FL::Vector2(ImGui::GetContentRegionAvail().x, 0));
		pressTypeProps.labelColor = "mappingContextLabelColor";
		pressTypeProps.labelTextColor = "mappingContextLabelTextColor";
		pressTypeProps.labelWidth = 88;
		pressTypeProps.b_light = true;
		pressTypeProps.b_lightSet = true;		
		b_changed = FL::GuiCore::RenderComboTable(pressTypeProps, FL::Controls::PressTypeStrings[currentPressType], FL::Controls::PressTypeStrings, currentPressType);												

		if (b_changed)
			pressType = (FL::Controls::PressType)currentPressType;

		FL::GuiCore::RenderSeparator(0,0);

		return b_changed;
	}

	bool CheckLastKeyPressed(std::string keyToCheck)
	{
		return FL::Controls::pressedKeys.size() && FL::Controls::pressedKeys.front() == keyToCheck;
	}

	void RenderMappingContextEditor(bool& b_show)
	{
		if (!b_show)
			return;
		
		if (FL::GuiCore::BeginWindow("Mapping Context Editor", b_show, 0, "mappingContextEditorBg"))
		{			
			float widthAvailable = ImGui::GetContentRegionAvail().x;
					
			if (FL::Controls::selectedMappingContextName == "")
			{
				if (FL::Controls::mappingContexts.size())
					FL::Controls::selectedMappingContextName = FL::Controls::mappingContexts.front().GetName();
				else
					FL::Controls::selectedMappingContextName = "None";
			}

			FL::Controls::MappingContext* currentContext = FL::Controls::GetMappingContext(FL::Controls::selectedMappingContextName);

			ImGui::BeginChild("Context Selection", FL::Vector2(0), FL::GuiCore::headerFlags);
			// {
				FL::GuiCore::MoveScreenCursor(0,2);
				FL::Vector2 selectedContextTableSize = FL::Vector2(ImGui::GetContentRegionAvail().x - 134, 0);
				int currentContextNameIndex;				
				std::vector<std::string> contextNames;
				for (int i = 0; i < FL::Controls::mappingContexts.size(); i++)
				{
					contextNames.push_back(FL::Controls::mappingContexts.at(i).GetName());	
					if (FL::Controls::selectedMappingContextName == FL::Controls::mappingContexts.at(i).GetName())				
						currentContextNameIndex = i;
				}
				FL::GuiCore::TableProps comboTableProps("mappingContextsCombo", "Current Context", selectedContextTableSize);
				comboTableProps.labelColor = "mappingContextLabelColor";
				comboTableProps.labelTextColor = "mappingContextLabelTextColor";
				comboTableProps.labelWidth = 120;
				comboTableProps.b_light = false;
				comboTableProps.b_lightSet = true;
				if (FL::GuiCore::RenderComboTable(comboTableProps, FL::Controls::selectedMappingContextName, contextNames, currentContextNameIndex)) FL::Controls::selectedMappingContextName = contextNames[currentContextNameIndex];

				ImGui::SameLine(0,3);

				ImGui::BeginDisabled(currentContext == nullptr);
				{
					if (FL::GuiCore::RenderButton("Save", FL::Vector2(40, FL::GuiCore::TABLE_HEIGHT)))
					{
						SaveMappingContext(currentContext->GetPath(), *currentContext);
					}
				}
				ImGui::EndDisabled();
				ImGui::SameLine(0, 3);
				if (FL::GuiCore::RenderButton("New Context", FL::Vector2(88, FL::GuiCore::TABLE_HEIGHT)))
				{
					Modals::b_openMappingContextModal = true;
				}


				ImGui::BeginDisabled(currentContext == nullptr);
				// {
							
					FL::GuiCore::RenderSectionHeader("Create new Input Action");													
					FL::GuiCore::MoveScreenCursor(0, 3);		

					FlatEngine::GuiCore::TableProps buttonProps("##InputButtonCombo", "Button", FL::Vector2(ImGui::GetContentRegionAvail().x - 22, 0));					
					buttonProps.labelColor = "mappingContextLabelColor";
					buttonProps.labelTextColor = "mappingContextLabelTextColor";
					buttonProps.labelWidth = 88;
					buttonProps.b_light = true;
					buttonProps.b_lightSet = true;	
					FL::GuiCore::RenderComboTable(buttonProps, FL::Controls::KeyBindingsAvailable[currentContext->tempKeycode], FL::Controls::KeyBindingsAvailable, currentContext->tempKeycode);
					ImGui::SameLine(0,0);

					static uint32_t disabledTimeout = 0;
					static bool b_timerStarted = false;
					
					if (currentContext->b_waitingForInput && FL::Controls::pressedKeys.size() && FL::Controls::pressedKeys.front() != "")
					{
						for (int i = 0; i < FL::Controls::KeyBindingsAvailable.size(); i++)
						{
							if (FL::Controls::KeyBindingsAvailable[i] == FL::Controls::pressedKeys.front())
							{
								currentContext->tempKeycode = i;
								break;
							}
						}					
						currentContext->b_waitingForInput = false;
						b_timerStarted = true;
						disabledTimeout = FL::Time::Time() + 1000;		
					}
										
					ImGui::BeginDisabled(b_timerStarted && FL::Time::Time() < disabledTimeout);
					if (FL::GuiCore::RenderImageButton("##ListenForInput", FL::Assets::assetManager.GetTexture("Button"), FL::Vector2(16), 0, FL::Vector2(3), "buttonBorder", currentContext->b_waitingForInput ? "imageButtonHoveredWaitingForInput" : "transparent", "imageButtonTint", currentContext->b_waitingForInput ? "imageButtonHoveredWaitingForInput" : "imageButtonHovered"))					
					{
						currentContext->b_waitingForInput = true;								
					}
					ImGui::EndDisabled();
					if (b_timerStarted && FL::Time::Time() > disabledTimeout)					
						b_timerStarted = false;					
					
					if (ImGui::IsItemHovered())					
						FL::GuiCore::RenderTextToolTip("Start listening for key press");
	
					FL::GuiCore::RenderSeparator(-4, -3);

					RenderInputActionName("##CreateInputAction", currentContext->tempActionName, FL::Vector2(ImGui::GetContentRegionAvail().x));
					RenderInputActionPressType("##CreateInputAction", currentContext->tempPressType);						
					FL::GuiCore::MoveScreenCursor(0, 3);	
					
					ImGui::BeginDisabled(currentContext->tempActionName == "");
					if (FL::GuiCore::RenderButton("Add Input", FL::Vector2(ImGui::GetContentRegionAvail().x, 0)))
					{
						currentContext->AddKeyBinding(FL::Controls::KeyBindingsAvailable[currentContext->tempKeycode], currentContext->tempActionName, (FL::Controls::PressType)currentContext->tempPressType);
						currentContext->tempActionName = "";
						currentContext->tempKeycode = 0;
					}
					ImGui::EndDisabled();

					FL::GuiCore::RenderSeparator(0,1);						
					
					std::string keycodeToRemove = "";					
					if (currentContext != nullptr)
					{					
						for (auto& inputMapping : currentContext->GetInputMappings())
						{			
							FL::GuiCore::RenderSectionHeader(inputMapping.second.keycode);																		
							FL::GuiCore::MoveScreenCursor(0, -3);				
							for (int i = 0; i < inputMapping.second.actions.size(); i++)
							{																
								FL::Controls::PressType tempPress = inputMapping.second.actions[i].pressType;
								RenderInputActionName("##CreateInputAction", inputMapping.second.actions[i].actionName, FL::Vector2(ImGui::GetContentRegionAvail().x - 22));
								FL::GuiCore::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 22, -22);
								if (FL::GuiCore::RenderImageButton("##trashIcon-" + inputMapping.second.keycode + inputMapping.second.actions[i].actionName, FL::Assets::assetManager.GetTexture("trash"), FL::Vector2(16), 0, FL::Vector2(3)))
								{
									currentContext->RemoveKeyBinding(inputMapping.second.keycode, tempPress);
									if (inputMapping.second.actions.size() == 0)   
										keycodeToRemove = inputMapping.second.keycode; 
									break;
								}	
								FL::GuiCore::MoveScreenCursor(0, -4);
								if (RenderInputActionPressType("##CreateInputAction", tempPress))
								{									
									currentContext->RemoveKeyBinding(inputMapping.second.keycode, tempPress);
									inputMapping.second.actions[i].pressType = tempPress;
									break;
								}

								if (i < inputMapping.second.actions.size() - 1)
									FL::GuiCore::MoveScreenCursor(0, -4);

								FL::GuiCore::MoveScreenCursor(0, 3);	
							}		
							
							if (keycodeToRemove != "")
							{
								currentContext->GetInputMappings().erase(keycodeToRemove);
								break;
							}
						}
					}

				// }
				ImGui::EndDisabled();

			// }
			ImGui::EndChild(); // Context Selection			
		}	
		
		FL::GuiCore::EndWindow(); // MappingContext Editor
	}
}