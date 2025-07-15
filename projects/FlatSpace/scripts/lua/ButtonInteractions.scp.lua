-- ButtonInteractions.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     ButtonInteractions[my_id] =
     {
          buttonBorder = this_object:FindChildByName("Border"):GetSprite(),
          globalSounds = GetObjectByName("GlobalSounds"):GetAudio()
     }
     local data = GetInstanceData("ButtonInteractions", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("ButtonInteractions", my_id)
end

function Update()
     local data = GetInstanceData("ButtonInteractions", my_id)
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("ButtonInteractions", my_id)
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
     local data = GetInstanceData("ButtonInteractions", my_id)

     data.globalSounds:Play("Hover")
     if (data.buttonBorder ~= nil) then
          data.buttonBorder:SetAlpha(1.0)
     end
end

function OnButtonMouseLeave()
     local data = GetInstanceData("ButtonInteractions", my_id)

     if (data.buttonBorder ~= nil) then
          data.buttonBorder:SetAlpha(0)
     end
end

function OnButtonLeftClick()
end

function OnButtonRightClick()
end