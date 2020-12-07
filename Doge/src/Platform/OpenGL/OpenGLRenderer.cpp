#include "pch.h"
#include <glad/glad.h>

#include "OpenGLBuffer.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLConstants.h"
#include "OpenGLRenderer.h"
#include "OpenGLVertexArray.h"
#include "OpenGLFramebuffer.h"

#include "Doge/Core/Window.h"

// Temporary
#include "Doge/Scene/SceneObject3D.h"

namespace Doge
{
	OpenGLRenderer::OpenGLRenderer(const WindowProps& props)
		: m_SkyboxVertexArray(nullptr), m_SkyboxShader(nullptr)
	{
		ConstructScreenQuadProperties(props);
		ConstructMainProperties(props);

		glEnable(GL_MULTISAMPLE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		Log::Trace("Renderer initialized successfully!");
	}

	void OpenGLRenderer::BeginRender(const Camera& camera)
	{
		// Bind the Multisampled Framebuffer and the Main Vertex Array
		m_MainFramebuffer->Bind();
		m_MainVertexArray->Bind();

		// Setup Rendering properties and Clear the Framebuffer's previous contents
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update Uniform Buffers' contents
		PrepareBufferObjects(camera);
	}

	void OpenGLRenderer::EndRender()
	{
		m_MainFramebuffer->BlitMultisampled(*m_QuadFramebuffer);
		m_MainFramebuffer->Unbind();
		m_QuadFramebuffer->BindColorAttachment(OpenGLBindings::QuadFramebufferColorAttachment);
		m_LastShaderState = nullptr;

		// Render Framebuffer to the Screen
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		// Bind screen sized Quad Framebuffer and appropriate Vertex Array and its buffers
		m_QuadTexturedShader->Bind();
		m_QuadVertexArray->Bind();

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::RenderObjectsIndexed()
	{
		glCullFace(GL_BACK);

		// For each unique Material, Retrieve the RenderQueue
		for (auto& materialLayer : m_RenderQueue)
		{
			// Set common Material properties and Bind the Shader if already not bound
			const OpenGLShader& materialShader = (*OpenGLShaderCache::GetInstance())[materialLayer.first->GetShader()];
			if (m_LastShaderState == nullptr || *m_LastShaderState != materialShader)
			{
				materialShader.Bind();
				m_LastShaderState = &materialShader;
			}
			BindCommonUniformProperties(materialLayer.first);

			// For each object in the current Material's RenderQueue, Draw the Object
			auto& renderQueue = materialLayer.second;
			while (!renderQueue.empty())
			{
				const auto& renderData = renderQueue.front();
				// Set Unique Material properties and the Model matrix
				BindUniqueUniformProperties(renderData.MaterialInstanceRef);
				materialShader.SetUniformMat4("u_Model", renderData.Transform);
				DrawIndexed(renderData.MeshData.VBOs, renderData.MeshData.IBO);
				renderQueue.pop();
			}
		}
	}

	void OpenGLRenderer::RenderOutlinedObjectsIndexed()
	{
		glCullFace(GL_FRONT);

		m_ObjectOutliningShader->Bind();
		m_ObjectOutliningShader->SetUniformFloat3("u_OutlineColor", { 0.8f, 0.5f, 0.2f });

		while (!m_OutlinedObjectsQueue.empty())
		{
			auto& renderData = m_OutlinedObjectsQueue.front();

			renderData.Transform = glm::scale(renderData.Transform, glm::vec3(1.03f));
			m_ObjectOutliningShader->SetUniformMat4("u_Model", renderData.Transform);
			DrawIndexed(renderData);

			m_OutlinedObjectsQueue.pop();
		}
	}

	void OpenGLRenderer::RenderLightObjectsIndexed()
	{
		m_ObjectOutliningShader->SetUniformFloat3("u_OutlineColor", { 1.0f, 1.0f, 1.0f });
		m_ObjectOutliningShader->SetUniformMat4("u_Model", m_PointLight->Transform);
		DrawIndexed(*m_PointLight);
	}

	void OpenGLRenderer::RenderSkybox()
	{
		if (!m_SkyboxShader || !m_SkyboxVertexArray)
		{
			glDepthFunc(GL_LEQUAL);

			m_SkyboxVertexArray->Bind();
			m_SkyboxShader->Bind();
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

			glDepthFunc(GL_LESS);
		}
	}

	void OpenGLRenderer::DrawIndexed(const std::vector<OpenGLVertexBuffer>& VBOs, const OpenGLIndexBuffer& IBO)
	{
		uint32_t binding = 0;
		for (const auto& vertexBuffer : VBOs)
		{
			m_MainVertexArray->BindVertexBuffer(vertexBuffer, binding);
			binding++;
		}
		m_MainVertexArray->BindIndexBuffer(IBO);

		glDrawElements(GL_TRIANGLES, IBO.GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::SetSkybox(CubemapTexture skybox)
	{
		if (!m_SkyboxShader || !m_SkyboxVertexArray)
			ConstructSkyboxProperties();

		glBindTextureUnit(OpenGLBindings::SkyboxTextureUnit, skybox);

		m_SkyboxShader->Bind();
		m_SkyboxShader->SetUniformInt("u_Skybox", OpenGLBindings::SkyboxTextureUnit);
	}

	void OpenGLRenderer::ClearSkybox()
	{
		glBindTextureUnit(OpenGLBindings::SkyboxTextureUnit, 0);

		m_SkyboxShader.reset();
		m_SkyboxVertexArray.reset();
	}

	void OpenGLRenderer::BindCommonUniformProperties(const Ref<Material>& material) const
	{
		for (const auto& [name, prop] : material->GetProperties())
		{
			switch (static_cast<Material::DataIndex>(prop.index()))
			{
				case Material::DataIndex::Float:
				{
					float value = std::get<float>(prop);
					m_LastShaderState->SetUniformFloat(name.c_str(), value);
					break;
				}
				case Material::DataIndex::Float2:
				{
					const glm::vec2& value = std::get<glm::vec2>(prop);
					m_LastShaderState->SetUniformFloat2(name.c_str(), value);
					break;
				}
				case Material::DataIndex::Float3:
				{
					const glm::vec3& value = std::get<glm::vec3>(prop);
					m_LastShaderState->SetUniformFloat3(name.c_str(), value);
					break;
				}
				case Material::DataIndex::Float4:
				{
					const glm::vec4& value = std::get<glm::vec4>(prop);
					m_LastShaderState->SetUniformFloat4(name.c_str(), value);
					break;
				}
			}
		}
	}

	void OpenGLRenderer::BindUniqueUniformProperties(const Ref<MaterialInstance>& materialInstance) const
	{
		for (const auto& [name, prop] : materialInstance->GetModifiedProperties())
		{
			switch (static_cast<Material::DataIndex>(prop.index()))
			{
				case Material::DataIndex::Float:
				{
					float value = std::get<float>(prop);
					m_LastShaderState->SetUniformFloat(name.c_str(), value);
					break;
				}
				case Material::DataIndex::Float2:
				{
					const glm::vec2& value = std::get<glm::vec2>(prop);
					m_LastShaderState->SetUniformFloat2(name.c_str(), value);
					break;
				}
				case Material::DataIndex::Float3:
				{
					const glm::vec3& value = std::get<glm::vec3>(prop);
					m_LastShaderState->SetUniformFloat3(name.c_str(), value);
					break;
				}
				case Material::DataIndex::Float4:
				{
					const glm::vec4& value = std::get<glm::vec4>(prop);
					m_LastShaderState->SetUniformFloat4(name.c_str(), value);
					break;
				}
			}
		}

		for (const auto& [name, prop] : materialInstance->GetAddedProperties())
		{
			switch (static_cast<Material::DataIndex>(prop.index()))
			{
				case Material::DataIndex::Float:
				{
					float value = std::get<float>(prop);
					m_LastShaderState->SetUniformFloat(name.c_str(), value);
					break;
				}
				case Material::DataIndex::Float2:
				{
					const glm::vec2& value = std::get<glm::vec2>(prop);
					m_LastShaderState->SetUniformFloat2(name.c_str(), value);
					break;
				}
				case Material::DataIndex::Float3:
				{
					const glm::vec3& value = std::get<glm::vec3>(prop);
					m_LastShaderState->SetUniformFloat3(name.c_str(), value);
					break;
				}
				case Material::DataIndex::Float4:
				{
					const glm::vec4& value = std::get<glm::vec4>(prop);
					m_LastShaderState->SetUniformFloat4(name.c_str(), value);
					break;
				}
			}
		}
	}

	void OpenGLRenderer::ResizeFramebuffer(uint32_t width, uint32_t height)
	{
		m_MainFramebuffer->Resize(width, height);
		m_QuadFramebuffer->Resize(width, height);
	}

	void OpenGLRenderer::ConstructScreenQuadProperties(const WindowProps& props)
	{
		// Create Framebuffer for Post-Processing Effects
		m_QuadFramebuffer = CreateScope<OpenGLFramebuffer>(FramebufferSpecification(props.Width, props.Height));
		m_QuadFramebuffer->Bind();
		m_QuadFramebuffer->BindColorAttachment(OpenGLBindings::QuadFramebufferColorAttachment);

		// Create Vertex Array, Vertex Buffer and Index Buffer for rendering Framebuffer
		m_QuadVertexArray = CreateScope<OpenGLVertexArray>();
		m_QuadVertexArray->Bind();
		m_QuadVertexArray->SetBufferLayout({
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		}, 0);
		{
			FramebufferQuadVertex vertices[4] = {
				FramebufferQuadVertex({-1.0f, -1.0f}, {0.0f, 0.0f}),
				FramebufferQuadVertex({ 1.0f, -1.0f}, {1.0f, 0.0f}),
				FramebufferQuadVertex({ 1.0f,  1.0f}, {1.0f, 1.0f}),
				FramebufferQuadVertex({-1.0f,  1.0f}, {0.0f, 1.0f})
			};
			Scope<OpenGLVertexBuffer> QuadVertexBuffer = CreateScope<OpenGLVertexBuffer>(&vertices->a_Position.x, sizeof(FramebufferQuadVertex) * 4);
			QuadVertexBuffer->SetLayout({
				{ ShaderDataType::Float2, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
				});
			m_QuadVertexArray->BindVertexBuffer(*QuadVertexBuffer, 0);

			uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
			Scope<OpenGLIndexBuffer> QuadIndexBuffer = CreateScope<OpenGLIndexBuffer>(indices, 6);
			m_QuadVertexArray->BindIndexBuffer(*QuadIndexBuffer);
		}

		// Create TexturedQuad Shader program for rendering Framebuffer
		m_QuadTexturedShader = CreateScope<OpenGLShader>(OpenGLRendererShaders::TexturedQuad);
		m_QuadTexturedShader->Bind();
		m_QuadTexturedShader->SetUniformInt("u_Texture", OpenGLBindings::QuadFramebufferColorAttachment);
	}

	void OpenGLRenderer::ConstructMainProperties(const WindowProps& props)
	{
		m_MainFramebuffer = CreateScope<OpenGLFramebuffer>(FramebufferSpecification(props.Width, props.Height, 4, true));

		// Create main Vertex Array for rendering objects
		m_MainVertexArray = CreateScope<OpenGLVertexArray>();
		m_MainVertexArray->Bind();
		m_MainVertexArray->SetBufferLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::UInt , "a_TexIndex" }
		}, 0);

		// ViewProjection Uniform Buffer: binding = 1
		m_ViewProjectionUniformBuffer = CreateScope<OpenGLUniformBuffer>(sizeof(glm::mat4) * 2, OpenGLBindings::ViewProjectionUniformBuffer);
		// Lighting Uniform buffer: binding = 2
		m_LightingUniformBuffer = CreateScope<OpenGLUniformBuffer>(sizeof(glm::vec4) * 5, OpenGLBindings::LightingUniformBuffer);

		// Create ObjectOutlining Shader program for outlining selected objects
		m_ObjectOutliningShader = CreateScope<OpenGLShader>(OpenGLRendererShaders::ObjectOutlining);

		// Set Lighting properties
		m_LightingUniformBuffer->Bind();
		glm::vec3 position(6.0f, 4.0f, 5.0f);
		glm::vec3 ambient(0.6f);
		glm::vec3 diffuse(0.6f);
		glm::vec3 specular(0.2f);
		m_LightingUniformBuffer->SetData(&position.x, sizeof(glm::vec3), sizeof(glm::vec4));
		m_LightingUniformBuffer->SetData(&ambient.x, sizeof(glm::vec3), sizeof(glm::vec4) * 2);
		m_LightingUniformBuffer->SetData(&diffuse.x, sizeof(glm::vec3), sizeof(glm::vec4) * 3);
		m_LightingUniformBuffer->SetData(&specular.x, sizeof(glm::vec3), sizeof(glm::vec4) * 4);

		// Set Point Light properties for rendering
		Ref<Material> whiteColorMaterial = CreateRef<Material>(m_ObjectOutliningShader);
		whiteColorMaterial->SetColor({ 1.0f, 1.0f, 1.0f });
		Ref<MaterialInstance>  lightMaterialInstance = CreateRef<MaterialInstance>(whiteColorMaterial);

		Sphere pointLight(1.0f, 72, 72);
		RenderData pointLightData(MeshRendererData(pointLight.GetMesh()), lightMaterialInstance, glm::translate(glm::mat4(1.0f), position));
		m_PointLight = CreateScope<RenderData>(pointLightData);
	}

	void OpenGLRenderer::ConstructSkyboxProperties()
	{
		m_SkyboxVertexArray = CreateScope<OpenGLVertexArray>();
		m_SkyboxVertexArray->Bind();
		m_SkyboxVertexArray->SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" }
		}, 0);

		const auto& positions = Cube::CalculatePositions(glm::vec3(1.0f));
		Scope<OpenGLVertexBuffer> SkyboxVertexBuffer = CreateScope<OpenGLVertexBuffer>(&positions.data()->x, positions.size() * sizeof(glm::vec3));
		SkyboxVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
		});
		m_SkyboxVertexArray->BindVertexBuffer(*SkyboxVertexBuffer, 0);

		const auto& indices = Cube::CalculateIndices();
		Scope<OpenGLIndexBuffer> SkyboxIndexBuffer = CreateScope<OpenGLIndexBuffer>(indices.data(), indices.size());
		m_SkyboxVertexArray->BindIndexBuffer(*SkyboxIndexBuffer);

		m_SkyboxShader = CreateScope<OpenGLShader>(OpenGLRendererShaders::Skybox);
	}

	void OpenGLRenderer::PrepareBufferObjects(const Camera& camera)
	{
		m_ViewProjectionUniformBuffer->Bind();
		// Set mat4 u_View
		m_ViewProjectionUniformBuffer->SetData(&camera.GetViewMatrix()[0], sizeof(glm::mat4), 0);
		// Set mat4 u_Projection
		m_ViewProjectionUniformBuffer->SetData(&camera.GetProjectionMatrix()[0], sizeof(glm::mat4), sizeof(glm::mat4));

		m_LightingUniformBuffer->Bind();
		// Set vec3 u_CameraPos
		m_LightingUniformBuffer->SetData(&camera.GetPosition()[0], sizeof(glm::vec3), 0);
	}
}