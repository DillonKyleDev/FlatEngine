-- Orbit.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     Orbit[my_id] =
     {
		transform = this_object:GetTransform()
     }
     local data = GetInstanceData("Orbit", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Orbit", my_id)     
end

function Update()
     local data = GetInstanceData("Orbit", my_id)

     data.transform:SetPosition(Vector2:new(10 * math.cos(GetTime() / 5000), 6 * math.sin(GetTime() / 5000)))
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("Orbit", my_id)
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