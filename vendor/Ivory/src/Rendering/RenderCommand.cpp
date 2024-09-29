#include "pch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Ivory {
	std::shared_ptr<RendererAPI> RenderCommand::s_rendererAPI = std::make_shared<OpenGLRendererAPI>();
}