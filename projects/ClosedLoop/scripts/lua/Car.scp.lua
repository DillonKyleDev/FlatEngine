-- Car.scp.lua

-- Use "this_object" to reference the object that owns this script and "my_id" to access its id


function Awake() 
     Car[my_id] =
     {
          animation = this_object:GetAnimation()
     }
     local data = GetInstanceData("Car", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Car", my_id)     
     -- data.animation:Play("explode")
end

function Update()
     local data = GetInstanceData("Car", my_id)
end

-- each of these functions must be present in each file if they are to be called otherwise other scripts copies will be used with this object instead
function OnBeginCollision(collidedWith, manifold)
     local data = GetInstanceData("Car", my_id)     

     if (manifold:GetPoints():size() == 2) then          
          for i = 1, manifold:GetPoints():size(), 1 do
               -- LogFloat(manifold:GetPoints()[i].normalVelocity)
          end          
     end

end

function OnEndCollision(collidedWith, manifold)
     local data = GetInstanceData("Car", my_id)     
end

function OnBeginSensorTouch(touched)
     LogString("Sensor touched " .. touched:GetParent():GetName())
end

function OnEndSensorTouch(touched)
     LogString("Sensor stopped touching " .. touched:GetParent():GetName())
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