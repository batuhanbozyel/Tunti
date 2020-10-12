#pragma once

// Core
#include "Doge/Core/Platform.h"
#include "Doge/Core/Application.h"
#include "Doge/Core/Timestep.h"
#include "Doge/Core/Input.h"
#include "Doge/Core/Log.h"

// Renderer
#include "Doge/Renderer/Renderer.h"
#include "Doge/Renderer/RendererCommands.h"
#include "Doge/Renderer/Framebuffer.h"
#include "Doge/Renderer/Texture.h"
#include "Doge/Renderer/Shader.h"

// Scene
#include "Doge/Scene/Scene.h"
#include "Doge/Scene/Entity.h"
#include "Doge/Scene/Components.h"
#include "Doge/Scene/Model.h"
#include "Doge/Scene/Light.h"
#include "Doge/Scene/SceneObject3D.h"
#include "Doge/Scene/LightingShader.h"
#include "Doge/Scene/CameraController.h"

// Utility
#include "Doge/Utility/Mesh.h"
#include "Doge/Utility/Material.h"

// 3rd-Party Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>