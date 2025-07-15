-- BlasterRound.scp.lua


function Awake() 
     BlasterRound[my_id] = 
     {
          lifeTime = 1500,
          startTime = GetTime(),
          audio = this_object:GetAudio()
     }     
end 

function Start() 
     local data = GetInstanceData("BlasterRound", my_id)
     data.audio:Play("Fire")
end 

function Update() 
     local data = GetInstanceData("BlasterRound", my_id)
     if GetTime() >= (data.startTime + data.lifeTime) then
          Destroy(my_id)
     end
end 

function OnBoxCollision(collidedWith)
end
 
-- each of these functions must be present in each file otherwise other files copies will be used with this object instead
function OnBoxCollision()
     local data = GetInstanceData("BlasterRound", my_id)
end

function OnBoxCollisionEnter(collidedWith)
     local data = GetInstanceData("BlasterRound", my_id)

     local collidedName = collidedWith:GetParent():GetName()     
     data.audio:Play("Explode")
     Instantiate("Explosion", this_object:GetTransform():GetTruePosition())
     
     if collidedWith:GetParent():HasTag("Enemy") then
          if (collidedName == "Queen" or collidedName == "QueenDark") then          
               DamageQueen(collidedWith:GetParentID(), 5)         
          elseif (collidedName == "Pawn" or collidedName == "PawnDark") then
               DamagePawn(collidedWith:GetParentID(), 5)
          elseif (collidedName == "Bishop" or collidedName == "BishopDark") then
               DamageBishop(collidedWith:GetParentID(), 5)
          elseif (collidedName == "Knight") then
               DamageKnight(collidedWith:GetParentID(), 5)
          elseif (collidedName == "Rook") then
               DamageRook(collidedWith:GetParentID(), 5)
          end
     elseif (collidedName == "Asteroid" or collidedName == "Asteroid2" or collidedName == "Asteroid3") then
          DamageAsteroid(collidedWith:GetParentID(), 5)
     elseif (collidedName == "Satellite") then
          DamageSatellite(collidedWith:GetParentID(), 5)
     elseif (collidedName == "Transport2" or collidedName == "Transport3" or collidedName == "Transport4") then
          DamageTransport(collidedWith:GetParentID(), 5)
     end     

     Destroy(my_id)
end


function OnBoxCollisionLeave()
     local data = GetInstanceData("BlasterRound", my_id)
end

function OnCircleCollision()
     local data = GetInstanceData("BlasterRound", my_id)
end

function OnCircleCollisionEnter()
     local data = GetInstanceData("BlasterRound", my_id)
end

function OnCircleCollisionLeave()
     local data = GetInstanceData("BlasterRound", my_id)
end

function OnButtonMouseOver()
     local data = GetInstanceData("BlasterRound", my_id)
end

function OnButtonMouseEnter()
     local data = GetInstanceData("BlasterRound", my_id)
end

function OnButtonMouseLeave()
     local data = GetInstanceData("BlasterRound", my_id)
end

function OnButtonLeftClick()
     local data = GetInstanceData("BlasterRound", my_id)
end

function OnButtonRightClick()
     local data = GetInstanceData("BlasterRound", my_id)
end