-- SpawnItems.scp.lua
-- use "this_object" to reference the object that owns this script and "my_id" to access it's id


function Awake() 
     SpawnItems[my_id] =
     {
		-- Key value pairs here
     }
     local data = GetInstanceData("SpawnItems", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("SpawnItems", my_id)

     for i = 0, 300, 1
     do
          -- local object = CreateGameObject()
          -- local mesh = object:AddMesh()
          -- mesh:AddTexture("../engine/images/colors/green.png", 0)
          -- mesh:SetMaterial("Unlit")
          -- mesh:SetModel("../engine/models/monkey.obj")
          -- mesh:CreateResources()
          -- local transform = object:GetTransform()
          -- local pos = Vector3:new(RandomFloat(-10.0,10.0),RandomFloat(-10.0,10.0),RandomFloat(-10.0,10.0))
          -- transform:SetPosition(pos)
          -- local force = Vector2:new(RandomFloat(-500.0,500.0),RandomFloat(0.0,0.0))
          local ball = Instantiate("IcoSphere", Vector3:new(0,0,2 *i))
          ball:GetMesh():SetUBOVec4("Color", Vector4:new(RandomFloat(0.0, 1.0),RandomFloat(0.0, 1.0),RandomFloat(0.0, 1.0),1.0))
          -- ball:GetBody():ApplyForceToCenter(force)
     end
end

function Update()
     local data = GetInstanceData("SpawnItems", my_id)
end

-- each of these functions must be present in each Lua script file otherwise other script's implementations will be used with this object instead
function OnBeginCollision(collidedWith, manifold)
     local data = GetInstanceData("SpawnItems", my_id)
end

function OnEndCollision(collidedWith, manifold)
     local data = GetInstanceData("SpawnItems", my_id)
end

function OnBeginSensorTouch(touched)
     local data = GetInstanceData("SpawnItems", my_id)
end

function OnEndSensorTouch(touched)
     local data = GetInstanceData("SpawnItems", my_id)
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