require "AdHoc"

local this = GetThis()
AdHoc.Global.playerID = this
local rigidbody = GetComponent(this, "RigidBody")

local input = GetInput()

local isGrounded = false

function Start()
    rigidbody:SetLinearFactor(1,1,0)
    rigidbody:SetAngularFactor(0,0,0)
end

function Update()
    if input:GetKey(AdHoc.Key.d) == true then
        rigidbody:AddVelocity(10 * DeltaTime(), 0, 0)
    elseif input:GetKey(AdHoc.Key.a) == true then
        rigidbody:AddVelocity(-10 * DeltaTime(), 0, 0)
    end

    if input:GetKeyDown(AdHoc.Key.space) == true then
        if isGrounded == true then
            rigidbody:AddVelocity(0, 12, 0)
        end
    end
end

function OnCollisionEnter(rhs)
    isGrounded = true
end

function OnCollisionExit(rhs)
    isGrounded = false
end
