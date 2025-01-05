#include "Camera.hpp"


Camera::Camera(GLFWwindow* window, glm::vec3 position)
	:
	window(window),
	mouseSensitivity(1.0),
	position(position),
	moveInput(glm::vec3(0.0f)),
	up(glm::vec3(0.0f)),
	right(glm::vec3(0.0f)),
	front(glm::vec3(0.0f))
{
	//to make camera look to positive Z
	rotation.y = 90.f;
}

glm::mat4 Camera::getViewMatrix() const
{
	glm::vec3 pos(position.x, position.y, position.z);
	glm::vec3 fr(front.x, front.y, front.z);
	glm::vec3 u(up.x, up.y, up.z);
	return glm::lookAt(pos, pos + fr, u);
}
void Camera::update(float deltaTime)
{
	movementInput();
	AscendDescent(deltaTime);

	position += (front * moveInput.z + right * moveInput.x) * deltaTime;

	calculateViewMatrix();
}
void Camera::processMouseInput(float xOffset, float yOffset)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
		return;

	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	rotation.y += xOffset;
	rotation.x += yOffset;

	if (rotation.x > 89.0f)
		rotation.x = 89.0f;
	if (rotation.x < -89.0f)
		rotation.x = -89.0f;

}

void Camera::AscendDescent(float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position.y += deltaTime;
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		position.y -= deltaTime;
}
void Camera::movementInput()
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		moveInput.z = 1;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		moveInput.z = -1;
	else moveInput.z = 0;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		moveInput.x = 1;
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		moveInput.x = -1;
	else moveInput.x = 0;
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
