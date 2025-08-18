-- GameManager.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     GameManager[my_id] =
     {
		score = 0,
          audio = this_object:GetAudio()
     }
     local data = GetInstanceData("GameManager", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("GameManager", my_id)     
end

function Update()
     local data = GetInstanceData("GameManager", my_id)

     if (data.score == 15) then
          data.audio:Play("Victory")
          this_object:GetText():SetActive(true)
          data.score = 0
     end

end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBeginCollision(collidedWith, manifold)
     local data = GetInstanceData("GameManager", my_id)
end

function OnEndCollision(collidedWith, manifold)
     local data = GetInstanceData("GameManager", my_id)
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