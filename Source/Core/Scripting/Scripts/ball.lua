require "AdHoc"

local this = GetThis()
AdHoc.Global.ball = this
local rigidbody

local input = GetInput()
local force = 0

local playerTransform

function Start()
    playerTransform = GetComponent(AdHoc.Global.playerID, "Transform")

    rigidbody = GetComponent(this, "RigidBody")
    rigidbody:SetTranslation(playerTransform.translate.x + 2, playerTransform.translate.y, playerTransform.translate.z)
    rigidbody:SetGravity(0, -20, 0)
end

function Update()
    if input:GetKey(AdHoc.Key.space) == true then
        force = force + 0.03
    end
    if input:GetKeyUp(AdHoc.Key.space) == true then
        rigidbody:AddVelocity(force, force * 2, 0.0)
    end

    if input:GetKeyDown(AdHoc.Key.p) == true then
        force = 0
        rigidbody:SetTranslation(playerTransform.translate.x + 2, playerTransform.translate.y, playerTransform.translate.z)
        rigidbody:ClearForces()
        rigidbody:ClearGravity()
        rigidbody:SetAngularVelocity(0,0,0)
        rigidbody:SetVelocity(0,0,0)
    end
    
    local a = rigidbody:GetTranslation()
    LogMessage(a.x)

end
