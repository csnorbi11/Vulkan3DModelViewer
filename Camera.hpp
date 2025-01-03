#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
	Camera()=default;
	~Camera() = default;

	Camera(GLFWwindow* window);

	glm::mat4 getViewMatrix() const;
	void update(double deltaTime);

	

private:
	void calculateViewMatrix();

	GLFWwindow* window;

	glm::vec3 position;
	glm::vec3 rotation;

	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
};