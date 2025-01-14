-- GameOverScreen.scp.lua

-- Use "this_object" to reference the object that owns this script


function Awake() 
     GameOverScreen[my_id] =
     {
		-- Key value pairs here
     }
     local data = GameOverScreen[my_id]
end

function Start()
     -- required to access instance data
     local data = GameOverScreen[my_id]

     LogString("GameOverScreen : Start() called on "..this_object:GetName())
end

function Update()
     local data = GameOverScreen[my_id]
end

-- each of these functions must be present in each file otherwise other files copies will be used with this object instead
function OnBoxCollision()
end

function OnBoxCollisionEnter()
end

function OnBoxCollisionLeave()
end

function OnCircleCollision()
end

function OnCircleCollisionEnter()
end

function OnCircleCollisionLeave()
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