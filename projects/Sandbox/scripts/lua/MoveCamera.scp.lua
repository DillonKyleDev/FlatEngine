-- MoveCamera.scp.lua
-- use "this_object" to reference the object that owns this script and "my_id" to access it's id


function Awake() 
     MoveCamera[my_id] =
     {
		cameraTransform = this_object:GetTransform()
     }
     local data = GetInstanceData("MoveCamera", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("MoveCamera", my_id)
end

function Update()
     local data = GetInstanceData("MoveCamera", my_id)
     
     local yPos = math.sin(GetTime() / 200) - 4
     local zPos = math.cos(GetTime() / 200) + 4
     
     data.cameraTransform:SetPosition(Vector3:new(-7, yPos, zPos))
end

-- each of these functions must be present in each Lua script file otherwise other script's implementations will be used with this object instead
function OnBeginCollision(collidedWith, manifold)
     local data = GetInstanceData("MoveCamera", my_id)
end

function OnEndCollision(collidedWith, manifold)
     local data = GetInstanceData("MoveCamera", my_id)
end

function OnBeginSensorTouch(touched)
     local data = GetInstanceData("MoveCamera", my_id)
end

function OnEndSensorTouch(touched)
     local data = GetInstanceData("MoveCamera", my_id)
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