#pragma once

namespace Tunti
{
#pragma pack(push, 1)
	struct ShaderCameraContainer
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::mat4 InvView;
		glm::mat4 InvProjection;
		glm::vec3 Position;

		ShaderCameraContainer(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& position)
			: View(view), Projection(proj), InvView(glm::inverse(view)), InvProjection(glm::inverse(proj)), Position(position) {}
	};
#pragma pack(pop)

	class Camera
	{
	public:
		Camera(const glm::mat4& projection = glm::mat4(1.0f))
			: m_Projection(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
	protected:
		glm::mat4 m_Projection;
	};
}