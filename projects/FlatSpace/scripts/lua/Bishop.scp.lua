-- Bishop.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     Bishop[my_id] =
     {
          name = this_object:GetName(),
          startTime = GetTime(),
          lifeTime = 8000,
          currentHealth = 40.0,
          totalHealth = 40.0,
          healthBar = this_object:FindChildByName("Healthbar"),
          transform = this_object:GetTransform(),
          animation = this_object:GetAnimation(),
          boxCollider = this_object:GetBoxCollider(),
          id = my_id,
          pointValue = 200,
          shots = {}
     }
     local data = GetInstanceData("Bishop", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Bishop", my_id)

     UpdateBishopHealthbar(my_id)
     -- this_object:GetAnimation():Play("Shoot")
end

function Update()
     local data = GetInstanceData("Bishop", my_id)

     if GetTime() >= (data.startTime + data.lifeTime) then          
          Destroy(my_id)
     end

     for i, shot in ipairs(data.shots) do          
          if (shot.startTime ~= 0) and (shot.totalSpawned < shot.shots) then
               if GetTime() > (shot.startTime + (shot.frequency * shot.totalSpawned)) then                    
                    shot.totalSpawned = shot.totalSpawned + 1
                    local bullet
                    if (data.name == "BishopDark") then
                         bullet = Instantiate("EnemyBlasterRoundLight", data.transform:GetTruePosition())
                    else
                         bullet = Instantiate("EnemyBlasterRound", data.transform:GetTruePosition())
                    end
                    local rigidBody = bullet:GetRigidBody()
                    rigidBody:AddForce(Vector2:new(0,-1), 10)
               end
          end

          if shot.totalSpawned == shot.numberToSpawn then
               shot.startTime = 0
               shot.totalSpawned = 0
          end
     end
end

function UpdateBishopHealthbar(id)
     local data = GetInstanceData("Bishop", id)
     local healthSprite = data.healthBar:GetSprite()
     local spriteScale = healthSprite:GetScale()
     local healthScale = data.currentHealth / data.totalHealth
     healthSprite:SetScale(Vector2:new(healthScale, spriteScale:y()))
end

function DamageBishop(id, amount)
     local data = GetInstanceData("Bishop", id)

     data.currentHealth = data.currentHealth - amount
     UpdateBishopHealthbar(id)

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


function BishopShoot(shots)
     local data = GetInstanceData("Bishop", my_id)
     
     local shot = { 
          shots = shots:int(),
          totalSpawned = 0,
          startTime = GetTime(),
          frequency = 400
     }
     
     data.shots[#data.shots + 1] = shot
end

-- each of these functions must be present in each file otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("Bishop", my_id)

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