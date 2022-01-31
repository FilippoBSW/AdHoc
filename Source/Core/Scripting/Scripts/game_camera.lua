require "AdHoc"

local this = GetThis()
local camera = GetComponent(this, "Camera3D")
local playerTransform

function Start()
    playerTransform = GetComponent(AdHoc.Global.playerID, "Transform")
end

function Update()
    camera.focusPosition = playerTransform.translate
end
