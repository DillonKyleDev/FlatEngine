-- Explosion.scp.lua

-- Use "this_object" to reference the object that owns this script


function Awake() 
     Explosion[my_id] =
     {
		-- Key value pairs here
     }
     local data = Explosion[my_id]
end

function Start()
     -- required to access instance data
     local data = Explosion[my_id]

     this_object:GetAnimation():Play("Explode")
end

function Update()
     local data = Explosion[my_id]
end

function DestroySelf()
     Destroy(my_id)
end

-- each of these functions must be present in each file otherwise other files copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
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