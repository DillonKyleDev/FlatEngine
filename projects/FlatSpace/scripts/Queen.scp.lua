-- Queen.scp.lua


function Awake() 
     Queen[my_id] = 
     {
          name = this_object:GetName(),
          currentHealth = GetScriptParam("health", my_id):int(),
          totalHealth = GetScriptParam("health", my_id):int(),
          healthBar = this_object:FindChildByName("Healthbar"),
          animation = this_object:GetAnimation(),
          boxCollider = this_object:GetBoxCollider(),
          transform = this_object:GetTransform(),
          id = my_id,          
          shots = {}
     }          
end 

function Start() 
     local data = GetInstanceData("Queen", my_id)
     UpdateQueenHealthbar(my_id)
     data.animation:Play("AttackPhase1")
end 

function Update() 
     local data = GetInstanceData("Queen", my_id)
     local bulletType = GetScriptParam("bulletType", my_id):string();

     for i, shot in ipairs(data.shots) do          
          if (shot.startTime ~= 0) and (shot.totalSpawned < shot.shots) then
               if GetTime() > (shot.startTime + (shot.frequency * shot.totalSpawned)) then                    
                    shot.totalSpawned = shot.totalSpawned + 1
                    local bullet = Instantiate(bulletType, data.transform:GetTruePosition())
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

function UpdateQueenHealthbar(id)
     local data = GetInstanceData("Queen", id)
     local healthSprite = data.healthBar:GetSprite()
     local spriteScale = healthSprite:GetScale()
     local healthScale = data.currentHealth / data.totalHealth
     healthSprite:SetScale(Vector2:new(healthScale, spriteScale:y()))
end

function DamageQueen(id, amount)
     local data = GetInstanceData("Queen", id)
     data.currentHealth = data.currentHealth - amount     
     UpdateQueenHealthbar(id)

     if data.currentHealth <= 0 then
          data.boxCollider:SetActive(false)
          data.animation:Play("ShipExplode")
          AddToScore(500)
          Die(id)
     end
end

function Die(id)     
     if (GetObjectByName("GameOverScreen"):IsActive() == false) then
          GetObjectByName("Music"):GetAudio():StopAll()
          GetObjectByName("GlobalSounds"):GetAudio():Play("Success")
          GetObjectByName("Player"):SetIgnore("EnemyDamage", true)
          GetObjectByName("Player"):SetIgnore("InteractableObject", true)
          scoreScreen = GetObjectByName("EndOfLevelScore")
          scoreScreen:SetActive(true)
     end
     
     Destroy(my_id)
end


function QueenShoot(shots)
     local data = GetInstanceData("Queen", my_id)

     local shot = { 
          shots = shots:int(),
          totalSpawned = 0,
          startTime = GetTime(),
          frequency = 400
     }
     
     data.shots[#data.shots + 1] = shot
end


-- each of these functions must be present in each file otherwise other files copies will be used with this object instead
function OnBoxCollision()
     local data = GetInstanceData("Queen", my_id)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("Queen", my_id)
     
     local collidedName = collidedWith:GetParent():GetName()   
     if (collidedWith:GetParent() and collidedWith:GetParent():HasTag("Player")) then          
          DamagePlayer(collidedWith:GetParentID(), 10)
     end
end

function OnBoxCollisionLeave()
     local data = GetInstanceData("Queen", my_id)
end

function OnCircleCollision()
     local data = GetInstanceData("Queen", my_id)
end

function OnCircleCollisionEnter()
     local data = GetInstanceData("Queen", my_id)
end

function OnCircleCollisionLeave()
     local data = GetInstanceData("Queen", my_id)
end

function OnButtonMouseOver()
     local data = GetInstanceData("Queen", my_id)
end

function OnButtonMouseEnter()
     local data = GetInstanceData("Queen", my_id)
end

function OnButtonMouseLeave()
     local data = GetInstanceData("Queen", my_id)
end

function OnButtonLeftClick()
     local data = GetInstanceData("Queen", my_id)
end

function OnButtonRightClick()
     local data = GetInstanceData("Queen", my_id)
end