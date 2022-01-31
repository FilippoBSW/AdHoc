require "AdHoc"

local this = GetThis()

function Start()
end

function Update()
end

function OnCollisionEnter(rhs)
    local m = GetComponent(rhs, "Material")
    m.albedo.x = math.random()
    m.albedo.y = math.random()
    m.albedo.z = math.random()
end
