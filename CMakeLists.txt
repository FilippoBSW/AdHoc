cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME AdHoc)
project(${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(USE_PLATFORM STREQUAL "Windows")
    add_executable(${PROJECT_NAME} WIN32 "")
	set(DATA_PATH "Data")
	set(LIB_PATH "Lib")
	set(EXE_PATH "Exe")
elseif(USE_PLATFORM STREQUAL "Mac")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE "")
	set(DATA_PATH "App/${PROJECT_NAME}.app/Data")
	set(LIB_PATH "App/${PROJECT_NAME}.app/Lib")
	set(EXE_PATH "App")
	set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE YES
        MACOSX_BUNDLE_IDENTIFIER io.${PROJECT_NAME}
        MACOSX_BUNDLE_GUI_IDENTIFIER io.${PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
		MACOSX_BUNDLE_EXECUTABLE ${PROJECT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
		MACOSX_BUNDLE_DISPLAY_NAME "${PROJECT_NAME}"
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_ATTRIBUTE_PRODUCT_NAME "${PROJECT_NAME}"
        XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.github.Filippo-BSW.${PROJECT_NAME}"
    )
    target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)
elseif(USE_PLATFORM STREQUAL "Linux")
	add_executable(${PROJECT_NAME} "")
	set(DATA_PATH "Data")
	set(LIB_PATH "Lib")
	set(EXE_PATH "Exe")
    target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)
	find_package(Threads REQUIRED)
    set(CMAKE_CXX_FLAGS "-pthread -ftrivial-auto-var-init=zero -g")
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE <stdint.h>)


if(USE_PLATFORM STREQUAL "Mac")
	set(TARGET_BUILD_PLATFORM "mac")
	add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD COMMAND
	    ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@executable_path/../../Lib/bin/mac.x86_64/$<CONFIGURATION>"
	    $<TARGET_FILE:${PROJECT_NAME}>)
elseif(USE_PLATFORM STREQUAL "Windows")
	set(TARGET_BUILD_PLATFORM "windows")
	add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy_directory
	    "${CMAKE_BINARY_DIR}/Lib/bin/win.x86_64.vc142.md/$<CONFIGURATION>"
	    "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
elseif(USE_PLATFORM STREQUAL "Linux")
	set(TARGET_BUILD_PLATFORM "linux")
    if($<CONFIGURATION> STREQUAL "Debug")
        set(PHY_CONFIG "debug")
    elseif($<CONFIGURATION> STREQUAL "Release")
        set(PHY_CONFIG "release")
    endif()
	add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_BINARY_DIR}/Lib/bin/linux.clang/${PHY_CONFIG}"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif()

set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
set(PHYSX_ROOT_DIR ${CMAKE_SOURCE_DIR}/External/PhysX/physx)
set(PXSHARED_PATH ${PHYSX_ROOT_DIR}/../pxshared)
set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(PX_PHYSX_ ${CMAKE_INSTALL_PREFIX})
set(CMAKEMODULES_VERSION "1.27")
set(CMAKEMODULES_PATH ${PHYSX_ROOT_DIR}/../externals/cmakemodules)
set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/External/PhysX)
set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/${LIB_PATH})

add_subdirectory(${PHYSX_ROOT_DIR}/compiler/public)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/Assimp)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/yaml-cpp)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/ImGui)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/ImGuizmo)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/lua-5.4.3)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/openal-soft)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Source)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Source/Api)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Source/Platform)

set(ADH_CORE_SRC ${CMAKE_SOURCE_DIR}/Source/Core)
target_include_directories(${PROJECT_NAME} PRIVATE ${ADH_CORE_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/External)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/External/openal-soft/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/External/PhysX/physx/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/External/PhysX/pxshared/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/External/yaml-cpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/External/lua-5.4.3/src)

set(PHYSX_LIBRARIES
	PhysXExtensions
	PhysX
	PhysXPvdSDK
	PhysXVehicle
	PhysXCharacterKinematic
	PhysXCooking
	PhysXCommon
	PhysXFoundation)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Lua
	ImGui
    ImGuizmo
    assimp
    yaml-cpp
    OpenAL
	${PHYSX_LIBRARIES})

if(USE_PLATFORM STREQUAL "Linux")
	target_link_libraries(${PROJECT_NAME} PRIVATE X11 xcb X11-xcb)
endif()

if(USE_PLATFORM STREQUAL "Mac")
    target_link_libraries(${PROJECT_NAME} PUBLIC
        "-framework AppKit"
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework GameController")
        target_compile_options(${PROJECT_NAME} PRIVATE -fobjc-arc -xobjective-c++)
endif()
if(APPLE OR UNIX)
	target_compile_options(${PROJECT_NAME} PUBLIC -Wno-switch)
	target_compile_options(${PROJECT_NAME} PUBLIC -Wno-format-security)
endif()

set_target_properties(
	${PROJECT_NAME}
	Lua
	ImGui
    ImGuizmo
	assimp
	yaml-cpp
    OpenAL
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
add_dependencies(${PROJECT_NAME} copy_assets)

target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:Debug>:ADH_DEBUG>")
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:Release>:ADH_RELEASE>")
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:Debug>:_DEBUG>")

if(USE_PLATFORM STREQUAL "Windows")
	target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
	target_compile_definitions(${PROJECT_NAME} PUBLIC UNICODE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "ADH_API=__declspec(dllexport)")
	target_compile_definitions(${PROJECT_NAME} INTERFACE "ADH_API=__declspec(dllimport)")
    target_compile_definitions(${PROJECT_NAME} PUBLIC "ADH_WINDOWS= ")
elseif(USE_PLATFORM STREQUAL "Mac")
    target_compile_definitions(${PROJECT_NAME} PUBLIC "ADH_API= ")
    target_compile_definitions(${PROJECT_NAME} PUBLIC "ADH_APPLE= ")
elseif(USE_PLATFORM STREQUAL "Linux")
    target_compile_definitions(${PROJECT_NAME} PUBLIC "ADH_API= ")
	target_compile_definitions(${PROJECT_NAME} PUBLIC "ADH_LINUX= ")
endif()

if(USE_PLATFORM STREQUAL "Windows")
	set(PLATFORM_SRC_DIR ${CMAKE_SOURCE_DIR}/Source/Platform/Windows)
    target_sources(${PROJECT_NAME} PRIVATE ${PLATFORM_SRC_DIR}/Window.cpp)
elseif(USE_PLATFORM STREQUAL "Mac")
	set(PLATFORM_SRC_DIR ${CMAKE_SOURCE_DIR}/Source/Platform/macOS)
    target_sources(${PROJECT_NAME} PRIVATE ${PLATFORM_SRC_DIR}/Window.mm)
	target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Source/Utility.mm)
elseif(USE_PLATFORM STREQUAL "Linux")
	set(PLATFORM_SRC_DIR ${CMAKE_SOURCE_DIR}/Source/Platform/Linux)
    target_sources(${PROJECT_NAME} PRIVATE ${PLATFORM_SRC_DIR}/Window.cpp)
endif()

#**********************************************
#Install option:
#**********************************************
option(INSTALL "Install" OFF)
if(INSTALL)
	target_compile_definitions(${PROJECT_NAME} PUBLIC "ADH_INSTALL= ")
	target_compile_definitions(${PROJECT_NAME} PUBLIC "DATA_DIRECTORY=\"${PROJECT_BINARY_DIR}/${DATA_PATH}/\"")
else()
	if(USE_PLATFORM STREQUAL "Windows")
		target_compile_definitions(${PROJECT_NAME} PUBLIC "DATA_DIRECTORY=\"../../Data/\"")
	elseif(USE_PLATFORM STREQUAL "Mac")
		target_compile_definitions(${PROJECT_NAME} PUBLIC "DATA_DIRECTORY=\"/Data/\"")
    elseif(USE_PLATFORM STREQUAL "Linux")
		target_compile_definitions(${PROJECT_NAME} PUBLIC "DATA_DIRECTORY=\"Data/\"")
	endif()
endif()

if(USE_PLATFORM STREQUAL "Windows")
	target_compile_definitions(${PROJECT_NAME} PUBLIC "EXE_PATH=nullptr")
else()
	target_compile_definitions(${PROJECT_NAME} PUBLIC "EXE_PATH=argv[0]")
endif()

#**********************************************
#Add source
#**********************************************
target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_SOURCE_DIR}/Source/Main.cpp
    ${CMAKE_SOURCE_DIR}/Source/Window.hpp
    ${CMAKE_SOURCE_DIR}/Source/Utility.hpp
	${CMAKE_SOURCE_DIR}/Source/Vertex.hpp)

target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/External/sse2neon/sse2neon.h)

#**********************************************
#Graphics api
#**********************************************
find_package(Vulkan)
if(Vulkan_FOUND)
	target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE "ADH_VULKAN= ")
    if(USE_PLATFORM STREQUAL "Windows")
        target_compile_definitions(${PROJECT_NAME} PRIVATE VK_USE_PLATFORM_WIN32_KHR)
        target_compile_definitions(${PROJECT_NAME} PRIVATE "ADH_VK_PLATFORM_SURFACE=VK_KHR_WIN32_SURFACE_EXTENSION_NAME")
    elseif(USE_PLATFORM STREQUAL "Mac")
        target_compile_definitions(${PROJECT_NAME} PRIVATE VK_USE_PLATFORM_MACOS_MVK)
        target_compile_definitions(${PROJECT_NAME} PRIVATE "ADH_VK_PLATFORM_SURFACE=VK_MVK_MACOS_SURFACE_EXTENSION_NAME")
	elseif(USE_PLATFORM STREQUAL "Linux")
        target_compile_definitions(${PROJECT_NAME} PRIVATE VK_USE_PLATFORM_XCB_KHR)
        target_compile_definitions(${PROJECT_NAME} PRIVATE "ADH_VK_PLATFORM_SURFACE=VK_KHR_XCB_SURFACE_EXTENSION_NAME")
    endif()

	set(VULKAN_API_SRC ${CMAKE_SOURCE_DIR}/Source/Api/Vulkan)
    target_sources(${PROJECT_NAME} PRIVATE
    ${VULKAN_API_SRC}/Initializers.hpp
    ${VULKAN_API_SRC}/Tools.hpp
    ${VULKAN_API_SRC}/Instance.hpp
    ${VULKAN_API_SRC}/Instance.cpp
    ${VULKAN_API_SRC}/PhysicalDevice.hpp
    ${VULKAN_API_SRC}/PhysicalDevice.cpp
    ${VULKAN_API_SRC}/Surface.hpp
    ${VULKAN_API_SRC}/Surface.cpp
    ${VULKAN_API_SRC}/Device.hpp
    ${VULKAN_API_SRC}/Device.cpp
    ${VULKAN_API_SRC}/DeviceQueues.hpp
    ${VULKAN_API_SRC}/DeviceQueues.cpp
    ${VULKAN_API_SRC}/Allocator.hpp
    ${VULKAN_API_SRC}/Allocator.cpp
    ${VULKAN_API_SRC}/Context.hpp
    ${VULKAN_API_SRC}/Context.cpp
    ${VULKAN_API_SRC}/Swapchain.hpp
    ${VULKAN_API_SRC}/Swapchain.cpp
    ${VULKAN_API_SRC}/Buffer.hpp
    ${VULKAN_API_SRC}/Buffer.cpp
    ${VULKAN_API_SRC}/Image.hpp
    ${VULKAN_API_SRC}/Image.cpp
	${VULKAN_API_SRC}/ImageView.hpp
	${VULKAN_API_SRC}/ImageView.cpp
	${VULKAN_API_SRC}/Framebuffer.hpp
	${VULKAN_API_SRC}/Framebuffer.cpp
    ${VULKAN_API_SRC}/Shader.hpp
    ${VULKAN_API_SRC}/Shader.cpp
    ${VULKAN_API_SRC}/Shader.inl
    ${VULKAN_API_SRC}/VertexLayout.hpp
    ${VULKAN_API_SRC}/VertexLayout.cpp
    ${VULKAN_API_SRC}/PipelineLayout.hpp
    ${VULKAN_API_SRC}/PipelineLayout.cpp
    ${VULKAN_API_SRC}/DescriptorSet.hpp
    ${VULKAN_API_SRC}/DescriptorSet.cpp
    ${VULKAN_API_SRC}/VertexBuffer.hpp
    ${VULKAN_API_SRC}/VertexBuffer.cpp
    ${VULKAN_API_SRC}/UniformBuffer.hpp
    ${VULKAN_API_SRC}/UniformBuffer.cpp
    ${VULKAN_API_SRC}/IndexBuffer.hpp
    ${VULKAN_API_SRC}/IndexBuffer.cpp
    ${VULKAN_API_SRC}/Attachments.hpp
    ${VULKAN_API_SRC}/Attachments.cpp
    ${VULKAN_API_SRC}/ComputePipeline.hpp
    ${VULKAN_API_SRC}/ComputePipeline.cpp
    ${VULKAN_API_SRC}/GraphicsPipeline.hpp
    ${VULKAN_API_SRC}/GraphicsPipeline.cpp
    ${VULKAN_API_SRC}/Subpass.hpp
    ${VULKAN_API_SRC}/Subpass.cpp
    ${VULKAN_API_SRC}/RenderPass.hpp
    ${VULKAN_API_SRC}/RenderPass.cpp
    ${VULKAN_API_SRC}/Sampler.hpp
    ${VULKAN_API_SRC}/Sampler.cpp
    ${VULKAN_API_SRC}/Texture2D.hpp
    ${VULKAN_API_SRC}/Texture2D.cpp
    ${VULKAN_API_SRC}/CommandPool.hpp
    ${VULKAN_API_SRC}/CommandPool.cpp
    ${VULKAN_API_SRC}/CommandBuffer.hpp
    ${VULKAN_API_SRC}/CommandBuffer.cpp
    ${VULKAN_API_SRC}/Viewport.hpp
    ${VULKAN_API_SRC}/Viewport.cpp
    ${VULKAN_API_SRC}/Scissor.hpp
    ${VULKAN_API_SRC}/Scissor.cpp
    ${VULKAN_API_SRC}/Memory.hpp)
endif()

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Found no supported API!")
endif()

#**********************************************
#Template library
#**********************************************
target_sources(${PROJECT_NAME} PRIVATE
    ${ADH_CORE_SRC}/Entity/Entity.hpp
    ${ADH_CORE_SRC}/Event/Event.hpp
    ${ADH_CORE_SRC}/Event/Event.cpp
    ${ADH_CORE_SRC}/Event/EventTypes.hpp
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
#Physics library
#**********************************************
target_sources(${PROJECT_NAME} PRIVATE
    ${ADH_CORE_SRC}/Physics/PhysicsWorld.hpp
    ${ADH_CORE_SRC}/Physics/PhysicsWorld.cpp)

#**********************************************
#Math library
#**********************************************
target_sources(${PROJECT_NAME} PRIVATE
    ${ADH_CORE_SRC}/Math/Math.hpp
    ${ADH_CORE_SRC}/Math/source/Matrix.hpp
    ${ADH_CORE_SRC}/Math/source/Matrix.inl
    ${ADH_CORE_SRC}/Math/source/Matrix2D.hpp
    ${ADH_CORE_SRC}/Math/source/Matrix2D.inl
    ${ADH_CORE_SRC}/Math/source/Matrix3D.hpp
    ${ADH_CORE_SRC}/Math/source/Matrix3D.inl
    ${ADH_CORE_SRC}/Math/source/Matrix4D.hpp
    ${ADH_CORE_SRC}/Math/source/Matrix4D.inl
    ${ADH_CORE_SRC}/Math/source/Numbers.hpp
    ${ADH_CORE_SRC}/Math/source/Quaternion.hpp
    ${ADH_CORE_SRC}/Math/source/Quaternion.inl
    ${ADH_CORE_SRC}/Math/source/Vector.hpp
    ${ADH_CORE_SRC}/Math/source/Vector.inl
    ${ADH_CORE_SRC}/Math/source/Vector2D.hpp
    ${ADH_CORE_SRC}/Math/source/Vector2D.inl
    ${ADH_CORE_SRC}/Math/source/Vector3D.hpp
    ${ADH_CORE_SRC}/Math/source/Vector3D.inl
    ${ADH_CORE_SRC}/Math/source/Vector4D.hpp
    ${ADH_CORE_SRC}/Math/source/Vector4D.inl
    ${ADH_CORE_SRC}/Math/source/XmmMatrix.hpp
    ${ADH_CORE_SRC}/Math/source/XmmMatrix.inl
    ${ADH_CORE_SRC}/Math/source/XmmVector.hpp
    ${ADH_CORE_SRC}/Math/source/XmmVector.inl)

#**********************************************
#Scene
#**********************************************
target_sources(${PROJECT_NAME} PRIVATE
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
#Scripting
#**********************************************
target_sources(${PROJECT_NAME} PRIVATE
	${ADH_CORE_SRC}/Scripting/Script.hpp
	${ADH_CORE_SRC}/Scripting/ScriptHandler.hpp
	${ADH_CORE_SRC}/Scripting/ScriptHandler.cpp)

#**********************************************
#Audio
#**********************************************
target_sources(${PROJECT_NAME} PRIVATE
	${ADH_CORE_SRC}/Audio/LoadWav.hpp
	${ADH_CORE_SRC}/Audio/Audio.hpp
	${ADH_CORE_SRC}/Audio/Audio.cpp)

#**********************************************
#Input
#**********************************************
target_sources(${PROJECT_NAME} PRIVATE
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
#Editor
#**********************************************
set(ADH_EDITOR_SRC ${CMAKE_SOURCE_DIR}/Source/Editor)
target_sources(${PROJECT_NAME} PRIVATE
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
