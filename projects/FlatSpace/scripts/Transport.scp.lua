-- Transport.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     Transport[my_id] =
     {
		lastSpawnTime = 0,
          delay = 100,
          startTime = GetTime(),
          lifeTime = 5000,
          currentHealth = GetScriptParam("health", my_id):int(),
          totalHealth = GetScriptParam("health", my_id):int(),
          transform = this_object:GetTransform(),
          engineTransform = this_object:FindChildByName("Engines"):GetTransform(),
          rigidBody = this_object:GetRigidBody(),
          animation = this_object:GetAnimation(),
          boxCollider = this_object:GetBoxCollider()
     }
     local data = GetInstanceData("Transport", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Transport", my_id)
     data.rigidBody:AddForce(Vector2:new(-1, 0), 3)
end

function Update()
     local data = GetInstanceData("Transport", my_id)

     if GetTime() >= (data.lastSpawnTime + data.delay) then          
          data.lastSpawnTime = GetTime()
          local smoke = Instantiate("Smoke", data.engineTransform:GetTruePosition())
          smoke:GetRigidBody():AddForce(Vector2:new(1, 0), 6)
     end

     if GetTime() >= (data.startTime + data.lifeTime) then          
          Destroy(my_id)
     end
end

function DamageTransport(id, amount)
     local data = GetInstanceData("Transport", id)

     data.currentHealth = data.currentHealth - amount

     if data.currentHealth <= 0 then
          GetObjectByID(id):GetRigidBody():SetPendingForces(Vector2:new(0,0))
          data.boxCollider:SetActive(false)
          data.transform:SetScale(Vector2:new(2,2))
          data.animation:Play("Explode")   
          -- AddToScore(data.pointValue) 
          
          local dropChance = RandomInt(0, 10)          
          local whichDrop = RandomInt(0, 10) 
          local randFloatX = RandomFloat(-1, 1)
          local randFloatY = RandomFloat(-1, 1)          

          if (dropChance < 6) then
               if (whichDrop < 4) then
                    local healthPack = Instantiate("HealthPack", data.transform:GetTruePosition())
                    healthPack:GetRigidBody():AddForce(Vector2:new(randFloatX, randFloatY), 1)
               else
                    local powerUp = Instantiate("PowerUp", data.transform:GetTruePosition())
                    powerUp:GetRigidBody():AddForce(Vector2:new(randFloatX, randFloatY), 1)
               end
          end
     end
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBoxCollision(collidedWith)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("Transport", my_id)
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