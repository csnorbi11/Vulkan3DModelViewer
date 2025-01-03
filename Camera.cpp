#include "Camera.hpp"


Camera::Camera(GLFWwindow* window)
	:
	window(window)
{
}

glm::mat4 Camera::getViewMatrix() const
{
	glm::vec3 pos(position.x, position.y, position.z);
	glm::vec3 fr(front.x, front.y, front.z);
	glm::vec3 u(up.x, up.y, up.z);
	return glm::lookAt(pos, pos + fr, u);
}

void Camera::update(double deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position.x += deltaTime;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position.x -= deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position.z += deltaTime;
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position.z -= deltaTime;

	calculateViewMatrix();
}

void Camera::calculateViewMatrix()
{
	glm::vec3 dir;
	dir.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	dir.y = sin(glm::radians(rotation.x));
	dir.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));


	front = glm::normalize(dir);
	right = glm::normalize(glm::cross(front, glm::vec3(0.0, 1.0, 0.0)));
	up = glm::normalize(glm::cross(right, front));
}
