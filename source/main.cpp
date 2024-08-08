#pragma region Headers
// OpenGL Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Texture Headers
#define STB_IMAGE_IMPLEMENTATION
#include "rendering/stb_image.h"

// Standard Library Headers
#include <iostream>
#include <filesystem>
#include <cmath>
#include <chrono>
#include <random>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;


// Custom Headers
#include "rendering/shader.hpp"
#include "chunk/perlinNoise.hpp"
#pragma endregion

// Typedefs
typedef float     f32;
typedef double    f64;
typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef size_t    usize;

const int a = 12;

// Macros
#define min(_a, _b) ({ __typeof__(_a) __a = (_a), __b = (_b); __a < __b ? __a : __b; })
#define max(_a, _b) ({ __typeof__(_a) __a = (_a), __b = (_b); __a > __b ? __a : __b; })

// Settings
const unsigned int DEFAULT_SCREEN_WIDTH = 1280;
const unsigned int DEFAULT_SCREEN_HEIGHT = 720;
unsigned int screenWidth = 1280;
unsigned int screenHeight = 720;

// Block ID's
#define Dirt            1
#define GrassBlock      2
#define Stone           3
#define Cobblestone     4
#define StoneBricks     5
#define StoneTiles      6
#define PolishedStone   7
#define Log             8
#define Planks          9
#define Sand            10
#define Sandstone       11
#define Bedrock         12
#define Bookshelf       13
#define Workbench       14
#define Bricks          15
#define Jukebox         16

// Camera Data
glm::vec3 cameraPos   = glm::vec3(3.0f);
glm::vec3 cameraFront = glm::vec3(-1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw   =  90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  90.0f;


// Timing Data
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool mouseClick = false;
int mouseClickType = 0;
u16 heldBlockType = 1;

u64 verticesLength = 240 * (16 * 16 * 16);
u64 indicesLength = 36 * (16 * 16 * 16);

// Structs and Classes
typedef struct {
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
} Buffers;

typedef struct{

	u64 pos[3];
	u16 blockTypes[16][16][16];
	u8 blockStates[16][16][16];
	u64 lastTick;
	Buffers storedBuffers;

} Chunk;

typedef struct {

	bool hit;
	glm::ivec3 rayPos;
	glm::ivec3 prevRayPos;

} RaycastData;

Chunk chunks[512];

// Voxel Generation Data
unsigned int voxelTris[] = {
	0,  1,  3,    //  1st triangle
	1,  2,  3,    //  2nd triangle
};
float triVertices[] = {
	1.0f,  1.0f,  0.0f,  // 0   top right     (front face)
	1.0f,  0.0f,  0.0f,  // 1   bottom right  (front face)
	0.0f,  0.0f,  0.0f,  // 2   bottom left   (front face)
	0.0f,  1.0f,  0.0f,  // 3   top left      (front face)
	1.0f,  1.0f,  1.0f,  // 4   top right     (back face)
	1.0f,  0.0f,  1.0f,  // 5   bottom right  (back face)
	0.0f,  0.0f,  1.0f,  // 6   bottom left   (back face)
	0.0f,  1.0f,  1.0f,  // 7   top left      (back face)
	1.0f,  1.0f,  1.0f,  // 8   top right     (top face)
	1.0f,  1.0f,  0.0f,  // 9   bottom right  (top face)
	0.0f,  1.0f,  0.0f,  // 10  bottom left   (top face)
	0.0f,  1.0f,  1.0f,  // 11  top left      (top face)
	1.0f,  0.0f,  1.0f,  // 12  top right     (bottom face)
	1.0f,  0.0f,  0.0f,  // 13  bottom right  (bottom face)
	0.0f,  0.0f,  0.0f,  // 14  bottom left   (bottom face)
	0.0f,  0.0f,  1.0f,  // 15  top left      (bottom face)
	0.0f,  1.0f,  0.0f,  // 16  top right     (left face)
	0.0f,  0.0f,  0.0f,  // 17  bottom right  (left face)
	0.0f,  0.0f,  1.0f,  // 18  bottom left   (left face)
	0.0f,  1.0f,  1.0f,  // 19  top left      (left face)
	1.0f,  1.0f,  1.0f,  // 20  top right     (right face)
	1.0f,  0.0f,  1.0f,  // 21  bottom right  (right face)
	1.0f,  0.0f,  0.0f,  // 22  bottom left   (right face)
	1.0f,  1.0f,  0.0f   // 23  top left      (right face)
};
float triTextureMaps[] = {
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};

// Function "Pre-Definitions"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
Buffers generateBuffersHand(float vertices[], int vertexArraySize);
Buffers generateBuffersChunk(float vertices[], int vertexArraySize,  unsigned int indices[], int indexArraySize);
bool CheckRender(int faceID, u16 blockTypes[16][16][16], int x, int y, int z);
void GenerateIndicesVerticesHand(float vertices[], unsigned int indices[], u16 blockType);
void GenerateIndicesVerticesChunk(float vertices[], unsigned int indices[], Chunk chunk);
double rand01() { return (double)rand() / RAND_MAX; }
void GenerateChunk(int chunkX, int chunkY, int chunkZ);
void RegenerateChunk(int chunkX, int chunkY, int chunkZ);

int main()
{

	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	GLFWwindow* window = glfwCreateWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "Minecraft: Reimagined - 1.0.0-indev", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "[Client - Main]  Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	std::cout << "[Client - Main]  Resolution: " << DEFAULT_SCREEN_WIDTH << " x " << DEFAULT_SCREEN_HEIGHT << std::endl;
	std::cout << "[Client - Main]  " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "[Client - Main]  OpenGL " << glGetString(GL_VERSION) << std::endl;  
	std::cout << "[Client - Main]  " << sizeof(Chunk) << " Bytes per Chunk." << std::endl;
	std::cout << "[Client - Main]  " << sizeof(Chunk) * 512 << " Bytes per Chunk Max." << std::endl;
	glViewport(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	GLFWimage images[1];
	images[0].pixels = stbi_load("resources/textures/icon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window, 1, images); 
	stbi_image_free(images[0].pixels);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("resources/textures/terrain.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "[Client - Main]  Failed to load terrain.png" << std::endl;
	}
	stbi_image_free(data);

	Shader blockShader = Shader("resources/shaders/vertexBlock.glsl", "resources/shaders/fragmentBlock.glsl");
	blockShader.use();
	blockShader.setInt("Texture", 0);

	Shader handBlockShader = Shader("resources/shaders/vertexHand.glsl", "resources/shaders/fragmentHand.glsl");
	handBlockShader.use();
	handBlockShader.setInt("Texture", 0);

	u32 seed = time(NULL);
	srand(seed);
	for (u32 chunkZ = 0; chunkZ < 8; chunkZ++) {
		for (u32 chunkY = 0; chunkY < 8; chunkY++) {
			for (u32 chunkX = 0; chunkX < 8; chunkX++) {
				GenerateChunk(chunkX, chunkY, chunkZ);
			}
		}
	}

	double currentFrame = static_cast<double>(glfwGetTime());
	double framesPerSecond = 60;
	lastFrame = currentFrame;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	while(!glfwWindowShouldClose(window))
	{

		currentFrame = static_cast<double>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		framesPerSecond = 1 / deltaTime;
		if (framesPerSecond <= 20.0) {
			std::cout << "[Client - Main] \033[1;31m Error! FPS dropped below 20 FPS. Current FPS: \033[0m" << framesPerSecond << std::endl;
		}

		// i n p u t    - Robot from that one movie.
		processInput(window);

		// Renderering Malarkey
		glClearColor(0.4f, 0.8f, 1.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		blockShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / (float)screenHeight, 0.1f, 200.0f);
		blockShader.setMat4("projection", projection);

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		blockShader.setMat4("view", view);

		for (int chunkID = 0; chunkID < 512; chunkID++) {
			
			blockShader.setVec4("chunkOffset", chunks[chunkID].pos[0], chunks[chunkID].pos[1], chunks[chunkID].pos[2], 0);
			
			glBindVertexArray(chunks[chunkID].storedBuffers.VAO);
			glDrawElements(GL_TRIANGLES, indicesLength, GL_UNSIGNED_INT, 0);
		}

		handBlockShader.use();
		float vertices[42] = {
			1.00f,  -1.00f,  0.0f,  1.0f, 1.0f, (float)heldBlockType, 0,  // 0   top right     (front face)   [Cube 0, 0, 0]
			1.00f,  -0.55f,  0.0f,  1.0f, 0.0f, (float)heldBlockType, 0,  // 1   bottom right  (front face)   [Cube 0, 0, 0]
			0.75f,  -1.00f,  0.0f,  0.0f, 1.0f, (float)heldBlockType, 0,  // 3   top left      (front face)   [Cube 0, 0, 0]
			1.00f,  -0.55f,  0.0f,  1.0f, 0.0f, (float)heldBlockType, 0,  // 1   bottom right  (front face)   [Cube 0, 0, 0]
			0.75f,  -0.55f,  0.0f,  0.0f, 0.0f, (float)heldBlockType, 0,  // 2   bottom left   (front face)   [Cube 0, 0, 0]
			0.75f,  -1.00f,  0.0f,  0.0f, 1.0f, (float)heldBlockType, 0   // 3   top left      (front face)   [Cube 0, 0, 0]
		};
		Buffers buffers;
		buffers = generateBuffersHand(vertices, 42 * sizeof(float));
		glBindVertexArray(buffers.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 42);

		glBindVertexArray(0);

		// fancy window majic
		glfwSwapBuffers(window);
		glfwPollEvents();    
	}
 
	glfwTerminate();
	return 0;
}

RaycastData getBlockRaycast(glm::vec3 camPos, glm::vec3 camFront) {

	RaycastData raycastData;
	glm::vec3 prevRayPos = camPos;
	glm::vec3 rayPos = camPos;
	int i = 0;
	bool hit = false;
	for (i = 0; i < 24; i += 1) {
		prevRayPos = rayPos;
		rayPos += camFront * 0.5f;
		if (rayPos.x > 0 && rayPos.y > 0 && rayPos.z > 0) {
			if (rayPos.x < 129 && rayPos.y > 0 && rayPos.z < 129) {
				u32 chunkX = (u32)(rayPos.x / 16);
				u32 chunkY = (u32)(rayPos.y / 16);
				u32 chunkZ = (u32)(rayPos.z / 16);
				u32 blockX = (u32)rayPos.x % 16;
				u32 blockY = (u32)rayPos.y % 16;
				u32 blockZ = (u32)rayPos.z % 16;
				if (chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockTypes[blockX][blockY][blockZ] != 0) {
					hit = true;
					raycastData.prevRayPos = prevRayPos;
					raycastData.rayPos = rayPos;
					raycastData.hit = hit;
					return raycastData;
				}
			}
		}
	} 
	raycastData.prevRayPos = prevRayPos;
	raycastData.rayPos = rayPos;
	raycastData.hit = false;
	return raycastData;
}

void breakBlock() {
	RaycastData raycast = getBlockRaycast(cameraPos, cameraFront);
		if (raycast.hit == false) {
			mouseClick = false;
			return;
		}
		u32 chunkX = (u32)(raycast.rayPos.x / 16);
		u32 chunkY = (u32)(raycast.rayPos.y / 16);
		u32 chunkZ = (u32)(raycast.rayPos.z / 16);
		chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockTypes[raycast.rayPos.x % 16][raycast.rayPos.y % 16][raycast.rayPos.z % 16] = 0;
		chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockStates[raycast.rayPos.x % 16][raycast.rayPos.y % 16][raycast.rayPos.z % 16] = 0;
		RegenerateChunk(chunkX, chunkY, chunkZ);
}
void placeBlock() {
	RaycastData raycast = getBlockRaycast(cameraPos, cameraFront);
		if (raycast.hit == false) {
			mouseClick = false;
			return;
		}
		u32 chunkX = (u32)(raycast.prevRayPos.x / 16);
		u32 chunkY = (u32)(raycast.prevRayPos.y / 16);
		u32 chunkZ = (u32)(raycast.prevRayPos.z / 16);
		chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockTypes[raycast.prevRayPos.x % 16][raycast.prevRayPos.y % 16][raycast.prevRayPos.z % 16] = heldBlockType;
		chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockStates[raycast.prevRayPos.x % 16][raycast.prevRayPos.y % 16][raycast.prevRayPos.z % 16] = heldBlockType;
		RegenerateChunk(chunkX, chunkY, chunkZ);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	std::cout << "[Client - Main]  Resolution: " << width << " x " << height << std::endl;
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}  

bool blockSwapCap = false;

void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				cameraPos += (cameraSpeed * cameraFront) * 25.0f;
			else
				cameraPos += cameraSpeed * cameraFront * 10.0f;
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			cameraPos += (cameraSpeed * cameraFront) * 2.5f;
		else
			cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos += cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos -= cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_M)) {
		glfwSetCursorPosCallback(window, NULL);  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
	if (glfwGetKey(window, GLFW_KEY_N)) {
		glfwSetCursorPosCallback(window, mouse_callback);  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
	if (mouseClick && mouseClickType == 0) {
		breakBlock();
	} if (mouseClick && mouseClickType == 1) {
		std::cout << "[Client - Main]  Scroll Wheel Clicked" << std::endl;
	} if (mouseClick && mouseClickType == 2) {
		placeBlock();
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		heldBlockType = Dirt;	
	} if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		heldBlockType = GrassBlock;
	} if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		heldBlockType = Stone;
	} if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		heldBlockType = StoneBricks;
	} if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		heldBlockType = StoneTiles;
	} if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
		heldBlockType = Cobblestone;
	} if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
		heldBlockType = PolishedStone;
	} if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
		heldBlockType = Log;
	} if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
		heldBlockType = Planks;
	}
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		if (blockSwapCap == false) {
			blockSwapCap = true;
			heldBlockType++;
		}
	} if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
		if (blockSwapCap == false) {
			blockSwapCap = true;
			heldBlockType--;
		}
	} if (!glfwGetKey(window, GLFW_KEY_MINUS) && !glfwGetKey(window, GLFW_KEY_EQUAL)) {
		blockSwapCap = false;
	}

	mouseClick = false;

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseClickType = 0;
		mouseClick = true;
	} if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		mouseClickType = 1;
		mouseClick = true;
	} if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		mouseClickType = 2;
		mouseClick = true;
	}
       
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

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

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset * 8.0f;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 90.0f)
		fov = 90.0f;
}

Buffers generateBuffersHand(float vertices[], int vertexArraySize) {

	Buffers buffers;
	glGenVertexArrays(1, &buffers.VAO);
	glGenBuffers(1, &buffers.VBO);

	glBindVertexArray(buffers.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexArraySize, vertices, GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float),  (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float),  (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// block type attribute
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float),  (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// block face attribute
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float),  (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);
	return buffers;

}

Buffers generateBuffersChunk(float vertices[], int vertexArraySize,  unsigned int indices[], int indexArraySize) {

	Buffers buffers;
	glGenVertexArrays(1, &buffers.VAO);
	glGenBuffers(1, &buffers.VBO);
	glGenBuffers(1, &buffers.EBO);

	glBindVertexArray(buffers.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexArraySize, vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArraySize, indices, GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),  (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),  (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// block type attribute
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float),  (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// block state attribute
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float),  (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);
	// block face attribute
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float),  (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(4);
	return buffers;

}

bool CheckRender(int faceID, u16 blockTypes[16][16][16], int x, int y, int z) {

	if (faceID == 0) {
		if ((z - 1) == -1)
			return true;
		if (blockTypes[x][y][z - 1] != 0)
			return false;
		else
			return true;
	}
	if (faceID == 1) {
		if ((z + 1) == 16)
			return true;
		if (blockTypes[x][y][z + 1] != 0)
			return false;
		else
			return true;
	}
	if (faceID == 2) {
		if ((y + 1) == 16)
			return true;
		if (blockTypes[x][y + 1][z] != 0)
			return false;
		else
			return true;
	}
	if (faceID == 3) {
		if ((y - 1) == -1)
			return true;
		if (blockTypes[x][y - 1][z] != 0)
			return false;
		else
			return true;
	}
	if (faceID == 4) {
		if ((x - 1) == -1)
			return true;
		if (blockTypes[x - 1][y][z] != 0)
			return false;
		else
			return true;
	}
	if (faceID == 5) {
		if ((x + 1) == 16)
			return true;
		if (blockTypes[x + 1][y][z] != 0)
			return false;
		else
			return true;
	}
	return false;
}


void GenerateIndicesVerticesChunk(float vertices[], unsigned int indices[], Chunk chunk) {

	// Generate Vertices
	u64 index = 0;
	u64 vertexIndex = 0;
	u64 textIndex = 0;
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				vertexIndex = 0;
				for (int faceCount = 0; faceCount < 6; faceCount++) {
					textIndex = 0;
					for (int v = 0; v < 4; v++) {
						vertices[index]       =   triVertices[vertexIndex] + x;
						vertices[index + 1]   =   triVertices[vertexIndex + 1] + y;
						vertices[index + 2]   =   triVertices[vertexIndex + 2] + z;
						vertices[index + 3]   =   triTextureMaps[textIndex];
						vertices[index + 4]   =   triTextureMaps[textIndex + 1];
						vertices[index + 5]   =   (float)chunk.blockTypes[x][y][z];
						vertices[index + 6]   =   (float)chunk.blockStates[x][y][z];
						vertices[index + 7]   =   faceCount;
						index        += 8;
						vertexIndex  += 3;
						textIndex    += 2;
					}
				}
			}
		}
	}
	// Generate Indices
	index = 0;
	u64 indexIndex = 0;
	for (u64 x = 0; x < 16; x++) {
		for (u64 y = 0; y < 16; y++) {
			for (u64 z = 0; z < 16; z++) {
				for (u64 faceID = 0; faceID < 6; faceID++) {
					for (u64 i = 0; i < 6; i++) {
						if (chunk.blockTypes[x][y][z] != 0 && CheckRender(faceID, chunk.blockTypes, x, y, z)) {
							indices[index] = voxelTris[i] + indexIndex;
						} else {
							indices[index] = 0;
						}
						index++;
					}
					indexIndex += 4;
				}
			}
		}
	}
}

void GenerateChunk(int chunkX, int chunkY, int chunkZ) {
	for (u32 x = 0; x < 16; x++) {
		for (u32 y = 0; y < 16; y++) {
			for (u32 z = 0; z < 16; z++) {
				if (chunkY == 0 && y == 0) { chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockTypes[x][y][z] = Bedrock;         }
				else if (chunkY == 0 && y == 1) { chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockTypes[x][y][z] = Dirt;       }
				else if (chunkY == 0 && y == 2) { chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockTypes[x][y][z] = Dirt;       }
				else if (chunkY == 0 && y == 3) { chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockTypes[x][y][z] = Dirt;       }
				else if (chunkY == 0 && y == 4) { chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockTypes[x][y][z] = GrassBlock; }
				else                       { chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockTypes[x][y][z] = 0;               }

				chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].blockStates[x][y][z] = z;
			}
		}
	}
	float vertices[verticesLength];
	unsigned int indices[indicesLength];
	chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].pos[0] = chunkX;
	chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].pos[1] = chunkY;
	chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].pos[2] = chunkZ;
	GenerateIndicesVerticesChunk(vertices, indices, chunks[(chunkZ * 64) + (chunkY * 8) + chunkX]);
	Buffers buffers;
	buffers = generateBuffersChunk(vertices, verticesLength * sizeof(float), indices, indicesLength * sizeof(unsigned int));
	chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].storedBuffers = buffers;
}

void RegenerateChunk(int chunkX, int chunkY, int chunkZ) {
	float vertices[verticesLength];
	unsigned int indices[indicesLength];
	chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].pos[0] = chunkX;
	chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].pos[1] = chunkY;
	chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].pos[2] = chunkZ;
	GenerateIndicesVerticesChunk(vertices, indices, chunks[(chunkZ * 64) + (chunkY * 8) + chunkX]);
	Buffers buffers;
	buffers = generateBuffersChunk(vertices, verticesLength * sizeof(float), indices, indicesLength * sizeof(unsigned int));
	chunks[(chunkZ * 64) + (chunkY * 8) + chunkX].storedBuffers = buffers;
}
