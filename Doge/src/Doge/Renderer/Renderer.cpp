#include "pch.h"
#include "Renderer.h"
#include "RendererCommands.h"

#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Framebuffer.h"
#include "ShaderDataType.h"

#include "Doge/Core/Window.h"

#include "Doge/Utility/Camera.h"
#include "Doge/Utility/Material.h"
#include "Doge/Utility/SceneObject3D.h"

static const char* TexturedQuadVertexShader = R"(
#version 450 core

layout(location = 0) in vec2  a_Position;
layout(location = 1) in vec2  a_TexCoord;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position, 0.0, 1.0);
})";

static const char* TexturedQuadFragmentShader = R"(
#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
	color = texture(u_Texture, v_TexCoord);
})";

namespace Doge
{
	RendererAPI Renderer::s_API = RendererAPI::None;

	std::unique_ptr<Framebuffer> Renderer::s_QuadFramebuffer = nullptr;
	std::unique_ptr<Doge::VertexArray> Renderer::s_QuadVertexArray = nullptr;
	std::shared_ptr<Doge::VertexBuffer> Renderer::s_QuadVertexBuffer = nullptr;
	std::shared_ptr<Doge::IndexBuffer> Renderer::s_QuadIndexBuffer = nullptr;
	std::shared_ptr<Doge::Shader> Renderer::s_QuadTexturedShader = nullptr;

	std::unique_ptr<VertexArray> Renderer::s_VertexArray = nullptr;
	std::unique_ptr<UniformBuffer> Renderer::s_ViewProjectionUniformBuffer = nullptr;
	std::unique_ptr<UniformBuffer> Renderer::s_LightingUniformBuffer = nullptr;
	std::unordered_map<std::shared_ptr<Material>, std::queue<RenderData>> Renderer::s_RenderQueue;

	const Shader* Renderer::s_LastShaderState = nullptr;

	void Renderer::Init(const WindowProps& props)
	{
		TextureManager::Init();
		RendererCommands::Init();

		// Create main Vertex Array for rendering objects
		s_VertexArray = VertexArray::Create();
		s_VertexArray->Bind();
		s_VertexArray->SetBufferLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::UInt , "a_TexIndex" }
			}, 0);

		// Create Framebuffer for Post-Processing Effects
		s_QuadFramebuffer = Framebuffer::Create(FramebufferSpecification(props.Width, props.Height));
		// Create Vertex Array, Vertex Buffer and Index Buffer for rendering Framebuffer
		s_QuadVertexArray = VertexArray::Create();
		s_QuadVertexArray->Bind();
		s_QuadVertexArray->SetBufferLayout({
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		}, 0);
		{
#pragma pack(push, 1)
			struct QuadVertex
			{
				QuadVertex(const glm::vec2& position, const glm::vec2& texCoord)
					: a_Position(position), a_TexCoord(texCoord) {}

				glm::vec2 a_Position;
				glm::vec2 a_TexCoord;
			};
#pragma pack(pop)
			QuadVertex vertices[4] = {
				QuadVertex({-1.0f, -1.0f}, {0.0f, 0.0f}),
				QuadVertex({ 1.0f, -1.0f}, {1.0f, 0.0f}),
				QuadVertex({ 1.0f,  1.0f}, {1.0f, 1.0f}),
				QuadVertex({-1.0f,  1.0f}, {0.0f, 1.0f})
			};
			s_QuadVertexBuffer = VertexBuffer::Create(&vertices->a_Position.x, sizeof(QuadVertex) * 4);
			s_QuadVertexBuffer->SetLayout({
				{ ShaderDataType::Float2, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
			});
			s_QuadVertexArray->BindVertexBuffer(s_QuadVertexBuffer, 0);

			uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
			s_QuadIndexBuffer = IndexBuffer::Create(indices, 6);
			s_QuadVertexArray->BindIndexBuffer(s_QuadIndexBuffer);
		}
		// Create TexturedQuad Shader program for rendering Framebuffer
		s_QuadTexturedShader = ShaderLibrary::CreateShader("TexturedQuad", TexturedQuadVertexShader, TexturedQuadFragmentShader);
		s_QuadTexturedShader->Bind();
		s_QuadTexturedShader->SetUniformInt("u_Texture", 0);

		// ViewProjection Uniform Buffer: binding = 1
		s_ViewProjectionUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4) * 2, 1);
		// Lighting Uniform buffer: binding = 2
		s_LightingUniformBuffer = UniformBuffer::Create(sizeof(glm::vec4) * 5, 2);

		// Set Lighting properties
		{
			s_LightingUniformBuffer->Bind();
			glm::vec3 ambient(0.5f);
			glm::vec3 diffuse(0.5f);
			glm::vec3 specular(1.0f);
			s_LightingUniformBuffer->SetData(&ambient.x, sizeof(glm::vec3), sizeof(glm::vec4) * 2);
			s_LightingUniformBuffer->SetData(&diffuse.x, sizeof(glm::vec3), sizeof(glm::vec4) * 3);
			s_LightingUniformBuffer->SetData(&specular.x, sizeof(glm::vec3), sizeof(glm::vec4) * 4);
		}
	}

	void Renderer::Submit(const RenderData& data)
	{
		s_RenderQueue[data.material].emplace(data);
	}

	void Renderer::PrepareBufferObjects(const Camera& camera)
	{
		// Set Uniform Buffer properties
		s_ViewProjectionUniformBuffer->Bind();
		s_ViewProjectionUniformBuffer->SetData(&camera.GetViewMatrix()[0], sizeof(glm::mat4), 0);
		s_ViewProjectionUniformBuffer->SetData(&camera.GetProjectionMatrix()[0], sizeof(glm::mat4), sizeof(glm::mat4));

		s_LightingUniformBuffer->Bind();
		s_LightingUniformBuffer->SetData(&camera.GetPosition()[0], sizeof(glm::vec3), 0);
		// Temporary: Light direction is set same as Camera direction
		s_LightingUniformBuffer->SetData(&camera.CalculateViewDirection()[0], sizeof(glm::vec3), sizeof(glm::vec4));
	}

	void Renderer::RenderObjectIndexed(const RenderData& renderData)
	{
		// Bind Buffers
		uint32_t binding = 0;
		for (const auto& vertexBuffer : renderData.VBOs)
		{
			s_VertexArray->BindVertexBuffer(vertexBuffer, binding);
			binding++;
		}
		s_VertexArray->BindIndexBuffer(renderData.IBO);

		RendererCommands::DrawIndexed(renderData.IBO->GetCount());
	}

	void Renderer::RenderIndexed(const Camera& camera)
	{
		// Render Objects into Framebuffer
		s_VertexArray->Bind();
		s_QuadFramebuffer->Bind();

		RendererCommands::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RendererCommands::Clear();

		PrepareBufferObjects(camera);
		for (auto& materialLayer : s_RenderQueue)
		{
			// Set Common Material properties
			const Shader& materialShader = materialLayer.first->GetShaderRef();
			if (s_LastShaderState == nullptr || *s_LastShaderState != materialShader)
			{
				materialShader.Bind();
				s_LastShaderState = &materialShader;
			}
			materialLayer.first->SetSharedUniforms();

			auto& renderQueue = materialLayer.second;
			while (!renderQueue.empty())
			{
				const auto& renderData = renderQueue.front();
				// Set Unique Material properties and the Model matrix
				renderData.material->SetModifiedUniforms();
				renderData.material->GetShaderRef().SetUniformMat4("u_Model", renderData.modelMatrix);
				RenderObjectIndexed(renderData);
				renderQueue.pop();
			}
		}
		s_QuadFramebuffer->Unbind();
		s_LastShaderState = nullptr;

		// Render the Framebuffer
		s_QuadFramebuffer->BindColorAttachment(0);
		s_QuadTexturedShader->Bind();
		s_QuadVertexArray->Bind();
		s_QuadVertexArray->BindVertexBuffer(s_QuadVertexBuffer, 0);
		s_QuadVertexArray->BindIndexBuffer(s_QuadIndexBuffer);
		RendererCommands::DrawIndexed(s_QuadIndexBuffer->GetCount());
	}
}