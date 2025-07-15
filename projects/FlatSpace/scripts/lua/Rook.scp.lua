-- Rook.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     Rook[my_id] =
     {
          name = this_object:GetName(),
          startTime = GetTime(),
          lifeTime = 5000,
          currentHealth = 50.0,
          totalHealth = 50.0,
          healthBar = this_object:FindChildByName("Healthbar"),
          transform = this_object:GetTransform(),
          animation = this_object:GetAnimation(),
          boxCollider = this_object:GetBoxCollider(),
          id = my_id,
          pointValue = 400,
          shots = {}
     }
     local data = GetInstanceData("Rook", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Rook", my_id)

     UpdateRookHealthbar(my_id)
     -- this_object:GetAnimation():Play("Shoot")
end

function Update()
     local data = GetInstanceData("Rook", my_id)

     if GetTime() >= (data.startTime + data.lifeTime) then          
          Destroy(my_id)
     end
     
     -- for i, shot in ipairs(data.shots) do          
     --      if (shot.startTime ~= 0) and (shot.totalSpawned < shot.shots) then
     --           if GetTime() > (shot.startTime + (shot.frequency * shot.totalSpawned)) then                    
     --                shot.totalSpawned = shot.totalSpawned + 1
     --                local bullet = Instantiate("EnemyBlasterRoundLight", data.transform:GetTruePosition())                    
     --                local rigidBody = bullet:GetRigidBody()
     --                rigidBody:AddForce(Vector2:new(0,-1), 10)
     --           end
     --      end

     --      if shot.totalSpawned == shot.numberToSpawn then
     --           shot.startTime = 0
     --           shot.totalSpawned = 0
     --      end
     -- end
end

function UpdateRookHealthbar(id)
     local data = GetInstanceData("Rook", id)
     local healthSprite = data.healthBar:GetSprite()
     local spriteScale = healthSprite:GetScale()
     local healthScale = data.currentHealth / data.totalHealth
     healthSprite:SetScale(Vector2:new(healthScale, spriteScale:y()))
end

function DamageRook(id, amount)
     local data = GetInstanceData("Rook", id)

     data.currentHealth = data.currentHealth - amount
     UpdateRookHealthbar(id)

     if data.currentHealth <= 0 then
          GetObjectById(id):GetRigidBody():SetPendingForces(Vector2:new(0,0))
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


function RookShoot(shots)
     local data = GetInstanceData("Rook", my_id)
     
     local shot = { 
          shots = shots:int(),
          totalSpawned = 0,
          startTime = GetTime(),
          frequency = 700
     }
     
     data.shots[#data.shots + 1] = shot
end

-- each of these functions must be present in each file otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("Rook", my_id)

     if (collidedWith:GetParent():HasTag("Player")) then          
          DamagePlayer(collidedWith:GetParentID(), 5)
     end
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