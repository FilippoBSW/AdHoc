require "AdHoc"

local this = GetThis()
local input = GetInput()
local transform
local rigidBody
local rot = 0

function Start()
    transform = GetComponent(this, "Transform")
    rigidBody = GetComponent(this, "RigidBody")
end

function Update()
    rigidBody:SetRotation(0, math.rad(rot), 0)
    rot = rot + 80 * DeltaTime()
    local e = Raycast(transform.translate, transform:GetForward(), 40)
    if e ~= 0 then
    local m = GetComponent(e, "Material")
    m.albedo.x = math.random()
    m.albedo.y = math.random()
    m.albedo.z = math.random()
    end
end
