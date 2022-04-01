require "AdHoc"

local this = GetThis()
AdHoc.Global.playerID = this
local rigidbody = GetComponent(this, "RigidBody")
local transform = GetComponent(this, "Transform")
local input = GetInput()
local entities = {}
local rigidbodies = {}
local meshes = {}
local transforms = {}
local nextId = 0
local isGrounded = false
local force = 0
local forward

function Start()
end

function Update()
    if input:GetKey(AdHoc.Key.d) == true then
        rigidbody:AddVelocity(10 * DeltaTime(), 0, 0)
        rigidbody:SetRotation(0, math.rad(90), 0)
    elseif input:GetKey(AdHoc.Key.a) == true then
        rigidbody:AddVelocity(-10 * DeltaTime(), 0, 0)
        rigidbody:SetRotation(0, math.rad(270), 0)
    elseif input:GetKey(AdHoc.Key.w) == true then
        rigidbody:AddVelocity(0, 0, 10 * DeltaTime())
        rigidbody:SetRotation(0, math.rad(0), 0)
    elseif input:GetKey(AdHoc.Key.s) == true then
        rigidbody:AddVelocity(0, 0, -10 * DeltaTime())
        rigidbody:SetRotation(0, math.rad(180), 0)
    end

    if input:GetKey(AdHoc.Key.r) == true then
        force = force +  10 * DeltaTime()
    end
    if input:GetKeyUp(AdHoc.Key.r) == true then
        entities[nextId] = CreateEntity()

        transforms[nextId] = GetComponent(entities[nextId], "Transform")
        transforms[nextId].scale.x = 0.5
        transforms[nextId].scale.y = 0.5
        transforms[nextId].scale.z = 0.5  

        meshes[nextId] = GetComponent(entities[nextId], "Mesh")
        meshes[nextId]:Load("sphere.glb")

        AddComponent(entities[nextId], "RigidBody", "Sphere");
        rigidbodies[nextId] = GetComponent(entities[nextId], "RigidBody")

        forward = transform:GetForward()
        rigidbodies[nextId]:SetTranslation(transform.translate.x + 2 * forward.x, transform.translate.y, transform.translate.z + 2 * forward.z)
        rigidbodies[nextId]:AddVelocity(force * forward.x, force * 2, force * forward.z)

        rigidbodies[nextId].bounciness = 0.3
        rigidbodies[nextId]:SetGravity(0, -20, 0)
        
        nextId = nextId + 1

        force = 0
    end
end

function OnCollisionEnter(rhs)
    isGrounded = true
end

function OnCollisionExit(rhs)
    isGrounded = false
end
