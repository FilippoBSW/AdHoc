require "AdHoc"

local this = GetThis()
local entities = {}
local rigidbodies = {}
local nextId = 0
local input = GetInput()

function Start()
end

function Update()
    -- if input:GetKey(AdHoc.Key.a) == true then
        entities[nextId] = CreateEntity()
        AddComponent(entities[nextId], "RigidBody", "Box");
        rigidbodies[nextId] = GetComponent(entities[nextId], "RigidBody")
        rigidbodies[nextId]:AddVelocity(0.0, 0.0, 10.0)
        nextId = nextId + 1
    -- end
end
