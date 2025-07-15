-- PropSpawner.scp.lua


function Awake() 
     PropSpawner[my_id] = 
     {
          animation = this_object:GetAnimation(),
          rigidBody = RigidBody:new(),
          waveTracker = 1,
          spawnWaves = {}
     }    
end 

function Start() 
     local data = GetInstanceData("PropSpawner", my_id)

     if (ContainsData("GameManager", 0)) then
          ResetScore()
     end

     data.animation:Play("Level")
end 

function Update() 
     local data = GetInstanceData("PropSpawner", my_id)

     for i, wave in ipairs(data.spawnWaves) do
          if (wave.startTime ~= 0) and (wave.totalSpawned < wave.numberToSpawn) then
               if GetTime() > (wave.startTime + (wave.frequency * wave.totalSpawned)) then
                    wave.spawnPos = GetObjectByName(wave.locationToSpawn):GetTransform():GetTruePosition()
                    wave.totalSpawned = wave.totalSpawned + 1
                    local spawnedMob = Instantiate(wave.mobToSpawn, wave.spawnPos)                    
                    wave.rigidBody = spawnedMob:GetRigidBody()
                    wave.rigidBody:AddForce(wave.direction, 4)

                    if (spawnedMob:GetName() == "Asteroid") then
                         SetAsteroidLifeTime(spawnedMob:GetID(), wave.frequency)  
                    elseif (spawnedMob:GetName() == "Satellite") then
                         SetSatelliteLifeTime(spawnedMob:GetID(), wave.frequency)  
                    end                    
               end
          end

          if wave.totalSpawned == wave.numberToSpawn then
               wave.startTime = 0
               wave.totalSpawned = 0
          end
     end
end 

function SpawnWave(numberOfEnemies, nameOfEnemy, spawnLocation, spawnTrajectory, spawnFrequency, isBool)
     local data = GetInstanceData("PropSpawner", my_id)

     local wave = {
          numberToSpawn = numberOfEnemies:int(),
          totalSpawned = 0,
          mobToSpawn = nameOfEnemy:string(),
          locationToSpawn = spawnLocation:string(),
          startTime = GetTime(),
          frequency = spawnFrequency:int(),
          direction = spawnTrajectory:Vector2()
     }
     data.spawnWaves[data.waveTracker] = wave
     data.waveTracker = data.waveTracker + 1
end