-- PlayerController.scp.lua


function Awake() 
    PlayerController[my_id] = 
    {
        gameManager = GetObjectByName("GameManager"),
        mappingContext = GetMappingContext("MC_Player"),
        transform = this_object:GetTransform(),
        body = this_object:GetBody(),
        clickPos = Vector2:new(),
        releasePos = Vector2:new(),
    }    
end 

function Start()     
    local data = GetInstanceData("PlayerController", my_id)
end 

function Update()     
    local data = GetInstanceData("PlayerController", my_id)
    handleMovement();
end 

function handleMovement()
    local data = GetInstanceData("PlayerController", my_id)

    if data.mappingContext:ActionPressed("IA_BeginShot") then
        data.clickPos = GetMousePosWorld()
    end
    if data.mappingContext:ActionPressed("IA_Aiming") then
        local difference = SubtractVectors(data.clickPos, GetMousePosWorld())
        local endDraw = AddVectors(data.transform:GetPosition(), difference)
        DrawLineInGame(data.transform:GetPosition(), endDraw, GetColor("white"), 4)
    end
    if data.mappingContext:ActionPressed("IA_ReleaseShot") then
        local forceMultiplier = 100
        local force = Vector2:new((data.clickPos.x - GetMousePosWorld().x) * forceMultiplier, (data.clickPos.y - GetMousePosWorld().y) * forceMultiplier)        
        data.body:ApplyForce(force, data.clickPos)
    end
end

-- each of these functions must be present in each Lua script file otherwise other script's implementations will be used with this object instead
function OnBeginCollision(collidedWith, manifold)
    local data = GetInstanceData("PlayerController", my_id)
    local relativeVelocity = SubtractVectors(collidedWith:GetLinearVelocity(), data.body:GetLinearVelocity())
    local magnitude = ToInt(relativeVelocity:GetMagnitude() * 3)  

    if (collidedWith:GetParent():GetName() == "Walls") then
        data.gameManager:GetAudio():SetEffectVolume("Hit3", magnitude)
        data.gameManager:GetAudio():Play("Hit3")
    end
end

function OnEndCollision(collidedWith, manifold)
    local data = GetInstanceData("PlayerController", my_id)
end

function OnBeginSensorTouch(touched)
    local data = GetInstanceData("PlayerController", my_id)
end

function OnEndSensorTouch(touched)
    local data = GetInstanceData("PlayerController", my_id)
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