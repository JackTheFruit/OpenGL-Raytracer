#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
private:
	glm::vec3 cameraPos, cameraFront, cameraUp;
	float pitch, yaw, fov, lastX, lastY;
	const float sensitivity = 0.1f;
	bool firstMouse;
public:
	Camera();
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void processCameraInput(GLFWwindow* window, float deltaTime);
	void updateYaw(float xoffset);
	void updatePitch(float yoffset);
	void updateFOV(float yoffset);
	void updateCameraFront();
	glm::mat4 makeView();
	glm::mat4 makePerspective(int width, int height, float near, float far);
	glm::vec3 getCameraPos();
	glm::vec3 getCameraFront();
};

Camera::Camera() {
	firstMouse = 1;
	lastX = 400;
	lastY = 300;
	cameraPos = glm::vec3(3.0f, 3.0f, 10.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0.0f;
	yaw = -90.0f;
	fov = 45.0f;
}

glm::mat4 Camera::makeView() {
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::makePerspective(int width, int height, float near, float far) {
	return glm::perspective(glm::radians(fov), float(width) / float(height), near, far);
}

void Camera::updateYaw(float xoffset) {
	yaw += xoffset;
}

void Camera::updatePitch(float yoffset) {
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}

void Camera::updateFOV(float yoffset) {
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void Camera::updateCameraFront() {
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void Camera::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	updateYaw(xoffset);
	updatePitch(yoffset);
	updateCameraFront();
}

void Camera::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	updateFOV(yoffset);
}

void Camera::processCameraInput(GLFWwindow* window, float deltaTime) {
	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
}

glm::vec3 Camera::getCameraPos() {
	return cameraPos;
}

glm::vec3 Camera::getCameraFront() {
	return cameraFront;
}
#endif