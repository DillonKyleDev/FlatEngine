#include "Inspector.h"
#include "GuiCore.h"
#include "managers/ProjectManager.h"

#include "Inspector.h"

namespace FL = FlatEngine;


namespace FlatGui
{
	void RenderPersistantScript(bool& b_show)
	{        
		if (!b_show)
			return;
		
        if (FL::GuiCore::BeginWindow("Persistant Scripts", b_show))
        {                      
            FL::GuiCore::MoveScreenCursor(0,3);
            Inspector::RenderScriptComponent(&FL::ProjectManager::loadedProject.persistantScript);        			
		}		

		FL::GuiCore::EndWindow(); // Persistant Scripts
	}
}