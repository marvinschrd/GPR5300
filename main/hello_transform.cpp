#include <SDL_main.h>
#include <glad/glad.h>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "engine.h"

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
		unsigned int texture_diffuse_;
		unsigned int texture_smily_;
		float time_ = 0.0f;
		glm::vec3 position_ = glm::vec3(0, 0, 2);
		glm::vec3 delta_position_ = glm::vec3(0, 0, 0);
		glm::mat4 model_ = glm::mat4(1.0f);
		glm::mat4 view_ = glm::mat4(1.0f);
		glm::mat4 projection_ = glm::mat4(1.0f);
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
			-0.5f, -0.5f, 0.0f, 1.0, 0.0, 0.0, 0.0, 0.0,
			 0.5f, -0.5f, 0.0f, 0.0, 1.0, 0.0, 1.0, 0.0,
			-0.5f,  0.5f, 0.0f, 0.0, 0.0, 1.0, 0.0, 1.0,
			 0.5f,  0.5f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0
		};

		std::array<std::uint32_t, 6> indices{
			0, 1, 2,
			1, 2, 3
		};

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
		{
			std::string jpg_file = path + "data/textures/texture_diffuse.jpg";
			int width, height, nrChannels;
			// stbi_set_flip_vertically_on_load(true);
			unsigned char* dataDiffuse = stbi_load(
				jpg_file.c_str(), 
				&width, 
				&height, 
				&nrChannels, 
				0);
			assert(dataDiffuse);

			glGenTextures(1, &texture_diffuse_);
			IsError(__FILE__, __LINE__);
			glBindTexture(GL_TEXTURE_2D, texture_diffuse_);
			IsError(__FILE__, __LINE__);
			glTexImage2D(
				GL_TEXTURE_2D, 
				0, 
				GL_RGB, 
				width, 
				height, 
				0, 
				GL_RGB, 
				GL_UNSIGNED_BYTE, 
				dataDiffuse);
			IsError(__FILE__, __LINE__);
			glTexParameteri(
				GL_TEXTURE_2D, 
				GL_TEXTURE_WRAP_S, 
				GL_MIRRORED_REPEAT);
			IsError(__FILE__, __LINE__);
			glTexParameteri(
				GL_TEXTURE_2D, 
				GL_TEXTURE_WRAP_T, 
				GL_MIRRORED_REPEAT);
			IsError(__FILE__, __LINE__);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			IsError(__FILE__, __LINE__);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			IsError(__FILE__, __LINE__);
			glGenerateMipmap(GL_TEXTURE_2D);
			IsError(__FILE__, __LINE__);
		}

		std::ifstream ifs_vert(
			path + "data/shaders/hello_transform/transform.vert");
		std::ifstream ifs_frag(
			path + "data/shaders/hello_transform/transform.frag");

		if (!ifs_vert.is_open())
		{
			throw std::runtime_error("Could not open vertex file.");
		}
		if (!ifs_frag.is_open())
		{
			throw std::runtime_error("Could not open fragment file.");
		}

		std::string vertex_source{ 
			std::istreambuf_iterator<char>(ifs_vert), 
			{} };
		std::string fragment_source{ 
			std::istreambuf_iterator<char>(ifs_frag), 
			{} };

		// Vertex shader.
		vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
		IsError(__FILE__, __LINE__);
		const char* ptr1 = vertex_source.c_str();
		glShaderSource(vertex_shader_, 1, &ptr1, 0);
		IsError(__FILE__, __LINE__);
		glCompileShader(vertex_shader_);
		GLint status = 0;
		glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint infoLogLength;
			glGetShaderiv(vertex_shader_, GL_INFO_LOG_LENGTH, &infoLogLength);
			GLchar* infoLog = new GLchar[infoLogLength];
			glGetShaderInfoLog(vertex_shader_, infoLogLength, NULL, infoLog);
			std::stringstream iss{};
			iss << "VS> could not compile: " << infoLog;
			throw std::runtime_error(iss.str());
		}

		// Fragment shader.
		fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
		IsError(__FILE__, __LINE__);
		const char* ptr2 = fragment_source.c_str();
		glShaderSource(fragment_shader_, 1, &ptr2, 0);
		IsError(__FILE__, __LINE__);
		glCompileShader(fragment_shader_);
		glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint infoLogLength;
			glGetShaderiv(vertex_shader_, GL_INFO_LOG_LENGTH, &infoLogLength);
			GLchar* infoLog = new GLchar[infoLogLength];
			glGetShaderInfoLog(vertex_shader_, infoLogLength, NULL, infoLog);
			std::stringstream iss{};
			iss << "FS> could not compile: " << infoLog;
			throw std::runtime_error(iss.str());
		}

		// Program.
		program_ = glCreateProgram();
		IsError(__FILE__, __LINE__);
		glAttachShader(program_, vertex_shader_);
		IsError(__FILE__, __LINE__);
		glAttachShader(program_, fragment_shader_);
		IsError(__FILE__, __LINE__);
		//glBindAttribLocation(program_, 0, "aPos");
		glLinkProgram(program_);
		IsError(__FILE__, __LINE__);
		assert(program_ != 0);

		// Bind uniform to program.
		glUseProgram(program_);
		IsError(__FILE__, __LINE__);
		glActiveTexture(GL_TEXTURE0);
		IsError(__FILE__, __LINE__);
		glBindTexture(GL_TEXTURE_2D, texture_diffuse_);
		IsError(__FILE__, __LINE__);
		int location_texture_diffuse = glGetUniformLocation(
			program_, 
			"textureDiffuse");
		IsError(__FILE__, __LINE__);
		glUniform1i(location_texture_diffuse, 0);
		IsError(__FILE__, __LINE__);
	
		glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
		IsError(__FILE__, __LINE__);
	}

	void HelloTransform::SetModelMatrix(seconds dt) 
	{
		model_ = glm::rotate(glm::mat4(1.0f), time_, glm::vec3(0.f, 1.f, 0.f));
	}

	void HelloTransform::SetViewMatrix(seconds dt) 
	{
		position_ += delta_position_;
		delta_position_ = glm::vec3(0, 0, 0);
		view_ = glm::lookAt(
			position_, 				// Eye position.
			glm::vec3(0, 0, 0),  	// Where it's looking.
			glm::vec3(0, 1, 0)); 	// Up vector.
	}

	void HelloTransform::SetProjectionMatrix()
	{
		projection_ = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);
	}

	void HelloTransform::SetUniformMatrix() const
	{
		glUseProgram(program_);
		IsError(__FILE__, __LINE__);
		int location_model = glGetUniformLocation(program_, "model");
		IsError(__FILE__, __LINE__);
		glUniformMatrix4fv(
			location_model,
			1,
			GL_FALSE,
			&model_[0][0]);
		IsError(__FILE__, __LINE__);
		int location_view = glGetUniformLocation(program_, "view");
		IsError(__FILE__, __LINE__);
		glUniformMatrix4fv(
			location_view,
			1,
			GL_FALSE,
			&view_[0][0]);
		IsError(__FILE__, __LINE__);
		int location_projection = glGetUniformLocation(program_, "projection");
		IsError(__FILE__, __LINE__);
		glUniformMatrix4fv(
			location_projection,
			1,
			GL_FALSE,
			&projection_[0][0]);
		IsError(__FILE__, __LINE__);
	}

	void HelloTransform::Update(seconds dt)
	{
		time_ += dt.count();
		SetViewMatrix(dt);
		SetModelMatrix(dt);
		SetProjectionMatrix();
		SetUniformMatrix();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		IsError(__FILE__, __LINE__);
		glUseProgram(program_);
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
		glDeleteProgram(program_);
		IsError(__FILE__, __LINE__);
		glDeleteShader(vertex_shader_);
		IsError(__FILE__, __LINE__);
		glDeleteShader(fragment_shader_);
		IsError(__FILE__, __LINE__);
	}

	void HelloTransform::OnEvent(SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
				exit(0);
			if (event.key.keysym.sym == SDLK_w)
			{
				delta_position_.z -= 0.1;
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				delta_position_.z += 0.1;
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				delta_position_.x -= 0.1;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				delta_position_.x += 0.1;
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
