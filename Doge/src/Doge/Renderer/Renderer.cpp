#include "pch.h"
#include "Renderer.h"
#include "RendererCommands.h"

#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Framebuffer.h"

#include "Doge/Core/Window.h"

#include "Doge/Utility/Camera.h"
#include "Doge/Utility/Material.h"
#include "Doge/Utility/SceneObject3D.h"

namespace Doge
{
	std::unique_ptr<Doge::Framebuffer> Renderer::s_Framebuffer = nullptr;
	std::unique_ptr<VertexArray> Renderer::s_VertexArray = nullptr;
	std::unique_ptr<UniformBuffer> Renderer::s_ViewProjectionUniformBuffer = nullptr;
	std::unique_ptr<UniformBuffer> Renderer::s_LightingUniformBuffer = nullptr;
	std::unordered_map<std::shared_ptr<Material>, std::queue<RenderData>> Renderer::s_RenderQueue;

	const Shader* Renderer::s_LastShaderState = nullptr;

	void Renderer::Init(const WindowProps& props)
	{
		TextureManager::Init();

		s_Framebuffer.reset(new Framebuffer(	FramebufferSpecification(props.Width, props.Height)));

		s_VertexArray.reset(new VertexArray);
		s_VertexArray->Bind();

		s_VertexArray->SetBufferLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::UInt , "a_TexIndex" }
			}, 0);

		// ViewProjection Uniform Buffer: binding = 1
		s_ViewProjectionUniformBuffer.reset(new UniformBuffer(sizeof(glm::mat4) * 2, 1));
		// Lighting Uniform buffer: binding = 2
		s_LightingUniformBuffer.reset(new UniformBuffer(sizeof(glm::vec4) * 5, 2));

		// Set Lighting properties
		s_LightingUniformBuffer->Bind();
		glm::vec3 ambient(0.5f);
		glm::vec3 diffuse(0.5f);
		glm::vec3 specular(1.0f);
		s_LightingUniformBuffer->SetData(&ambient.x, sizeof(glm::vec3), sizeof(glm::vec4) * 2);
		s_LightingUniformBuffer->SetData(&diffuse.x, sizeof(glm::vec3), sizeof(glm::vec4) * 3);
		s_LightingUniformBuffer->SetData(&specular.x, sizeof(glm::vec3), sizeof(glm::vec4) * 4);
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
		s_LightingUniformBuffer->SetData(&camera.GetViewDirection()[0], sizeof(glm::vec3), sizeof(glm::vec4));
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

		glDrawElements(GL_TRIANGLES, renderData.IBO->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::DrawIndexed(const Camera& camera)
	{
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
	}
}