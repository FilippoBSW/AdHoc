require "AdHoc"

local this = GetThis()
local material = GetComponent(this, "Material")

function Start()
end

function Update()
end

function OnCollision(rhs)
    if rhs == AdHoc.Global.floor1ID then
        material.albedo.x = math.random()
        material.albedo.y = math.random()
        material.albedo.z = math.random()
    end
end

function OnCollisionExit(rhs)
    if rhs == AdHoc.Global.floor1ID then
        local m = GetComponent(rhs, "Material")
        m.albedo.x = math.random()
        m.albedo.y = math.random()
        m.albedo.z = math.random()
    end
end