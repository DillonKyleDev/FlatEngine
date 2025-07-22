-- Character.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     Character[my_id] =
     {
		-- Key value pairs here
          rigidBody = this_object:GetRigidBody()
     }
     local data = GetInstanceData("Character", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Character", my_id)
     data.rigidBody:AddForce(Vector2:new(-1,-1), 10)
end

function Update()
     local data = GetInstanceData("Character", my_id)


end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("Character", my_id)
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
end

function OnButtonMouseLeave()
end

function OnButtonLeftClick()
end

function OnButtonRightClick()
end