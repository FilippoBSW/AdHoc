require "AdHoc"

local this = GetThis()

function Start()
end

function Update()
end

function OnTriggerEnter(rhs)
    if rhs == AdHoc.Global.ball then
        LogMessage("Score!")
    end
end