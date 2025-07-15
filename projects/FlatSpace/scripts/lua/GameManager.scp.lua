-- GameManager.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     GameManager[0] =
     {
		currentLevel = 1,
          gameScore = 0,
          audios = this_object:GetAudio()
     }

     local data = GetInstanceData("GameManager", 0)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("GameManager", 0)
end

function Update()
     local data = GetInstanceData("GameManager", 0)
end

function GetCurrentLevel()
     local data = GetInstanceData("GameManager", 0)

     return data.currentLevel
end

function LoadLevel(levelNumber)     
     local data = GetInstanceData("GameManager", 0)
     data.currentLevel = levelNumber:int()
     LoadScene("Level"..levelNumber:int())
end

function LoadMainMenu()     
     LoadScene("MainMenu")
end

function AddToScore(amount)
     local data = GetInstanceData("GameManager", 0)
     data.gameScore = data.gameScore + amount
     GetObjectByName("CurrentScore"):GetText():SetText(IntToString(data.gameScore)) 
     GetObjectByName("Score"):GetText():SetText(IntToString(data.gameScore))
end

 function ResetScore(amount)
     local data = GetInstanceData("GameManager", 0)
     data.gameScore = 0
     GetObjectByName("CurrentScore"):GetText():SetText(IntToString(data.gameScore)) 
     GetObjectByName("Score"):GetText():SetText(IntToString(data.gameScore))
end

function DestroySelf()
     Destroy(this_object:GetId())
end

-- each of these functions must be present in each file otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
end

function OnBoxCollisionLeave(collidedWith)
end

function OnCircleCollision(collidedWith)
end

function OnCircleCollisionEnter(collidedWith)
end

function OnCircleCollisionLeave(collidedWith)
end

function OnButtonMouseOver()
end

function OnButtonMouseEnter()
end

function OnButtonMouseLeave()
end

function OnButtonLeftClick()
end

function OnButtonRightClick()
end