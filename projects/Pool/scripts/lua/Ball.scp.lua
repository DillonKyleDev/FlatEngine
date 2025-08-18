-- Ball.scp.lua
-- use "this_object" to reference the object that owns this script and "my_id" to access it's id


function Awake() 
     Ball[my_id] =
     {
		-- Key value pairs here
          gameManager = GetObjectByName("GameManager")
     }
     local data = GetInstanceData("Ball", my_id)

     local r = RandomFloat(0,1)
     local g = RandomFloat(0,1)
     local b = RandomFloat(0,1)
     local a = 1
     local color = Vector4:new(r,g,b,a)
     this_object:GetSprite():SetTintColor(color)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Ball", my_id)
end

function Update()
     local data = GetInstanceData("Ball", my_id)
end

function DestroySelf()
     Destroy(my_id);
end

-- each of these functions must be present in each Lua script file otherwise other script's implementations will be used with this object instead
function OnBeginCollision(collidedWith, manifold)
     local data = GetInstanceData("Ball", my_id)
     local relativeVelocity = SubtractVectors(collidedWith:GetLinearVelocity(), this_object:GetBody():GetLinearVelocity())
     local magnitude = ToInt(relativeVelocity:GetMagnitude() * 3)   

     if (collidedWith:GetParent():GetName() == "Ball" or collidedWith:GetParent():GetName() == "WhiteBall") then
          data.gameManager:GetAudio():SetEffectVolume("Hit1", magnitude)
          data.gameManager:GetAudio():Play("Hit1")
     elseif (collidedWith:GetParent():GetName() == "Walls") then
          data.gameManager:GetAudio():SetEffectVolume("Hit2", magnitude)
          data.gameManager:GetAudio():Play("Hit2")
     end


end

function OnEndCollision(collidedWith, manifold)
     local data = GetInstanceData("Ball", my_id)
end

function OnBeginSensorTouch(touched)
     local data = GetInstanceData("Ball", my_id)
end

function OnEndSensorTouch(touched)
     local data = GetInstanceData("Ball", my_id)
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