#include "Inspector.h"
#include "GuiCore.h"
#include "managers/ProjectManager.h"

#include "Inspector.h"

namespace FL = FlatEngine;


namespace FlatGui
{
	void RenderPersistentScript(bool& b_show)
	{        
		if (!b_show)
			return;
		
        if (FL::GuiCore::BeginWindow("Persistent Scripts", b_show))
        {                      
			bool b_isActive = FL::ProjectManager::loadedProject.persistentScript.IsActive();
			if (Inspector::RenderIsActiveCheckbox(b_isActive))
				FL::ProjectManager::loadedProject.persistentScript.SetActive(b_isActive);
            FL::GuiCore::MoveScreenCursor(0,3);
            Inspector::RenderScriptComponent(&FL::ProjectManager::loadedProject.persistentScript);        			
		}		

		FL::GuiCore::EndWindow(); // Persistent Scripts
	}
}