-- HurtBox.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     HurtBox[my_id] =
     {
		position = this_object:GetTransform():GetPosition()
     }
     local data = GetInstanceData("HurtBox", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("HurtBox", my_id)   
     -- LogString("Circle Col")  
end

function Update()
     local data = GetInstanceData("HurtBox", my_id)
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
     -- LogString("Circle Col")
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("HurtBox", my_id)
     -- LogString("Circle enter")
end

function OnBoxCollisionLeave(collidedWith)
     -- LogString("Circle leave")
     
end

function OnCircleCollision(collidedWith)
     
end

function OnCircleCollisionEnter(collidedWith)
     
     local data = GetInstanceData("HurtBox", my_id)

     local collidedPos = collidedWith:GetParent():GetTransform():GetPosition()
     local direction = Vector2:new(collidedPos:x() - data.position:x(), collidedPos:y() - data.position:y())
     collidedWith:GetParent():GetRigidBody():AddForce(direction:Normalize(), 1)
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