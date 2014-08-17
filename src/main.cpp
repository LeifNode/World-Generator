#include "glStd.h"
#include "glApp.h"

GLApp* pApplication;

void errorCallback(int error, const char* description)
{
	std::cout << "Error " << error << " occured" << std::endl << description << std::endl;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
	pApplication->OnResize(width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	pApplication->OnKey(key, action);
}

void mouseMoveCallback(GLFWwindow* window, double x, double y)
{
	pApplication->OnMouseMove((float)x, (float)y);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		pApplication->OnMouseDown(button);
	else if (action == GLFW_RELEASE)
		pApplication->OnMouseUp(button);
}

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
		std::cout << argv[i] << std::endl;


    GLFWwindow* window;

	glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        return -1;
	
	glfwWindowHint(GLFW_SAMPLES, 16);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1280, 720, "Loading...", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

	/* Make the window's context current */
    glfwMakeContextCurrent(window);

	GLenum glewInitResult = glewInit();
	if (glewInitResult != GLEW_OK)
	{
		std::string trash;
		std::cout << "Failed to initialize GLEW." << std::endl << glewGetErrorString(glewInitResult) << std::endl;
		std::getline(std::cin, trash);
		return -1;
	}

	pApplication = new GLApp(window);

	glfwSetFramebufferSizeCallback(window, resizeCallback);

	glfwSetKeyCallback(window, keyCallback);

	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);

	//Uncap the framerate
	glfwSwapInterval(0);

	pApplication->Initialize();
	pApplication->InitializeModels(argc, argv);

	//Main loop
    while (!glfwWindowShouldClose(window))
    {
		pApplication->Update();

		pApplication->PreRender();
		pApplication->Render();
		pApplication->PostRender();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

	delete pApplication;
	pApplication = nullptr;

    glfwTerminate();

	std::string trash;

	//std::cout << "\n\npress enter to exit..." << std::endl;
	//std::getline(std::cin, trash);

    return 0;
}