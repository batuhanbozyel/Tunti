#include "pch.h"
#include "Renderer.h"
#include "RendererCommands.h"
#include "RendererBaseShaders.h"

#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Framebuffer.h"
#include "ShaderDataType.h"

#include "Doge/Core/Window.h"

#include "Doge/Utility/Camera.h"
#include "Doge/Utility/Material.h"

// Temporary
#include "Doge/Scene/SceneObject3D.h"

namespace Doge
{
	RendererAPI Renderer::s_API = RendererAPI::None;
	Scope<Renderer> Renderer::s_Instance = nullptr;

	void Renderer::Init(const WindowProps& props)
	{
		TextureManager::Init();
		RendererCommands::Init();

		s_Instance = CreateScope<Renderer>(props);

		RendererCommands::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	}

	Renderer::Renderer(const WindowProps& props)
	{
		if (s_Instance == nullptr)
		{
			ConstructScreenQuadProperties(props);
			ConstructMainProperties(props);

			Log::Trace("Renderer initialized successfully!");
		}
		else
		{
			Log::Warn("Renderer already initialized!");
		}
	}

	void Renderer::PrepareBufferObjects(const Camera& camera)
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

	void Renderer::BeginRender(const Camera& camera)
	{
		// Bind the Multisampled Framebuffer and the Main Vertex Array
		m_MainFramebuffer->Bind();
		m_MainVertexArray->Bind();

		// Setup Rendering properties and Clear the Framebuffer's previous contents
		RendererCommands::EnableFaceCulling();
		RendererCommands::EnableDepthTest();
		RendererCommands::Clear();

		// Update Uniform Buffers' contents
		PrepareBufferObjects(camera);
	}

	void Renderer::EndRender()
	{
		m_MainFramebuffer->BlitMultisampled(m_QuadFramebuffer->GetFramebufferID());
		m_MainFramebuffer->Unbind();
		m_QuadFramebuffer->BindColorAttachment(TextureBinding::QuadFramebufferColorAttachment);
		m_LastShaderState = nullptr;
	}

	void Renderer::RenderObjectsIndexed()
	{
		RendererCommands::SetFaceCulling(CullFace::Back);

		// For each unique Material, Retrieve the RenderQueue
		for (auto& materialLayer : m_RenderQueue)
		{
			// Set common Material properties and Bind the Shader if already not bound
			const Ref<Shader>& materialShader = materialLayer.first->GetShaderRef();
			if (m_LastShaderState == nullptr || *m_LastShaderState != *materialShader)
			{
				materialShader->Bind();
				m_LastShaderState = materialShader.get();
			}
			materialLayer.first->AssignCommonUniforms();

			// For each object in the current Material's RenderQueue, Draw the Object
			auto& renderQueue = materialLayer.second;
			while (!renderQueue.empty())
			{
				const auto& renderData = renderQueue.front();
				// Set Unique Material properties and the Model matrix
				renderData.MaterialInstanceRef->AssignUniqueUniforms();
				renderData.MaterialInstanceRef->GetShaderRef()->SetUniformMat4("u_Model", renderData.ModelMatrix);
				DrawIndexed(renderData);
				renderQueue.pop();
			}
		}
	}

	void Renderer::RenderOutlinedObjectsIndexed()
	{
		RendererCommands::SetFaceCulling(CullFace::Front);

		m_ObjectOutliningShader->Bind();
		m_ObjectOutliningShader->SetUniformFloat3("u_OutlineColor", { 0.8f, 0.5f, 0.2f });
		while (!m_OutlineRenderQueue.empty())
		{
			auto& renderData = m_OutlineRenderQueue.front();
			renderData.ModelMatrix = glm::scale(renderData.ModelMatrix, glm::vec3(1.03f));
			m_ObjectOutliningShader->SetUniformMat4("u_Model", renderData.ModelMatrix);
			DrawIndexed(renderData);
			m_OutlineRenderQueue.pop();
		}
	}

	void Renderer::RenderLightObjectsIndexed()
	{
 		m_ObjectOutliningShader->SetUniformFloat3("u_OutlineColor", { 1.0f, 1.0f, 1.0f });
 		m_ObjectOutliningShader->SetUniformMat4("u_Model", m_PointLight->ModelMatrix);
		DrawIndexed(*m_PointLight);
	}

	void Renderer::RenderSkybox()
	{
		if (m_Skybox)
		{
			RendererCommands::SetFaceCulling(CullFace::Back);
			m_SkyboxVertexArray->Bind();
			m_SkyboxShader->Bind();
			RendererCommands::DrawIndexed(36);
		}
	}

	void Renderer::RenderFramebuffer()
	{
		RendererCommands::DisableDepthTest();
		RendererCommands::DisableFaceCulling();

		// Bind screen sized Quad Framebuffer and appropriate Vertex Array and its buffers
		m_QuadTexturedShader->Bind();
		m_QuadVertexArray->Bind();

		RendererCommands::DrawIndexed(6);
	}

	void Renderer::Submit(const RenderData& data)
	{
		// Emplace the Object to the appropriate Material bucket in the Map
		s_Instance->m_RenderQueue[data.MaterialInstanceRef->GetBaseMaterialRef()].emplace(data);

		// Emplace the Object to the Outline RenderQueue if marked as Selected
		if (data.Selected)
			s_Instance->m_OutlineRenderQueue.emplace(data);
	}

	void Renderer::SetSkybox(const Ref<CubemapTexture>& skybox)
	{
		if (s_Instance->m_Skybox == nullptr)
			s_Instance->ConstructSkyboxProperties();

		skybox->Bind(TextureBinding::SkyboxTextureUnit);
		s_Instance->m_Skybox = skybox;
		s_Instance->m_SkyboxShader->Bind();
		s_Instance->m_SkyboxShader->SetUniformInt("u_Skybox", TextureBinding::SkyboxTextureUnit);
	}

	void Renderer::DrawIndexed(const RenderData& renderData)
	{
		// Bind Buffers
		uint32_t binding = 0;
		for (const auto& vertexBuffer : renderData.VBOs)
		{
			m_MainVertexArray->BindVertexBuffer(*vertexBuffer, binding);
			binding++;
		}
		m_MainVertexArray->BindIndexBuffer(*renderData.IBO);

		RendererCommands::DrawIndexed(renderData.IBO->GetCount());
	}

	void Renderer::RenderIndexed(const Camera& camera)
	{
		// Render in between Begin and End calls
		s_Instance->BeginRender(camera);
		{
			// Render objects normally
			s_Instance->RenderObjectsIndexed();

			// Render objects in single color with vertices expanded along their normal
			s_Instance->RenderOutlinedObjectsIndexed();

			// Render Light objects in white color
			// Make sure Lights are rendered right after Object Outlining
			// So, We don't have to bind the same shader again
			s_Instance->RenderLightObjectsIndexed();

			// Render Skybox last for optimization
			s_Instance->RenderSkybox();
		}
		s_Instance->EndRender();

		// Render Framebuffer to the screen
		s_Instance->RenderFramebuffer();
	}

	void Renderer::OnWindowResize(WindowResizeEvent& e)
	{
		s_Instance->m_MainFramebuffer->Resize(e.GetWidth(), e.GetHeight());
		s_Instance->m_QuadFramebuffer->Resize(e.GetWidth(), e.GetHeight());
	}

	void Renderer::ConstructScreenQuadProperties(const WindowProps& props)
	{
		// Create Framebuffer for Post-Processing Effects
		m_QuadFramebuffer = Framebuffer::Create(FramebufferSpecification(props.Width, props.Height));
		m_QuadFramebuffer->Bind();
		m_QuadFramebuffer->BindColorAttachment(TextureBinding::QuadFramebufferColorAttachment);
		// Create Vertex Array, Vertex Buffer and Index Buffer for rendering Framebuffer
		m_QuadVertexArray = VertexArray::Create();
		m_QuadVertexArray->Bind();
		m_QuadVertexArray->SetBufferLayout({
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		}, 0);
		{
			QuadVertex vertices[4] = {
				QuadVertex({-1.0f, -1.0f}, {0.0f, 0.0f}),
				QuadVertex({ 1.0f, -1.0f}, {1.0f, 0.0f}),
				QuadVertex({ 1.0f,  1.0f}, {1.0f, 1.0f}),
				QuadVertex({-1.0f,  1.0f}, {0.0f, 1.0f})
			};
			Scope<VertexBuffer> QuadVertexBuffer = VertexBuffer::Create(&vertices->a_Position.x, sizeof(QuadVertex) * 4);
			QuadVertexBuffer->SetLayout({
				{ ShaderDataType::Float2, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
			});
			m_QuadVertexArray->BindVertexBuffer(*QuadVertexBuffer, 0);

			uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
			Scope<IndexBuffer> QuadIndexBuffer = IndexBuffer::Create(indices, 6);
			m_QuadVertexArray->BindIndexBuffer(*QuadIndexBuffer);
		}
		// Create TexturedQuad Shader program for rendering Framebuffer
		m_QuadTexturedShader = ShaderLibrary::CreateShader("TexturedQuad", TexturedQuadVertexShader.data(), TexturedQuadFragmentShader.data());
		m_QuadTexturedShader->Bind();
		m_QuadTexturedShader->SetUniformInt("u_Texture", TextureBinding::QuadFramebufferColorAttachment);
	}

	void Renderer::ConstructMainProperties(const WindowProps& props)
	{
		m_MainFramebuffer = Framebuffer::Create(FramebufferSpecification(props.Width, props.Height, 4, true));
		// Create main Vertex Array for rendering objects
		m_MainVertexArray = VertexArray::Create();
		m_MainVertexArray->Bind();
		m_MainVertexArray->SetBufferLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::UInt , "a_TexIndex" }
		}, 0);

		// ViewProjection Uniform Buffer: binding = 1
		m_ViewProjectionUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4) * 2, BufferBinding::ViewProjectionUniformBuffer);
		// Lighting Uniform buffer: binding = 2
		m_LightingUniformBuffer = UniformBuffer::Create(sizeof(glm::vec4) * 5, BufferBinding::LightingUniformBuffer);

		// Create ObjectOutlining Shader program for outlining selected objects
		m_ObjectOutliningShader = ShaderLibrary::CreateShader("ObjectOutlining", ObjectOutliningVertexShader.data(), ObjectOutliningFragmentShader.data());

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
		whiteColorMaterial->SetBaseColor({ 1.0f, 1.0f, 1.0f });
		Ref<MaterialInstance>  lightMaterialInstance = CreateRef<MaterialInstance>(whiteColorMaterial);

		Sphere pointLight(1.0f);
		RenderData pointLightData = RenderDataManager::Construct(pointLight.GetMesh(), lightMaterialInstance);
		pointLightData.ModelMatrix = glm::translate(glm::mat4(1.0f), position);
		m_PointLight.reset(new RenderData(pointLightData));
	}

	void Renderer::ConstructSkyboxProperties()
	{
		m_SkyboxVertexArray = VertexArray::Create();
		m_SkyboxVertexArray->Bind();
		m_SkyboxVertexArray->SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" }
		}, 0);

		const auto& positions = Cube::CalculatePositions(glm::vec3(1.0f));
		Scope<VertexBuffer> SkyboxVertexBuffer = VertexBuffer::Create(&positions.data()->x, positions.size() * sizeof(glm::vec3));
		SkyboxVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
		});
		m_SkyboxVertexArray->BindVertexBuffer(*SkyboxVertexBuffer, 0);

		const auto& indices = Cube::CalculateIndices();
		Scope<IndexBuffer> SkyboxIndexBuffer = IndexBuffer::Create(indices.data(), indices.size());
		m_SkyboxVertexArray->BindIndexBuffer(*SkyboxIndexBuffer);

		m_SkyboxShader = ShaderLibrary::CreateShader("SkyboxShader", SkyboxVertexShader.data(), SkyboxFragmentShader.data());
	}
}