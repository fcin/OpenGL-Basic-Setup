#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"

int main()
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR: GLEW NOT INITIALIZED PROPERLY";
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[] = {
		-0.5, -0.5, 0.0f, 0.0f,
		 0.5, -0.5, 1.0f, 0.0f,
		 0.5,  0.5, 1.0f, 1.0f,
		-0.5,  0.5, 0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA));

	VertexBuffer vb(positions, 4 * 4 * sizeof(float));
	VertexArray va;
	VertexBufferLayout layout;
	layout.Push<float>(2);
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);
	IndexBuffer ib(indices, 6);

	Shader shader("res/shaders/Basic.shader");
	shader.Bind();
	shader.SetUniform4f("u_Color", 0.3f, 0.7f, 0.15f, 1.0f);

	Texture texture("res/textures/smileyFace.png");
	texture.Bind();

	shader.SetUniform1i("u_Texture", 0);

	va.Unbind();
	shader.Unbind();
	ib.Unbind();
	vb.Unbind();

	Renderer renderer;

	while (!glfwWindowShouldClose(window))
	{
		renderer.Clear();

		shader.Bind();
		shader.SetUniform4f("u_Color", 1.0f, 0.5f, 0.5f, 0.5f);

		renderer.Draw(va, ib, shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}