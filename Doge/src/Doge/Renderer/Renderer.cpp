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
#include "Doge/Utility/SceneObject3D.h"

namespace Doge
{
	RendererAPI Renderer::s_API = RendererAPI::None;
	Renderer* Renderer::s_Renderer = nullptr;

	void Renderer::Init(const WindowProps& props)
	{
		TextureManager::Init();
		RendererCommands::Init();

		s_Renderer = new Renderer(props);

		RendererCommands::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	}

	Renderer::Renderer(const WindowProps& props)
	{
		s_MainFramebuffer = Framebuffer::Create(FramebufferSpecification(props.Width, props.Height, 4, true));
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
		s_QuadFramebuffer = Framebuffer::Create(FramebufferSpecification(props.Width, props.Height, 1, false));
		s_QuadFramebuffer->Bind();
		s_QuadFramebuffer->BindColorAttachment(0);
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
			s_QuadVertexArray->BindVertexBuffer(*s_QuadVertexBuffer, 0);

			uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
			s_QuadIndexBuffer = IndexBuffer::Create(indices, 6);
			s_QuadVertexArray->BindIndexBuffer(*s_QuadIndexBuffer);
		}
		// Create TexturedQuad Shader program for rendering Framebuffer
		s_QuadTexturedShader = ShaderLibrary::CreateShader("TexturedQuad", TexturedQuadVertexShader.data(), TexturedQuadFragmentShader.data());
		s_QuadTexturedShader->Bind();
		s_QuadTexturedShader->SetUniformInt("u_Texture", 0);

		// ViewProjection Uniform Buffer: binding = 1
		s_ViewProjectionUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4) * 2, 1);
		// Lighting Uniform buffer: binding = 2
		s_LightingUniformBuffer = UniformBuffer::Create(sizeof(glm::vec4) * 5, 2);

		// Create ObjectOutlining Shader program for outlining selected objects
		s_ObjectOutliningShader = ShaderLibrary::CreateShader("ObjectOutlining", ObjectOutliningVertexShader.data(), ObjectOutliningFragmentShader.data());

		// Set Lighting properties
		s_LightingUniformBuffer->Bind();
		glm::vec3 position(6.0f, 4.0f, 5.0f);
		glm::vec3 ambient(0.6f);
		glm::vec3 diffuse(0.6f);
		glm::vec3 specular(0.2f);
		s_LightingUniformBuffer->SetData(&position.x, sizeof(glm::vec3), sizeof(glm::vec4));
		s_LightingUniformBuffer->SetData(&ambient.x, sizeof(glm::vec3), sizeof(glm::vec4) * 2);
		s_LightingUniformBuffer->SetData(&diffuse.x, sizeof(glm::vec3), sizeof(glm::vec4) * 3);
		s_LightingUniformBuffer->SetData(&specular.x, sizeof(glm::vec3), sizeof(glm::vec4) * 4);

		// Set Point Light properties for rendering
		Sphere pointLight(1.0f);
		Ref<Material> lightMaterial = CreateRef<Material>(*s_ObjectOutliningShader);
		RenderData pointLightData = RenderDataManager::Construct(pointLight.GetMesh(), lightMaterial);
		pointLightData.ModelMatrix = glm::translate(glm::mat4(1.0f), position);
		s_PointLight.reset(new RenderData(pointLightData));
	}

	Renderer::~Renderer()
	{
		delete s_Renderer;
	}

	void Renderer::PrepareBufferObjects(const Camera& camera)
	{
		s_ViewProjectionUniformBuffer->Bind();
		// Set mat4 u_View
		s_ViewProjectionUniformBuffer->SetData(&camera.GetViewMatrix()[0], sizeof(glm::mat4), 0);
		// Set mat4 u_Projection
		s_ViewProjectionUniformBuffer->SetData(&camera.GetProjectionMatrix()[0], sizeof(glm::mat4), sizeof(glm::mat4));

		s_LightingUniformBuffer->Bind();
		// Set vec3 u_CameraPos
		s_LightingUniformBuffer->SetData(&camera.GetPosition()[0], sizeof(glm::vec3), 0);
	}

	void Renderer::BeginRender(const Camera& camera)
	{
		// Bind the Multisampled Framebuffer and the Main Vertex Array
		s_MainFramebuffer->Bind();
		s_VertexArray->Bind();

		// Setup Rendering properties and Clear the Framebuffer's previous contents
		RendererCommands::EnableFaceCulling();
		RendererCommands::EnableDepthTest();
		RendererCommands::Clear();

		// Update Uniform Buffers' contents
		PrepareBufferObjects(camera);
	}

	void Renderer::EndRender()
	{
		s_MainFramebuffer->BlitMultisampled(s_QuadFramebuffer->GetFramebufferID());
		s_MainFramebuffer->Unbind();
		s_QuadFramebuffer->BindColorAttachment(0);
		s_LastShaderState = nullptr;
	}

	void Renderer::RenderObjectsIndexed()
	{
		RendererCommands::SetFaceCulling(CullFace::Back);

		// For each unique Material, Retrieve the RenderQueue
		for (auto& materialLayer : s_RenderQueue)
		{
			// Set common Material properties and Bind the Shader if already not bound
			const Shader& materialShader = materialLayer.first->GetShaderRef();
			if (s_LastShaderState == nullptr || *s_LastShaderState != materialShader)
			{
				materialShader.Bind();
				s_LastShaderState = &materialShader;
			}
			materialLayer.first->SetSharedUniforms();

			// For each object in the current Material's RenderQueue, Draw the Object
			auto& renderQueue = materialLayer.second;
			while (!renderQueue.empty())
			{
				const auto& renderData = renderQueue.front();
				// Set Unique Material properties and the Model matrix
				renderData.MaterialRef->SetModifiedUniforms();
				renderData.MaterialRef->GetShaderRef().SetUniformMat4("u_Model", renderData.ModelMatrix);
				DrawIndexed(renderData);
				renderQueue.pop();
			}
		}
	}

	void Renderer::RenderOutlinedObjectsIndexed()
	{
		RendererCommands::SetFaceCulling(CullFace::Front);

		s_ObjectOutliningShader->Bind();
		s_ObjectOutliningShader->SetUniformFloat3("u_OutlineColor", { 0.8f, 0.5f, 0.2f });
		while (!s_OutlineRenderQueue.empty())
		{
			auto& renderData = s_OutlineRenderQueue.front();
			renderData.ModelMatrix = glm::scale(renderData.ModelMatrix, glm::vec3(1.03f));
			s_ObjectOutliningShader->SetUniformMat4("u_Model", renderData.ModelMatrix);
			DrawIndexed(renderData);
			s_OutlineRenderQueue.pop();
		}
	}

	void Renderer::RenderLightObjectsIndexed()
	{
		s_ObjectOutliningShader->SetUniformFloat3("u_OutlineColor", { 1.0f, 1.0f, 1.0f });
		s_ObjectOutliningShader->SetUniformMat4("u_Model", s_PointLight->ModelMatrix);
		DrawIndexed(*s_PointLight);
	}

	void Renderer::RenderFramebuffer()
	{
		RendererCommands::DisableDepthTest();
		RendererCommands::DisableFaceCulling();

		// Bind screen sized Quad Framebuffer and appropriate Vertex Array and its buffers
		s_QuadTexturedShader->Bind();
		s_QuadVertexArray->Bind();
		s_QuadVertexArray->BindVertexBuffer(*s_QuadVertexBuffer, 0);
		s_QuadVertexArray->BindIndexBuffer(*s_QuadIndexBuffer);

		RendererCommands::DrawIndexed(s_QuadIndexBuffer->GetCount());
	}

	void Renderer::Submit(const RenderData& data)
	{
		// Emplace the Object to the appropriate Material bucket in the Map
		s_Renderer->s_RenderQueue[data.MaterialRef].emplace(data);

		// Emplace the Object to the Outline RenderQueue if marked as Selected
		if (data.Selected)
			s_Renderer->s_OutlineRenderQueue.emplace(data);
	}

	void Renderer::DrawIndexed(const RenderData& renderData)
	{
		// Bind Buffers
		uint32_t binding = 0;
		for (const auto& vertexBuffer : renderData.VBOs)
		{
			s_VertexArray->BindVertexBuffer(*vertexBuffer, binding);
			binding++;
		}
		s_VertexArray->BindIndexBuffer(*renderData.IBO);

		RendererCommands::DrawIndexed(renderData.IBO->GetCount());
	}

	void Renderer::RenderIndexed(const Camera& camera)
	{
		// Render in between Begin and End calls
		s_Renderer->BeginRender(camera);
		{
			// Render objects normally
			s_Renderer->RenderObjectsIndexed();

			// Render objects in single color with vertices expanded along their normal
			s_Renderer->RenderOutlinedObjectsIndexed();

			// Render Light objects in white color
			// Make sure Lights are rendered right after Object Outlining
			// So, We don't have to bind the same shader again
			s_Renderer->RenderLightObjectsIndexed();
		}
		s_Renderer->EndRender();

		// Render Framebuffer to the screen
		s_Renderer->RenderFramebuffer();
	}

	void Renderer::OnWindowResize(WindowResizeEvent& e)
	{
		s_Renderer->s_MainFramebuffer->Resize(e.GetWidth(), e.GetHeight());
		s_Renderer->s_QuadFramebuffer->Resize(e.GetWidth(), e.GetHeight());
	}
}