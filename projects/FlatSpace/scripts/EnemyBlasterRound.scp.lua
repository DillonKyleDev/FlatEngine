-- EnemyBlasterRound.scp.lua


function Awake() 
     EnemyBlasterRound[my_id] = 
     {
          lifeTime = 1500,
          startTime = GetTime(),
          audio = this_object:GetAudio()
     }     
end 

function Start() 
     local data = EnemyBlasterRound[my_id]
     data.audio:Play("Fire")
end 

function Update() 
     local data = EnemyBlasterRound[my_id]
     if GetTime() >= (data.startTime + data.lifeTime) then
          Destroy(my_id)
     end
end 

function OnBoxCollision(collidedWith)
end
 
-- each of these functions must be present in each file otherwise other files copies will be used with this object instead
function OnBoxCollision()
     local data = EnemyBlasterRound[my_id]
end

function OnBoxCollisionEnter(collidedWith)
     local data = EnemyBlasterRound[my_id]
     
     data.audio:Play("Explode")
     Instantiate("Explosion", this_object:GetTransform():GetTruePosition())
     
     if collidedWith:GetParent():HasTag("Player") then                 
          DamagePlayer(collidedWith:GetParentID(), 5)          
     end     

     Destroy(my_id)
end

function OnBoxCollisionLeave()
     local data = EnemyBlasterRound[my_id]
end

function OnCircleCollision()
     local data = EnemyBlasterRound[my_id]
end

function OnCircleCollisionEnter()
     local data = EnemyBlasterRound[my_id]
end

function OnCircleCollisionLeave()
     local data = EnemyBlasterRound[my_id]
end

function OnButtonMouseOver()
     local data = EnemyBlasterRound[my_id]
end

function OnButtonMouseEnter()
     local data = EnemyBlasterRound[my_id]
end

function OnButtonMouseLeave()
     local data = EnemyBlasterRound[my_id]
end

function OnButtonLeftClick()
     local data = EnemyBlasterRound[my_id]
end

function OnButtonRightClick()
     local data = EnemyBlasterRound[my_id]
end