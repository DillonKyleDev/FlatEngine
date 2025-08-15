-- PlayerController.scp.lua


function Awake() 
    PlayerController[my_id] = 
    {
        -- gameScore = 0,
        -- scoreText = GetObjectByName("CurrentScore"),
        mappingContext = GetMappingContext("MC_Player"),
        transform = this_object:GetTransform(),
        body = this_object:GetBody(),
        clickPos = Vector2:new(),
        releasePos = Vector2:new()
    }    
end 

function Start()     
    local data = GetInstanceData("PlayerController", my_id)
    -- GetObjectByName("Music"):GetAudio():Play("ThemeMusic")
end 

function Update()     
    local data = GetInstanceData("PlayerController", my_id)
    handleMovement();
end 

function handleMovement()
    local data = GetInstanceData("PlayerController", my_id)

    if data.mappingContext:ActionPressed("IA_StartShot") then
        data.clickPos = GetMousePosWorld()
    end
    if data.mappingContext:ActionPressed("IA_Aiming") then
        SceneDrawLine(data.clickPos, GetMousePosWorld(), GetColor("white"), 1)
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