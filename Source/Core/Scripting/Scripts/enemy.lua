require "AdHoc"

local this = GetThis()
local m = GetComponent(this, "Material")
local r = GetComponent(this, "RigidBody")

local timer = 0

function Start()
end

function Update()
    if timer < 500 then
        r:AddVelocity(0, 0, -2 * DeltaTime())
    elseif timer < 1000 then
       r:AddVelocity(0, 0, 2 * DeltaTime())
    else
        timer = 0;
    end
    timer = timer + 1;
end

function OnCollisionEnter(rhs)
    m.albedo.x = math.random()
    m.albedo.y = math.random()
    m.albedo.z = math.random()
end