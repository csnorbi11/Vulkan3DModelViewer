#include "GlfwHandler.hpp"


class App {
public:
	App();
	~App();

	void run();

private:
	void loop();

	GlfwHandler glfwHandler{};


};