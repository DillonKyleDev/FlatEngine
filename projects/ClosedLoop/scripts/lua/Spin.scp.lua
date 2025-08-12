-- Spin.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     Spin[my_id] =
     {
		body = this_object:GetBody()
     }
     local data = GetInstanceData("Spin", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Spin", my_id)
     -- LogString("Spin : Start() called on "..this_object:GetName())
     data.body:ApplyTorque(-10000)
end

function Update()
     local data = GetInstanceData("Spin", my_id)
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBeginCollision(collidedWith, manifold)
     local data = GetInstanceData("Spin", my_id)
end

function OnEndCollision(collidedWith, manifold)
     local data = GetInstanceData("Spin", my_id)
end

function OnBeginSensorTouch(touched)
     local data = GetInstanceData("Spin", my_id)
end

function OnEndSensorTouch(touched)
     local data = GetInstanceData("Spin", my_id)
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
--elseif

--end

--Lua for loops:
--for init, min/max value, increment
do

end
--example:
for i = 0, 10, 1
do

LogInt(i)
end