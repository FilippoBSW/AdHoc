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

function Start()
    rigidbody:SetLinearFactor(1,1,1)
    rigidbody:SetAngularFactor(0,1,0)
end

function Update()
    if input:GetKey(AdHoc.Key.d) == true then
        rigidbody:AddVelocity(10 * DeltaTime(), 0, 0)
    elseif input:GetKey(AdHoc.Key.a) == true then
        rigidbody:AddVelocity(-10 * DeltaTime(), 0, 0)
    elseif input:GetKey(AdHoc.Key.w) == true then
        rigidbody:AddVelocity(0, 0, 10 * DeltaTime())
    elseif input:GetKey(AdHoc.Key.s) == true then
        rigidbody:AddVelocity(0, 0, -10 * DeltaTime())
    end

    -- Rigidbody position etc Get-Set
    if input:GetKeyDown(AdHoc.Key.r) == true then
        entities[nextId] = CreateEntity()
        AddComponent(entities[nextId], "RigidBody", "Sphere");

        transforms[nextId] = GetComponent(entities[nextId], "Transform")
        transforms[nextId].translate = transform.translate
        LogMessage(transforms[nextId].translate.x)
        LogMessage(transforms[nextId].translate.y)
        LogMessage(transforms[nextId].translate.z)

        meshes[nextId] = GetComponent(entities[nextId], "Mesh")
        meshes[nextId]:Load("/Users/filippo-bsw/Repos/AdHoc/build/App/AdHoc.app/Data/Assets/Models/sphere.glb")

        rigidbodies[nextId] = GetComponent(entities[nextId], "RigidBody")
        rigidbodies[nextId]:AddVelocity(5.0, 10.0, 0.0)
        rigidbodies[nextId].bounciness = 0.3

        nextId = nextId + 1
    end
end

function OnCollisionEnter(rhs)
    isGrounded = true
end

function OnCollisionExit(rhs)
    isGrounded = false
end
