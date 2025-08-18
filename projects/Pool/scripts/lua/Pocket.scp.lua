-- Pocket.scp.lua
-- use "this_object" to reference the object that owns this script and "my_id" to access it's id


function Awake() 
     Pocket[my_id] =
     {
          gameManager = GetObjectByName("GameManager")
     }
     local data = GetInstanceData("Pocket", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Pocket", my_id)
end

function Update()
     local data = GetInstanceData("Pocket", my_id)
end

-- each of these functions must be present in each Lua script file otherwise other script's implementations will be used with this object instead
function OnBeginCollision(collidedWith, manifold)
   
end

function OnEndCollision(collidedWith, manifold)
     local data = GetInstanceData("Pocket", my_id)
end

function OnBeginSensorTouch(touched)
     local data = GetInstanceData("Pocket", my_id)
     local touchedParent = touched:GetParent()    

     if (touchedParent:HasComponent("Body")) then
          -- if (touchedParent:GetName() == "WhiteBall") then
          --      ResetWhiteBall()
          -- end                    
          data.gameManager:GetAudio():Play("BallIn")
          touchedParent:GetAnimation():Play("Disappear")
          local gameData = GetInstanceData("GameManager", data.gameManager:GetID())
          gameData.score = gameData.score + 1
     end
end

function OnEndSensorTouch(touched)      
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