#pragma once

using Real = float;
// USE ONLY IN EXTERNAL APPLICATIONS
#include "Application.h"
#include "Layers/Layer.h"
#include "Logging/Log.h"
#include "ImGui/ImGuiLayer.h"
#include "IMath.h"

#include "Core/Timestep.h"

#include "Input.h"
#include "MouseButtonCodes.h"
#include "KeyCodes.h"

#include "Project/Project.h"

#include "Rendering/Buffer.h"
#include "Rendering/GraphicsContext.h"
#include "Rendering/OrthographicCamera.h"
#include "Rendering/RenderCommand.h"
#include "Rendering/Renderer.h"
#include "Rendering/Renderer2D.h"
#include "Rendering/RendererAPI.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"
#include "Rendering/FrameBuffer.h"
#include "Rendering/SubTexture.h"
#include "OrthoCameraController.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Rendering/EditorCamera.h"