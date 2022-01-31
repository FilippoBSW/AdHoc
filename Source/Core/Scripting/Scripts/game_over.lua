require "AdHoc"

local this = GetThis()

function Start()
end

function Update()
end

function OnTriggerEnter(rhs)
    if rhs == AdHoc.Global.playerID then
        LogMessage("Game over!")
    end
end