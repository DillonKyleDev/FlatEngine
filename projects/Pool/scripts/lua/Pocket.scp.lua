-- Pocket.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     Pocket[my_id] =
     {
		collider = this_object:GetRigidBody()
     }
     local data = GetInstanceData("Pocket", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Pocket", my_id)
     LogString("Pocket : Start() called on "..this_object:GetName())
end

function Update()
     local data = GetInstanceData("Pocket", my_id)
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("Pocket", my_id)
end

function OnBoxCollisionLeave(collidedWith)
end

function OnCircleCollision(collidedWith)
end

function OnCircleCollisionEnter(collidedWith)
     -- LogString(collidedWith:GetParent():GetName())
     -- Destroy(collidedWith:GetParent():GetID())
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