#include <SDL_main.h>
#include <glad/glad.h>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"
#include "camera.h"
#include "texture.h"
#include "shader.h"

namespace gl {

	class HelloTransform : public Program
	{
	public:
		void Init() override;
		void Update(seconds dt) override;
		void Destroy() override;
		void OnEvent(SDL_Event& event) override;
		void DrawImGui() override;

	protected:
		void SetModelMatrix(seconds dt);
		void SetViewMatrix(seconds dt);
		void SetProjectionMatrix();
		void IsError(const std::string& file, int line) const;
		void SetUniformMatrix() const;

	protected:
		unsigned int VAO_;
		unsigned int VBO_;
		unsigned int EBO_;
		unsigned int vertex_shader_;
		unsigned int fragment_shader_;
		unsigned int program_;

		float time_ = 0.0f;
		float delta_time_ = 0.0f;

		std::unique_ptr<Camera> camera_ = nullptr;
		std::unique_ptr<Texture> texture_diffuse_ = nullptr;
		std::unique_ptr<Shader> shaders_ = nullptr;

		glm::mat4 model_ = glm::mat4(1.0f);
		glm::mat4 view_ = glm::mat4(1.0f);
		glm::mat4 projection_ = glm::mat4(1.0f);
		glm::mat4 model_inverse_ = glm::mat4(1.0f);
	};

	void HelloTransform::IsError(const std::string& file, int line) const
	{
		auto error_code = glGetError();
		if (error_code != GL_NO_ERROR)
		{
			std::cerr
				<< error_code
				<< " in file: " << file
				<< " at line: " << line
				<< "\n";
		}
	}

	void HelloTransform::Init()
	{
		std::array<float, 32> vertices = {
			-0.5f, -0.5f, 0.0f, 1.0, 0.0, 1.0, 0.0, 0.0,
			 0.5f, -0.5f, 0.0f, 0.0, 1.0, 1.0, 1.0, 0.0,
			-0.5f,  0.5f, 0.0f, 0.0, 0.0, 1.0, 0.0, 1.0,
			 0.5f,  0.5f, 0.0f, 1.0, 0.0, 1.0, 1.0, 1.0
		};

		std::array<std::uint32_t, 6> indices{
			0, 1, 2,
			1, 2, 3
		};

		camera_ = std::make_unique<Camera>(glm::vec3(.0f, .0f, 2.0f));

		// VAO binding should be before VAO.
		glGenVertexArrays(1, &VAO_);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(VAO_);
		IsError(__FILE__, __LINE__);

		// EBO.
		glGenBuffers(1, &EBO_);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
		IsError(__FILE__, __LINE__);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(float),
			indices.data(),
			GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		IsError(__FILE__, __LINE__);

		// VBO.
		glGenBuffers(1, &VBO_);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_);
		IsError(__FILE__, __LINE__);
		glBufferData(
			GL_ARRAY_BUFFER,
			vertices.size() * sizeof(float),
			vertices.data(),
			GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);

		GLintptr vertex_color_offset = 3 * sizeof(float);
		GLintptr vertex_tex_offset = 6 * sizeof(float);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(float),
			0);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(float),
			(GLvoid*)vertex_color_offset);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(float),
			(GLvoid*)vertex_tex_offset);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(0);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(1);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(2);
		IsError(__FILE__, __LINE__);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		IsError(__FILE__, __LINE__);

		std::string path = "../";

		texture_diffuse_ = std::make_unique<Texture>(
			path + "data/textures/texture_diffuse.jpg");

		shaders_ = std::make_unique<Shader>(
			path + "data/shaders/hello_light/lightShader.vert",
			path + "data/shaders/hello_light/lightShader.frag");

		// Bind uniform to program.
		shaders_->Use();
		texture_diffuse_->Bind(0);
		shaders_->SetInt("textureDiffuse", 0);

		glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
		IsError(__FILE__, __LINE__);
	}

	void HelloTransform::SetModelMatrix(seconds dt)
	{
		model_ = glm::rotate(glm::mat4(1.0f), time_, glm::vec3(0.f, 1.f, 0.f));
		model_inverse_ = glm::transpose(glm::inverse(model_));
	}

	void HelloTransform::SetViewMatrix(seconds dt)
	{
		view_ = camera_->GetViewMatrix();
	}

	void HelloTransform::SetProjectionMatrix()
	{
		projection_ = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);
	}

	void HelloTransform::SetUniformMatrix() const
	{
		shaders_->Use();
		shaders_->SetMat4("model", model_);
		shaders_->SetMat4("view", view_);
		shaders_->SetMat4("projection", projection_);
		shaders_->SetMat4("model_inverse", model_inverse_);
		shaders_->SetVec3("camera_pos", camera_->position);
	}

	void HelloTransform::Update(seconds dt)
	{
		delta_time_ = dt.count();
		time_ += delta_time_;
		SetViewMatrix(dt);
		SetModelMatrix(dt);
		SetProjectionMatrix();
		SetUniformMatrix();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(VAO_);
		IsError(__FILE__, __LINE__);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		IsError(__FILE__, __LINE__);
	}

	void HelloTransform::Destroy()
	{

	}

	void HelloTransform::OnEvent(SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
				exit(0);
			if (event.key.keysym.sym == SDLK_w)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::FORWARD, delta_time_);
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::BACKWARD, delta_time_);
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::LEFT, delta_time_);
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::RIGHT, delta_time_);
			}
		}
	}

	void HelloTransform::DrawImGui()
	{
	}

} // End namespace gl.

int main(int argc, char** argv)
{
	gl::HelloTransform program;
	gl::Engine engine(program);
	engine.Run();
	return EXIT_SUCCESS;
}
