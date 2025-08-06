-- Pocket.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     Pocket[my_id] =
     {
		
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

function OnBoxCollision(collidedWith)
end

function OnBeginCollision(collidedWith)
     local data = GetInstanceData("Pocket", my_id)
end

function OnEndCollision(collidedWith)
end
