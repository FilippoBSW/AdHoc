require "AdHoc"

local this = GetThis()
local material = GetComponent(this, "Material")
local rigidbody = GetComponent(this, "RigidBody")

local input = GetInput()

function Start()
end

function Update()
    if input:GetKey(AdHoc.Key.d) == true then
        rigidbody:AddVelocity(10.0 * DeltaTime(),0.0,0.0)
    elseif input:GetKey(AdHoc.Key.a) == true then
        rigidbody:AddVelocity(-10.0 * DeltaTime(),0.0,0.0)
    end
end

function OnCollision(rhs)
    if rhs == AdHoc.Global.floor1ID then
        material.albedo.x = math.random()
        material.albedo.y = math.random()
        material.albedo.z = math.random()
    end
end