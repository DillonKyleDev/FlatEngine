-- DestroyAfterTime.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     DestroyAfterTime[my_id] =
     {
		startTime = GetTime(),
          sprite = this_object:GetSprite()
     }
     local data = GetInstanceData("DestroyAfterTime", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("DestroyAfterTime", my_id)
end

function Update()
     local data = GetInstanceData("DestroyAfterTime", my_id)
     local timeTillDestruction = GetScriptParam("time", my_id):int() 

     local newAlpha = data.sprite:GetAlpha() - 0.01
     if (newAlpha >= 0) then
          data.sprite:SetAlpha(newAlpha)
     end
     
     if (GetTime() >= (data.startTime + 2000)) then 
          LogString("Destroying")         
          Destroy(my_id)
     end
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("DestroyAfterTime", my_id)
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