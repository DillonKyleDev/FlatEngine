-- Pawn.scp.lua


function Awake() 
     Pawn[my_id] = 
     {
          name = this_object:GetName(),
          startTime = GetTime(),
          lifeTime = 5000,
          currentHealth = 20.0,
          totalHealth = 20.0,
          healthBar = this_object:FindChildByName("Healthbar"),
          animation = this_object:GetAnimation(),
          boxCollider = this_object:GetBoxCollider(),
          id = my_id,
          pointValue = 100
     }     
end 

function Start() 
     local data = GetInstanceData("Pawn", my_id)
     UpdatePawnHealthbar(my_id)
end 

function Update() 
     local data = GetInstanceData("Pawn", my_id)

     if GetTime() >= (data.startTime + data.lifeTime) then          
          Destroy(my_id)
     end
end 

function UpdatePawnHealthbar(id)
     local data = GetInstanceData("Pawn", id)
     local healthSprite = data.healthBar:GetSprite()
     local spriteScale = healthSprite:GetScale()
     local healthScale = data.currentHealth / data.totalHealth
     healthSprite:SetScale(Vector2:new(healthScale, spriteScale:y()))
end

function DamagePawn(id, amount)
     local data = GetInstanceData("Pawn", id)

     data.currentHealth = data.currentHealth - amount
     UpdatePawnHealthbar(id)

     if data.currentHealth <= 0 then
          GetObjectByID(id):GetRigidBody():SetPendingForces(Vector2:new(0,0))
          data.boxCollider:SetActive(false)
          data.animation:Play("ShipExplode")   
          AddToScore(data.pointValue)       
     end
end

function Die(id)
     Destroy(id)
end

function Die()
     Destroy(my_id)
end

-- each of these functions must be present in each file otherwise other files copies will be used with this object instead
function OnBoxCollision()
     local data = GetInstanceData("Pawn", my_id)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("Pawn", my_id)

     if (collidedWith:GetParent():HasTag("Player")) then          
          DamagePlayer(collidedWith:GetParentID(), 5)
     end
end

function OnBoxCollisionLeave()
     local data = GetInstanceData("Pawn", my_id)
end

function OnCircleCollision()
     local data = GetInstanceData("Pawn", my_id)
end

function OnCircleCollisionEnter()
     local data = GetInstanceData("Pawn", my_id)
end

function OnCircleCollisionLeave()
     local data = GetInstanceData("Pawn", my_id)
end

function OnButtonMouseOver()
     local data = GetInstanceData("Pawn", my_id)
end

function OnButtonMouseEnter()
     local data = GetInstanceData("Pawn", my_id)
end

function OnButtonMouseLeave()
     local data = GetInstanceData("Pawn", my_id)
end

function OnButtonLeftClick()
     local data = GetInstanceData("Pawn", my_id)
end

function OnButtonRightClick()
     local data = GetInstanceData("Pawn", my_id)
end