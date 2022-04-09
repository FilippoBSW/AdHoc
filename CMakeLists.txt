cmake_minimum_required(VERSION 3.20)

project(AdHoc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    add_executable(AdHoc WIN32 "")
	set(DATA_PATH "Data")
	set(LIB_PATH "Lib")
	set(EXE_PATH "Exe")
elseif(APPLE)
    add_executable(AdHoc MACOSX_BUNDLE "")
	set(DATA_PATH "App/AdHoc.app/Data")
	set(LIB_PATH "App/AdHoc.app/Lib")
	set(EXE_PATH "App")
	set_target_properties(AdHoc PROPERTIES
        MACOSX_BUNDLE YES
        MACOSX_BUNDLE_IDENTIFIER io.AdHoc
        MACOSX_BUNDLE_GUI_IDENTIFIER io.AdHoc
        MACOSX_BUNDLE_BUNDLE_NAME "AdHoc"
		MACOSX_BUNDLE_EXECUTABLE AdHoc
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
		MACOSX_BUNDLE_DISPLAY_NAME "AdHoc"
    )
    set_target_properties(AdHoc PROPERTIES
        XCODE_ATTRIBUTE_PRODUCT_NAME "AdHoc"
        XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.github.Filippo-BSW.AdHoc"
    )
endif()

# PhysX
if(APPLE)
	set(TARGET_BUILD_PLATFORM "mac")
elseif(WIN32)
	set(TARGET_BUILD_PLATFORM "windows")
endif()
#TODO check for x86-arm
# set(OSX_BITNESS "-arch arm64")
# set(PHYSX_CXX_FLAGS "${OSX_BITNESS} -w")
set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
set(PHYSX_ROOT_DIR ${CMAKE_SOURCE_DIR}/External/PhysX/physx)
set(PXSHARED_PATH ${PHYSX_ROOT_DIR}/../pxshared)
set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CMAKEMODULES_VERSION "1.27")
set(CMAKEMODULES_PATH ${PHYSX_ROOT_DIR}/../externals/cmakemodules)
set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)
set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)

add_subdirectory(${PHYSX_ROOT_DIR}/compiler/public)

target_include_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/PhysX/physx/include)
target_include_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/PhysX/pxshared/include)

set(PHYSX_LIBRARIES
	PhysXExtensions
	PhysX
	PhysXPvdSDK
	PhysXVehicle
	PhysXCharacterKinematic
	PhysXCooking
	PhysXCommon
	PhysXFoundation)

###

set(BULLET_LIBRARIES
    BulletDynamics 
    BulletCollision  
    LinearMath 
    Bullet3Common)

add_subdirectory(${CMAKE_SOURCE_DIR}/External/bullet3)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/assimp)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/yaml-cpp)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/ImGui)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/ImGuizmo)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/lua-5.4.3)

target_include_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/Source)
target_include_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External)
target_include_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/bullet3/src)
target_include_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/yaml-cpp/include)
target_include_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/lua-5.4.3/src)

target_include_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/Source/Api)
target_include_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/Source/Platform)

set(ADH_CORE_SRC ${CMAKE_SOURCE_DIR}/Source/Core)
target_include_directories(AdHoc PRIVATE ${ADH_CORE_SRC})

target_link_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/bullet3)
target_link_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/assimp)
target_link_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/yaml-cpp)
target_link_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/lua-5.4.3)
target_link_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/ImGui)
target_link_directories(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/ImGuizmo)

target_link_libraries(AdHoc PRIVATE 
	Lua 
	ImGui 
    ImGuizmo
    assimp
    yaml-cpp
	${BULLET_LIBRARIES}
	${PHYSX_LIBRARIES})
     
if(APPLE)
    target_link_libraries(AdHoc PUBLIC 
        "-framework AppKit" 
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework GameController")
        target_compile_options(AdHoc PRIVATE -fobjc-arc -xobjective-c++)
endif()
if(APPLE OR UNIX)
	target_compile_options(AdHoc PUBLIC -Wno-switch)
	target_compile_options(AdHoc PUBLIC -Wno-format-security)
endif()

set_target_properties(
	AdHoc 
	ImGui 
    ImGuizmo
	assimp 
	yaml-cpp
	${BULLET_LIBRARIES}
	${PHYSX_LIBRARIES}
    PROPERTIES 
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${LIB_PATH}"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${LIB_PATH}"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${EXE_PATH}")

add_custom_target(copy_assets
		COMMAND ${CMAKE_COMMAND} -E copy_directory
				${CMAKE_SOURCE_DIR}/Source/Data
				${PROJECT_BINARY_DIR}/${DATA_PATH}
		COMMAND ${CMAKE_COMMAND} -E copy_directory
				${CMAKE_SOURCE_DIR}/Source/Editor/UIOverlay/Settings
				${PROJECT_BINARY_DIR}/${DATA_PATH}/Resources/Settings
        COMMAND ${CMAKE_COMMAND} -E copy_directory
				${CMAKE_SOURCE_DIR}/Source/Api/Vulkan/Shaders
				${PROJECT_BINARY_DIR}/${DATA_PATH}/Resources/Shaders
		COMMAND ${CMAKE_COMMAND} -E copy_directory
				${CMAKE_SOURCE_DIR}/Source/Core/Scripting/Scripts
				${PROJECT_BINARY_DIR}/${DATA_PATH}/Assets/Scripts
		COMMAND ${CMAKE_COMMAND} -E copy_directory
				${CMAKE_SOURCE_DIR}/Source/Core/Scripting/Scripts
				${PROJECT_BINARY_DIR}/${DATA_PATH}/Resources/Scripts)  
add_dependencies(AdHoc copy_assets) 

target_compile_definitions(AdHoc PUBLIC "$<$<CONFIG:Debug>:ADH_DEBUG>")
target_compile_definitions(AdHoc PUBLIC "$<$<CONFIG:Debug>:_DEBUG>")
target_compile_definitions(AdHoc PUBLIC "$<$<CONFIG:Release>:ADH_RELEASE>")

if(WIN32)
	target_compile_definitions(AdHoc PUBLIC NOMINMAX)
	target_compile_definitions(AdHoc PUBLIC _CRT_SECURE_NO_WARNINGS)
	target_compile_definitions(AdHoc PUBLIC UNICODE)
    target_compile_definitions(AdHoc PRIVATE "ADH_API=__declspec(dllexport)")
	target_compile_definitions(AdHoc INTERFACE "ADH_API=__declspec(dllimport)")
    target_compile_definitions(AdHoc PUBLIC "ADH_WINDOWS= ")
elseif(APPLE)
    target_compile_definitions(AdHoc PUBLIC "ADH_API= ")
    target_compile_definitions(AdHoc PUBLIC "ADH_APPLE= ")
elseif(UNIX)
    target_compile_definitions(AdHoc PUBLIC "ADH_API= )")
	target_compile_definitions(AdHoc PUBLIC "ADH_LINUX= ")
endif()

if(WIN32)
	set(PLATFORM_SRC_DIR ${CMAKE_SOURCE_DIR}/Source/Platform/Windows)
    target_sources(AdHoc PRIVATE ${PLATFORM_SRC_DIR}/Window.cpp)
elseif(APPLE)
	set(PLATFORM_SRC_DIR ${CMAKE_SOURCE_DIR}/Source/Platform/macOS)
    target_sources(AdHoc PRIVATE ${PLATFORM_SRC_DIR}/Window.mm)
	target_sources(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/Source/Utility.mm)
elseif(UNIX)

endif()

#**********************************************
# Install option:
#**********************************************
option(INSTALL "Install" OFF)
if(INSTALL)
	target_compile_definitions(AdHoc PUBLIC "ADH_INSTALL= ")
	target_compile_definitions(AdHoc PUBLIC "DATA_DIRECTORY=\"${PROJECT_BINARY_DIR}/${DATA_PATH}/\"")
else()
	if(WIN32)
		target_compile_definitions(AdHoc PUBLIC "DATA_DIRECTORY=\"../../Data/\"")
	elseif(APPLE)
		target_compile_definitions(AdHoc PUBLIC "DATA_DIRECTORY=\"/Data/\"")
	endif()
endif()

if(WIN32)
	target_compile_definitions(AdHoc PUBLIC "EXE_PATH=nullptr")
else()
	target_compile_definitions(AdHoc PUBLIC "EXE_PATH=argv[0]")
endif()

target_sources(AdHoc PRIVATE 
	${CMAKE_SOURCE_DIR}/Source/Main.cpp
    ${CMAKE_SOURCE_DIR}/Source/Window.hpp
    ${CMAKE_SOURCE_DIR}/Source/Utility.hpp
	${CMAKE_SOURCE_DIR}/Source/Vertex.hpp)

target_sources(AdHoc PRIVATE ${CMAKE_SOURCE_DIR}/External/sse2neon/sse2neon.h)

#**********************************************
# AdHoc graphics api
#**********************************************
find_package(Vulkan)
if(Vulkan_FOUND)
	target_include_directories(AdHoc PRIVATE ${Vulkan_INCLUDE_DIRS})
	target_link_libraries(AdHoc PRIVATE ${Vulkan_LIBRARIES})
    target_compile_definitions(AdHoc PRIVATE "ADH_VULKAN= ")
    if(WIN32)
            target_compile_definitions(AdHoc PRIVATE VK_USE_PLATFORM_WIN32_KHR)
            target_compile_definitions(AdHoc PRIVATE "ADH_VK_PLATFORM_SURFACE=VK_KHR_WIN32_SURFACE_EXTENSION_NAME")
    elseif(APPLE)
        	target_compile_definitions(AdHoc PRIVATE VK_USE_PLATFORM_MACOS_MVK)
            target_compile_definitions(AdHoc PRIVATE "ADH_VK_PLATFORM_SURFACE=VK_MVK_MACOS_SURFACE_EXTENSION_NAME")
    elseif(UNIX)
        	target_compile_definitions(AdHoc PRIVATE VK_USE_PLATFORM_DIRECTFB_EXT)
    endif()
 
	set(VULKAN_API_SRC_DIR ${CMAKE_SOURCE_DIR}/Source/Api/Vulkan)
    target_sources(AdHoc PRIVATE 
    ${VULKAN_API_SRC_DIR}/Initializers.hpp
    ${VULKAN_API_SRC_DIR}/Tools.hpp
    ${VULKAN_API_SRC_DIR}/Instance.hpp
    ${VULKAN_API_SRC_DIR}/Instance.cpp
    ${VULKAN_API_SRC_DIR}/PhysicalDevice.hpp
    ${VULKAN_API_SRC_DIR}/PhysicalDevice.cpp
    ${VULKAN_API_SRC_DIR}/Surface.hpp
    ${VULKAN_API_SRC_DIR}/Surface.cpp
    ${VULKAN_API_SRC_DIR}/Device.hpp
    ${VULKAN_API_SRC_DIR}/Device.cpp
    ${VULKAN_API_SRC_DIR}/DeviceQueues.hpp
    ${VULKAN_API_SRC_DIR}/DeviceQueues.cpp
    ${VULKAN_API_SRC_DIR}/Allocator.hpp
    ${VULKAN_API_SRC_DIR}/Allocator.cpp
    ${VULKAN_API_SRC_DIR}/Context.hpp
    ${VULKAN_API_SRC_DIR}/Context.cpp
    ${VULKAN_API_SRC_DIR}/Swapchain.hpp
    ${VULKAN_API_SRC_DIR}/Swapchain.cpp
    ${VULKAN_API_SRC_DIR}/Buffer.hpp
    ${VULKAN_API_SRC_DIR}/Buffer.cpp
    ${VULKAN_API_SRC_DIR}/Image.hpp
    ${VULKAN_API_SRC_DIR}/Image.cpp
    ${VULKAN_API_SRC_DIR}/Shader.hpp
    ${VULKAN_API_SRC_DIR}/Shader.cpp
    ${VULKAN_API_SRC_DIR}/Shader.inl
    ${VULKAN_API_SRC_DIR}/VertexLayout.hpp
    ${VULKAN_API_SRC_DIR}/VertexLayout.cpp
    ${VULKAN_API_SRC_DIR}/PipelineLayout.hpp
    ${VULKAN_API_SRC_DIR}/PipelineLayout.cpp
    ${VULKAN_API_SRC_DIR}/DescriptorSet.hpp
    ${VULKAN_API_SRC_DIR}/DescriptorSet.cpp
    ${VULKAN_API_SRC_DIR}/VertexBuffer.hpp
    ${VULKAN_API_SRC_DIR}/VertexBuffer.cpp
    ${VULKAN_API_SRC_DIR}/UniformBuffer.hpp
    ${VULKAN_API_SRC_DIR}/UniformBuffer.cpp
    ${VULKAN_API_SRC_DIR}/IndexBuffer.hpp
    ${VULKAN_API_SRC_DIR}/IndexBuffer.cpp
    ${VULKAN_API_SRC_DIR}/Attachments.hpp
    ${VULKAN_API_SRC_DIR}/Attachments.cpp
    ${VULKAN_API_SRC_DIR}/ComputePipeline.hpp
    ${VULKAN_API_SRC_DIR}/ComputePipeline.cpp
    ${VULKAN_API_SRC_DIR}/GraphicsPipeline.hpp
    ${VULKAN_API_SRC_DIR}/GraphicsPipeline.cpp
    ${VULKAN_API_SRC_DIR}/Subpass.hpp
    ${VULKAN_API_SRC_DIR}/Subpass.cpp
    ${VULKAN_API_SRC_DIR}/RenderPass.hpp
    ${VULKAN_API_SRC_DIR}/RenderPass.cpp
    ${VULKAN_API_SRC_DIR}/Sampler.hpp
    ${VULKAN_API_SRC_DIR}/Sampler.cpp
    ${VULKAN_API_SRC_DIR}/Texture2D.hpp
    ${VULKAN_API_SRC_DIR}/Texture2D.cpp
    ${VULKAN_API_SRC_DIR}/CommandPool.hpp
    ${VULKAN_API_SRC_DIR}/CommandPool.cpp
    ${VULKAN_API_SRC_DIR}/CommandBuffer.hpp
    ${VULKAN_API_SRC_DIR}/CommandBuffer.cpp
    ${VULKAN_API_SRC_DIR}/Viewport.hpp
    ${VULKAN_API_SRC_DIR}/Viewport.cpp
    ${VULKAN_API_SRC_DIR}/Scissor.hpp
    ${VULKAN_API_SRC_DIR}/Scissor.cpp
    ${VULKAN_API_SRC_DIR}/Memory.hpp)
endif()

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Found no supported API!")
endif()

#**********************************************
# AdHoc template library
#**********************************************
target_sources(AdHoc PRIVATE 
    ${ADH_CORE_SRC}/Entity/Entity.hpp
    ${ADH_CORE_SRC}/Event/Event.hpp
    ${ADH_CORE_SRC}/Event/Event.cpp
    ${ADH_CORE_SRC}/Event/EventTypes.hpp
    ${ADH_CORE_SRC}/Physics/Physics.hpp
    ${ADH_CORE_SRC}/Physics/Physics.cpp
    ${ADH_CORE_SRC}/Std/Algorithms.hpp
    ${ADH_CORE_SRC}/Std/Random.hpp
    ${ADH_CORE_SRC}/Std/Array.hpp
    ${ADH_CORE_SRC}/Std/Benchmark.hpp
    ${ADH_CORE_SRC}/Std/Concepts.hpp
    ${ADH_CORE_SRC}/Std/File.hpp
    ${ADH_CORE_SRC}/Std/Function.hpp
    ${ADH_CORE_SRC}/Std/Iterator.hpp
    ${ADH_CORE_SRC}/Std/List.hpp
    ${ADH_CORE_SRC}/Std/Queue.hpp
    ${ADH_CORE_SRC}/Std/SharedPtr.hpp
    ${ADH_CORE_SRC}/Std/SparseSet.hpp
    ${ADH_CORE_SRC}/Std/Stack.hpp
    ${ADH_CORE_SRC}/Std/StaticArray.hpp
    ${ADH_CORE_SRC}/Std/Stopwatch.hpp
    ${ADH_CORE_SRC}/Std/TGALoader.hpp
    ${ADH_CORE_SRC}/Std/UniquePtr.hpp
    ${ADH_CORE_SRC}/Std/Utility.hpp)

#**********************************************
# AdHoc math library
#**********************************************
target_sources(AdHoc PRIVATE 
    ${ADH_CORE_SRC}/Math/Math.hpp
    ${ADH_CORE_SRC}/Math/Source/Matrix.hpp
    ${ADH_CORE_SRC}/Math/Source/Matrix.inl
    ${ADH_CORE_SRC}/Math/Source/Matrix2D.hpp
    ${ADH_CORE_SRC}/Math/Source/Matrix2D.inl
    ${ADH_CORE_SRC}/Math/Source/Matrix3D.hpp
    ${ADH_CORE_SRC}/Math/Source/Matrix3D.inl
    ${ADH_CORE_SRC}/Math/Source/Matrix4D.hpp
    ${ADH_CORE_SRC}/Math/Source/Matrix4D.inl
    ${ADH_CORE_SRC}/Math/Source/Numbers.hpp
    ${ADH_CORE_SRC}/Math/Source/Quaternion.hpp
    ${ADH_CORE_SRC}/Math/Source/Quaternion.inl
    ${ADH_CORE_SRC}/Math/Source/Vector.hpp
    ${ADH_CORE_SRC}/Math/Source/Vector.inl
    ${ADH_CORE_SRC}/Math/Source/Vector2D.hpp
    ${ADH_CORE_SRC}/Math/Source/Vector2D.inl
    ${ADH_CORE_SRC}/Math/Source/Vector3D.hpp
    ${ADH_CORE_SRC}/Math/Source/Vector3D.inl
    ${ADH_CORE_SRC}/Math/Source/Vector4D.hpp
    ${ADH_CORE_SRC}/Math/Source/Vector4D.inl
    ${ADH_CORE_SRC}/Math/Source/XmmMatrix.hpp
    ${ADH_CORE_SRC}/Math/Source/XmmMatrix.inl
    ${ADH_CORE_SRC}/Math/Source/XmmVector.hpp
    ${ADH_CORE_SRC}/Math/Source/XmmVector.inl)

#**********************************************
# AdHoc scene
#**********************************************
target_sources(AdHoc PRIVATE 
    ${ADH_CORE_SRC}/Scene/Scene.hpp
    ${ADH_CORE_SRC}/Scene/Scene.cpp
	${ADH_CORE_SRC}/Scene/Serializer.hpp
	${ADH_CORE_SRC}/Scene/Serializer.cpp
	${ADH_CORE_SRC}/Scene/ComponentsSerializer.hpp
    ${ADH_CORE_SRC}/Scene/Components.hpp
    ${ADH_CORE_SRC}/Scene/Components/Camera.hpp
	${ADH_CORE_SRC}/Scene/Components/RigidBody.hpp
	${ADH_CORE_SRC}/Scene/Components/RigidBody.cpp
    ${ADH_CORE_SRC}/Scene/Components/Light.hpp
    ${ADH_CORE_SRC}/Scene/Components/Material.hpp
    ${ADH_CORE_SRC}/Scene/Components/Mesh.hpp
    ${ADH_CORE_SRC}/Scene/Components/Mesh.cpp
    ${ADH_CORE_SRC}/Scene/Components/Tag.hpp
    ${ADH_CORE_SRC}/Scene/Components/Transform.hpp)

#**********************************************
# AdHoc Scripting
#**********************************************
target_sources(AdHoc PRIVATE 
	${ADH_CORE_SRC}/Scripting/Script.hpp
	${ADH_CORE_SRC}/Scripting/ScriptHandler.hpp
	${ADH_CORE_SRC}/Scripting/ScriptHandler.cpp)
 
#**********************************************
# AdHoc input
#**********************************************
target_sources(AdHoc PRIVATE 
    ${ADH_CORE_SRC}/Input/Input.hpp
    ${ADH_CORE_SRC}/Input/Input.cpp
    ${ADH_CORE_SRC}/Input/Controller.hpp
    ${ADH_CORE_SRC}/Input/Controller.cpp
    ${ADH_CORE_SRC}/Input/Mouse.hpp
    ${ADH_CORE_SRC}/Input/Mouse.cpp
    ${ADH_CORE_SRC}/Input/Keyboard.hpp
    ${ADH_CORE_SRC}/Input/Keyboard.cpp
    ${ADH_CORE_SRC}/Input/Keycodes.hpp
    ${ADH_CORE_SRC}/Input/AppleKeycodes.hpp)

#**********************************************
# AdHoc Editor
#**********************************************
set(ADH_EDITOR_SRC ${CMAKE_SOURCE_DIR}/Source/Editor)
target_sources(AdHoc PRIVATE 
	${ADH_EDITOR_SRC}/Editor.hpp
	${ADH_EDITOR_SRC}/Editor.cpp
	${ADH_EDITOR_SRC}/Api/Vulkan/VulkanImGui.hpp
	${ADH_EDITOR_SRC}/Api/Vulkan/VulkanImGui.cpp
	${ADH_EDITOR_SRC}/UIOverlay/UIOverlay.hpp
	${ADH_EDITOR_SRC}/UIOverlay/UIOverlay.cpp
	${ADH_EDITOR_SRC}/UIOverlay/IconFontCppHeaders/IconFontAwesome5.hpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/AssetPanel.hpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/AssetPanel.cpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/ConsolePanel.hpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/ConsolePanel.cpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/GamePanel.hpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/GamePanel.cpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/InspectorPanel.hpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/InspectorPanel.cpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/SceneHierarchyPanel.hpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/SceneHierarchyPanel.cpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/ScenePanel.hpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/ScenePanel.cpp
    ${ADH_EDITOR_SRC}/UIOverlay/Panels/ControlsPanel.hpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/ControlsPanel.cpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/ViewportRect.hpp
	${ADH_EDITOR_SRC}/UIOverlay/Panels/ViewportUtility.hpp)

if(NOT WIN32)
    target_compile_features(AdHoc PRIVATE c_std_17)
endif()
target_compile_features(AdHoc PRIVATE cxx_std_20)

#TODO: GNU/Linux support
#TODO: IOS support 
# option(BUILD_FOR_IOS "Build for IOS" OFF)
# if(BUILD_FOR_IOS)
#     target_compile_definitions(AdHoc PRIVATE "ADH_IOS= ")
#     set(CMAKE_OSX_SYSROOT "iphoneos")
#     set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")
#     target_link_libraries(
#         AdHoc
#         PUBLIC
#         "-framework UIKit"
#     )

#     set_target_properties(AdHoc PROPERTIES
#         MACOSX_BUNDLE YES
#         MACOSX_BUNDLE_GUI_IDENTIFIER com.AdHoc
#         MACOSX_BUNDLE_BUNDLE_NAME "AdHoc"
#         MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
#         MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
#         XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
#         XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
#         XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
#         XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO"
#         )

#     set_target_properties(
#         AdHoc
#         PROPERTIES
#         XCODE_ATTRIBUTE_PRODUCT_NAME
#         "AdHoc"
#         XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER
#         "com.github.Filippo-BSW.AdHoc"
#     )

#     set_target_properties(
#         AdHoc
#         PROPERTIES
#         XCODE_ATTRIBUTE_PRODUCT_NAME[variant=Debug]
#         "AdHoc-Dbg"
#         XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER[variant=Debug]
#          "com.github.Filippo-BSW.AdHoc"
#     )
# endif()

# VK_USE_PLATFORM_IOS_MVK
# VK_MVK_IOS_SURFACE_EXTENSION_NAME
# VkIOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
# surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
# surfaceCreateInfo.pNext = NULL;
# surfaceCreateInfo.flags = 0;
# surfaceCreateInfo.pView = view;
# err = vkCreateIOSSurfaceMVK(instance, &surfaceCreateInfo, nullptr, &surface);
