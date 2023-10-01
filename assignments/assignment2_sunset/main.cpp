#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <xr/shader.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

struct Vertex {
	float x, y, z;
	float u, v;
};

Vertex vertices[4] = {
	//    x      y      z      u     v
	   {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f }, // Bottom left
	   { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f }, // Bottom right
	   { 1.0f,  1.0f, 0.0f, 1.0f, 1.0f }, // Top right
	   {-1.0f,  1.0f, 0.0f, 0.0f, 1.0f }  // Top left
};

unsigned int indices[6]{
	0, 1, 2, // TRIANGLE 1
	0, 2, 3  // TRIANGLE 2
};

bool showImGUIDemoWindow = true;

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
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

	xr::ShaderLibrary shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shader.use();

	unsigned int vao = createVAO(vertices, 4, indices, 6);
	glBindVertexArray(vao);

	float moonSpeed = 0.25;
	float moonRadius = 0.45;
	float glowRadius = 0.7;
	float topDayLight[3] = { 0.3, 0.1, 1.0 };
	float bottomDayLight[3] = { 0.0, 0.0, 0.0 };
	float topNightLight[3] = { 0.7, 0.3, 0.03 };
	float bottomNightLight[3] = { 0.4, 0.0, 0.6 };
	float moonColor[3] = { 1.0, 1.0, 1.0 };
	float hillColor[3] = { 0.1, 0.2, 0.2 };

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.setFloat("iSunSpeed", moonSpeed);
		shader.setFloat("iSunRadius", moonRadius);
		shader.setFloat("iGlowRadius", glowRadius);
		shader.setFloat("iTime", glfwGetTime());
		shader.setVec2("iResolution", SCREEN_WIDTH, SCREEN_HEIGHT);
		shader.setVec3("iTopDayLightCycle", topDayLight[0], topDayLight[1], topDayLight[2]);
		shader.setVec3("iBottomDayLightCycle", bottomDayLight[0], bottomDayLight[1], bottomDayLight[2]);
		shader.setVec3("iTopNightLightCycle", topNightLight[0], topNightLight[1], topNightLight[2]);
		shader.setVec3("iBottomNightLightCycle", bottomNightLight[0], bottomNightLight[1], bottomNightLight[2]);
		shader.setVec3("iSunColor", moonColor[0], moonColor[1], moonColor[2]);
		shader.setVec3("iHillColor", hillColor[0], hillColor[1], hillColor[2]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			ImGui::SliderFloat("Sun Speed", &moonSpeed, 0.0f, 2.0f);
			ImGui::SliderFloat("Sun Radius", &moonRadius, 0.0f, 2.0f);
			ImGui::SliderFloat("Glow Radius", &glowRadius, 0.0f, 2.0f);
			ImGui::ColorEdit3("Top Sky Daylight Gradient", topDayLight);
			ImGui::ColorEdit3("Bottom Sky Daylight Gradient", bottomDayLight);
			ImGui::ColorEdit3("Top Sky Night Gradient", topNightLight);
			ImGui::ColorEdit3("Bottom Night Gradient", bottomNightLight);
			ImGui::ColorEdit3("Sun Color", moonColor);
			ImGui::ColorEdit3("Hill Color", hillColor);

			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertexData, GL_STATIC_DRAW);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(0);

	//UV
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, u));
	glEnableVertexAttribArray(1);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
