#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

class Camera {
public:
	Camera() = default;
	~Camera() = default;

	Camera(GLFWwindow* window, glm::vec3 position = glm::vec3(0), float moveSpeed=1.0f);

	glm::mat4 getViewMatrix() const;
	glm::vec3 getPosition() const;

	void update(float deltaTime);

	void processMouseInput(float xOffset, float yOffset);

	double mouseSensitivity;
	float moveSpeed;

private:
	void calculateViewMatrix();

	void AscendDescent(float deltaTime);
	void movementInput();

	GLFWwindow* window;

	glm::vec3 position;
	glm::vec3 rotation;

	glm::vec3 moveInput;

	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
};