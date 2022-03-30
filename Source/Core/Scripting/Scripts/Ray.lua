require "AdHoc"

local this = GetThis()
local input = GetInput()
local transfrom

function Start()
    transfrom = GetComponent(this, "Transform")
end

function Update()
    transfrom.rotation.y = transfrom.rotation.y + 0.005

    --if input:GetKeyDown(AdHoc.Key.a) == true then
    local e = Raycast(transfrom.translate, transfrom:GetForward(), 40)
    if e ~= 0 then
    local m = GetComponent(e, "Material")
    m.albedo.x = math.random()
    m.albedo.y = math.random()
    m.albedo.z = math.random()
    end
--end
end
