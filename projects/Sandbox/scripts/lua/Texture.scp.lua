-- Texture.scp.lua
-- use "this_object" to reference the object that owns this script and "my_id" to access it's id


function Awake() 
     Texture[my_id] =
     {
		mesh = this_object:GetMesh(),
          transform = this_object:GetTransform(),
          sceneViewCameraObject = GetSceneViewCameraObject()
     }
     local data = GetInstanceData("Texture", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Texture", my_id)
end

function Update()
     local data = GetInstanceData("Texture", my_id)

     data.mesh:SetUBOVec4("Disturbance",  Vector4:new(GetTime() / 5000, 0.0, 0.0, 0.0))
     lookAt = data.sceneViewCameraObject:GetTransform():GetPosition()
     -- data.transform:LookAt(lookAt)
     -- local scale = data.transform:GetScale()
     -- scale:SetX(scale.x + 0.001)
     -- scale:SetY(scale.y + 0.001)
     -- data.transform:SetScale(scale)
end

-- each of these functions must be present in each Lua script file otherwise other script's implementations will be used with this object instead
function OnBeginCollision(collidedWith, manifold)
     local data = GetInstanceData("Texture", my_id)
end

function OnEndCollision(collidedWith, manifold)
     local data = GetInstanceData("Texture", my_id)
end

function OnBeginSensorTouch(touched)
     local data = GetInstanceData("Texture", my_id)
end

function OnEndSensorTouch(touched)
     local data = GetInstanceData("Texture", my_id)
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


--Lua cheatsheet

--Lua if statements:
--if (test) then
-- <do something>
--elseif
-- <do something>
--end

--Lua for loops:
--for init, min/max value, increment
--do
-- <do something>
--end


--example:
--for i = 0, 10, 1
--do
--LogInt(i)
--end