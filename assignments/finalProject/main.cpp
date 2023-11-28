#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

using namespace std;

struct Light {
	ew::Vec3 color = ew::Vec3(1.0, 1.0, 1.0);
};

struct Material {
	float ambientK = 0.2, diffuseK = 0.5, specular = 0.5, shininess = 2;
	ew::Vec3 lightColor = ew::Vec3(1.0f, 1.0f, 1.0f);
} material;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;
int numOfLights = 1;
const int MAX_LIGHTS = 4;

bool doLightsOrbit = true;

float prevTime, orbitRadius = 2.0f;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

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

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag"), unlitShader("assets/unlit.vert", "assets/unlit.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg", GL_REPEAT, GL_LINEAR);

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);

	resetCamera(camera, cameraController);

	Light lights[MAX_LIGHTS];
	ew::Transform lightTransform[MAX_LIGHTS];
	ew::Mesh shapeLightMesh = ew::Mesh(ew::createSphere(0.5f, 16));

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		//Draw shapes
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		for (int i = 0; i < numOfLights; i++) {
			shader.setVec3("_Lights[" + to_string(i) + "].position", lightTransform[i].position);
			shader.setVec3("_Lights[" + to_string(i) + "].color", lights[i].color);
		}

		shader.setVec3("_LightColor", material.lightColor);
		shader.setVec3("_CameraPos", camera.position);

		shader.setFloat("_Ambient", material.ambientK);
		shader.setFloat("_Diffuse", material.diffuseK);
		shader.setFloat("_Specular", material.specular);
		shader.setFloat("_Shininess", material.shininess);

		shader.setInt("_NumOfLights", numOfLights);

		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		for (int i = 0; i < numOfLights; i++) {
			if (doLightsOrbit) {
				lightTransform[i].position.x = (cos(i * (ew::TAU / 4) + time) * orbitRadius);
				lightTransform[i].position.y = lightTransform[i].position.y;
				lightTransform[i].position.z = (-sin(i * (ew::TAU / 4) + time) * orbitRadius);
			}

			lightTransform[i].scale = 0.35f;


			unlitShader.setMat4("_Model", lightTransform[i].getModelMatrix());
			unlitShader.setVec3("_Color", lights[i].color);
			shapeLightMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			ImGui::ColorEdit3("BG color", &bgColor.x);
			ImGui::DragInt("Light Count", &numOfLights, 1.0f, 0.0f, MAX_LIGHTS);

			ImGui::Checkbox("Orbitting Lights", &doLightsOrbit);
			ImGui::DragFloat("Orbit Radius", &orbitRadius, 0.1f, 0.5f);

			for (int i = 0; i < numOfLights; i++) {
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Lights")) {
					ImGui::DragFloat3("Position", &lightTransform[i].position.x, 0.1f);
					ImGui::ColorEdit3("Color", &lights[i].color.x, 0.1f);
				}
				ImGui::PopID();
			}

			if (ImGui::CollapsingHeader("Material")) {
				ImGui::ColorEdit3("Light Color", &material.lightColor.x);
				ImGui::DragFloat("Ambient", &material.ambientK, 0.05f, 0.0f, 1.0f);
				ImGui::DragFloat("Diffuse", &material.diffuseK, 0.05f, 0.0f, 1.0f);
				ImGui::DragFloat("Specular", &material.specular, 0.05f, 0.0f, 1.0f);
				ImGui::DragFloat("Shininess", &material.shininess, 0.1f, 2.0f, 100.0f);
			}

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
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}