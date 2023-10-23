#include <stdio.h>
#include <math.h>
#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <xr/camera.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void moveCamera(GLFWwindow* window, xr::Camera* camera, xr::CameraControls* controls, float deltaTime);
void resetCamera(bool willReset);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

bool willReset;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];
xr::CameraControls controls;
xr::Camera camera;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");

	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));
	
	camera.position = ew::Vec3(0.0, 0.0, 5.0);
	camera.target = ew::Vec3(0.0, 0.0, 0.0);
	camera.fov = 62;
	camera.aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
	camera.orthoSize = 6;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;


	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	float prevTime = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		moveCamera(window, &camera, &controls, deltaTime);

		//Set uniforms
		shader.use();

		
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		shader.setMat4("_View", camera.ViewMatrix());
		shader.setMat4("_Clip", camera.ProjectionMatrix());

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			/*ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}*/
			ImGui::Text("Camera");
			ImGui::DragFloat3("Position", &camera.position.x, 0.05f);
			ImGui::DragFloat3("Target", &camera.target.x, 0.05f);
			ImGui::Checkbox("Orthographic", &camera.orthographic);
			if (camera.orthographic) {
				ImGui::SliderFloat("Ortho Height", &camera.orthoSize, 0, 100);
			}
			else if (!camera.orthographic) {
				ImGui::DragFloat("FOV", &camera.fov, 0.05f);
			}
			ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f);
			ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1);

			ImGui::Text("Camera Controller");

			ImGui::Text("Yaw: %f", controls.yaw);
			ImGui::Text("Pitch: %f", controls.pitch);
			ImGui::DragFloat("Move Speed", &controls.moveSpeed, 1);
			ImGui::Checkbox("Reset", &willReset);
			resetCamera(willReset);


			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void moveCamera(GLFWwindow* window, xr::Camera* camera, xr::CameraControls* controls, float deltaTime) {
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		controls->firstMouse = true;
		return;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (controls->firstMouse) {
		controls->firstMouse = false;
		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;
	}

	double xOffset = mouseX - controls->prevMouseX;
	double yOffset = controls->prevMouseY - mouseY;

	controls->yaw += xOffset * controls->mouseSensitivity;
	controls->pitch += yOffset * controls->mouseSensitivity;

	if (controls->pitch > 89.0f) {
		controls->pitch = 89.0f;

	}
	else if (controls->pitch < -89.0f) {
		controls->pitch = -89.0f;
	}

	controls->prevMouseX = mouseX;
	controls->prevMouseY = mouseY;

	ew::Vec3 forward;
	forward.x = cos(ew::Radians(controls->yaw)) * cos(ew::Radians(controls->pitch));
	forward.y = sin(ew::Radians(controls->pitch));
	forward.z = sin(ew::Radians(controls->yaw)) * cos(ew::Radians(controls->pitch));

	camera->target = camera->position + forward;

	ew::Vec3 worldUp = { 0.0, 1.0, 0.0 };
	ew::Vec3 right = ew::Normalize(ew::Cross(forward, worldUp));
	ew::Vec3 up = ew::Normalize(ew::Cross(right, forward));

	if (glfwGetKey(window, GLFW_KEY_W)) {
		camera->position += forward * controls->moveSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_S)) {
		camera->position -= forward * controls->moveSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_D)) {
		camera->position += right * controls->moveSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_A)) {
		camera->position -= right * controls->moveSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_E)) {
		camera->position += up * controls->moveSpeed * deltaTime;
	} if (glfwGetKey(window, GLFW_KEY_Q)) {
		camera->position -= up * controls->moveSpeed * deltaTime;
	}

	camera->target = camera->position + forward;
}

void resetCamera(bool willReset) {
	if (willReset) {
		camera.position = ew::Vec3(0.0, 0.0, 5.0);
		camera.target = ew::Vec3(0.0, 0.0, 0.0);
		camera.fov = 62;
		camera.aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
		camera.orthoSize = 6;
		camera.nearPlane = 0.1f;
		camera.farPlane = 100.0f;
		camera.orthographic = false;
		controls = xr::CameraControls();
		controls.yaw = -90;
		controls.pitch = 0;
	}
}