-- Strip.scp.lua
-- use "this_object" to reference the object that owns this script and "my_id" to access it's id

function Lerp(a, b, speed)
     local difference = b - a
     return a + (difference * speed)
end


function Awake() 
     Strip[my_id] =
     {
		mesh = this_object:GetMesh(),
          moveByList = {},
          min = 1.1,
          max = 1.15,
          lerpSpeed = 0.5,
          currentStep = 0,
          initialized = false,
          radius = 0,
          xChange = 0,
          yChange = 0,
          zChange = 0
     }
     local data = GetInstanceData("Strip", my_id)
end

function Start()
     -- required to access instance data
     local data = GetInstanceData("Strip", my_id)

     for i = 0, 14, 1 do
          local vec4Name = "vert"..i.."Move"

          data.moveByList[i] = Vector4:new(0,0,0,0)  
          data.mesh:SetUBOVec4(vec4Name, data.moveByList[i])
     end

     -- data.moveByList[15] = Vector4:new(RandomFloat(data.min, data.max), RandomFloat(data.min, data.max), RandomFloat(data.min, data.max), 0)
     local x = math.sin(GetTime() / 50) * data.radius
     local y = math.cos(GetTime() / 50) * data.radius
     local z = Lerp(0, RandomFloat(data.min, data.max), data.lerpSpeed)

     data.moveByList[15] = Vector4:new(x, y, 0, 0)
     data.mesh:SetUBOVec4("vert15Move", data.moveByList[15])
end

function Update()
     local data = GetInstanceData("Strip", my_id)

     data.mesh:SetUBOVec4("Disturbance",  Vector4:new(GetTime() / 10000, 0.0, 0.0, 0.0))
     local rateLimit = 1          

     if (data.currentStep >= rateLimit or data.initialized == false) then
          data.xChange = RandomFloat(data.min, data.max)
          data.yChange = RandomFloat(data.min, data.max)
          data.zChange = RandomFloat(data.min, data.max)
          data.currentStep = 0
          data.initialized = true
     end

     for i = 0, 14, 1 do
          local vec4Name = "vert"..i.."Move"
     
          data.moveByList[i] = data.moveByList[i + 1]
          data.mesh:SetUBOVec4(vec4Name, data.moveByList[i])
     end     
     data.moveByList[15] = Vector4:new(math.sin(GetTime() / 25) * data.radius, math.cos(GetTime() / 25) * data.radius, math.sin(GetTime() / 50) * data.radius, 0)
     -- data.moveByList[15] = Vector4:new(Lerp(data.moveByList[15].x, data.moveByList[15].x + data.xChange, data.lerpSpeed), Lerp(data.moveByList[15].y, data.moveByList[15].y + data.yChange, data.lerpSpeed), Lerp(data.moveByList[15].z, data.moveByList[15].z + data.zChange, data.lerpSpeed), 0)
     data.mesh:SetUBOVec4("vert15Move", data.moveByList[15])

     data.currentStep = data.currentStep + 1;

     if (data.radius <= 1) then
          data.radius = data.radius + 0.05
     end
end

-- each of these functions must be present in each Lua script file otherwise other script's implementations will be used with this object instead
function OnBeginCollision(collidedWith, manifold)
     local data = GetInstanceData("Strip", my_id)
end

function OnEndCollision(collidedWith, manifold)
     local data = GetInstanceData("Strip", my_id)
end

function OnBeginSensorTouch(touched)
     local data = GetInstanceData("Strip", my_id)
end

function OnEndSensorTouch(touched)
     local data = GetInstanceData("Strip", my_id)
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


--Lua cheatsheet

--Lua if statements:
--if (test) then
-- <do something>
--elseif
-- <do something>
--end

--Lua for loops:
--for init, min/max value, increment
--do
-- <do something>
--end


--example:
--for i = 0, 10, 1
--do
--LogInt(i)
--end