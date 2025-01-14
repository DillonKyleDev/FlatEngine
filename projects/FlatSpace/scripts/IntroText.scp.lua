-- IntroText


function Awake() 
     introText = GetObjectByName("LevelIntroText")
     if introText ~= NULL then
          introText:GetAnimation():Play("DisplayIntroText")
     end
end 

function Start()

end

function Update()
     
end