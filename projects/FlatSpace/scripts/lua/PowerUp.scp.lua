-- PowerUp.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     PowerUp[my_id] =
     {
          startTime = GetTime(),
          lifeTime = 10000,
          audio = this_object:GetAudio()
     }
     local data = GetInstanceData("PowerUp", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("PowerUp", my_id)
end

function Update()
     local data = GetInstanceData("PowerUp", my_id)

     if GetTime() >= (data.startTime + data.lifeTime) then          
          Destroy(my_id)
     end
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("PowerUp", my_id)

     if (collidedWith:GetParent():HasTag("Player")) then          
          PowerUpPlayer(collidedWith:GetParentID());
     end

     data.audio:Play("Collected")
     Destroy(my_id)
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