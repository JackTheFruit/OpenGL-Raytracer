#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <shader.h>
#include <camera.h>
#include <model.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <filesystem>

glm::vec3 lightPos(1.2f, -1.0f, 4.0f);

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int windowWidth = 800, windowHeight = 600;

Camera camera;

//void calcNormals() {
//	float normals[36];
//	for (int i = 0; i < 12; i++) {
//		int vert_A_idx = indices[3 * i];
//		int vert_B_idx = indices[3 * i + 1];
//		int vert_C_idx = indices[3 * i + 2];
//		glm::vec3 vecA = glm::vec3(vertices[3 * vert_A_idx], vertices[3 * vert_A_idx + 1], vertices[3 * vert_A_idx + 2]);
//		glm::vec3 vecB = glm::vec3(vertices[3 * vert_B_idx], vertices[3 * vert_B_idx + 1], vertices[3 * vert_B_idx + 2]);
//		glm::vec3 vecC = glm::vec3(vertices[3 * vert_C_idx], vertices[3 * vert_C_idx + 1], vertices[3 * vert_C_idx + 2]);
//		glm::vec3 normal = glm::normalize(glm::cross((vecB - vecA), (vecC - vecB)));
//		normals[3 * i] = normal.x;
//		normals[3 * i + 1] = normal.y;
//		normals[3 * i + 2] = normal.z;
//	}
//}

void processInput(GLFWwindow* window)
{
	camera.processCameraInput(window, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.mouseCallback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.scrollCallback(window, xoffset, yoffset);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Raytracer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		printf("Failed to initialize OpenGL context\n");
		return -1;
	}

	printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	glViewport(0, 0, windowWidth, windowHeight);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	Shader ourShader("./shaders/shader.vert", "./shaders/shader.frag");
	
	std::filesystem::path model_path = std::filesystem::current_path() / "models" / "backpack" / "backpack.obj";
	Model ourModel(model_path.string());

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera.makeView();
		glm::mat4 perspective = camera.makePerspective(windowWidth, windowHeight, 0.1f, 100.0f);

		ourShader.use();

		ourShader.setVec3("viewPos", camera.getCameraPos());

		// view/projection transformations
		ourShader.setMat4("projection", perspective);
		ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}