/*
待做功能：
目前做到了遍历model的各个mesh及mesh中各个顶点。（√）
1.获取了每个mesh和顶点后，数据流如何放入shader	（√）
2.使用结构体组合数据后，是否直接将数据接口（即vertex类型变量）放入属性点即可，还是别的方法（√）
（设置属性点读取起点及步长）
3.颜色暂时以白模的形式展现，具体的材质如何复原至原有的mesh上，材质的texcoord是否需要自定义(√）
4.模型的动作如何完成，是否需要先在DCC上完成后再尝试导入模型
5.完成窗口的操作设置（√）
6.添加光照计算，(√）
7.添加星云，使用体渲染
8.添加天空盒（√）
9.添加地球，地球的纹理变换：在阴影处和光照处纹理应用应不同
10.操作PLUS；如何通过点击模型，控制模型的移动？最好有坐标轴，通过控制坐标轴来移动
11.阴影（√）
12.To be continuing,,,

代码架构：
1.不同的program之间的作用关系是什么，是否切换program后，前一个program的显示还能继续？
2.现在的代码结构如何修改，现在就是一团杂物，啥都有
3.如果要添加功能，项目的架构如何设计
*/
/*服了，用了下ai给我搞成屎山了，国庆开始重构代码，现有整体已有基本功能*/

#include<iostream>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<glad.h>
#include<GLFW/glfw3.h>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//#include"myOpenGLErrorCheck.h"

//int width, height;
bool isFirst = true;
int lastX, lastY;
float yaw, pitch;

void framebufferSize_callback(GLFWwindow* window, int width, int height);
//key_callback函数已弃用，无法满足长按键的实现
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


struct Camera
{
	glm::vec3 worPosition;
	glm::vec3 center;
	glm::vec3 up;
}camera;

class LuoTY
{
public:
	LuoTY();
	~LuoTY();

	void Init();
	void render();
	void shutdown();
	void loadPMX(const std::string& filePath);
	int getSubMeshSize();
	int getTotalIndeicesSize();
	GLuint loadTexFromFile(const std::string& path);
	GLuint loadCubemap(std::vector<std::string> faces);
	void initshadow();
	void resizeShadowMap(int width, int height);

private:
	GLFWwindow* window;
	std::string filePath;
	Assimp::Importer importer;
	const aiScene* scene;
	//LuoTY
	GLuint program;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	struct Uniforms
	{
		glm::vec3 wlightPos;
		glm::vec3 wviewPos;
	}uniforms;
	struct Vertex
	{
		glm::vec4 position;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 uv;
	};
	struct Mesh
	{
		std::vector<Vertex> vertices;
		GLuint index_diffuse;
		size_t  indexCount = 0;
		size_t  indexOffset = 0;
	};

	std::vector<Mesh> subMesh;
	std::vector<unsigned int> indices;
	std::vector<GLuint> tex_diffuse;

	//plane
	GLuint programPlane;
	GLuint planeVAO, planeVBO, planeEBO;
	std::vector<float> planeVertices;
	std::vector<unsigned int> planeIndices;

	//skybox
	GLuint skyboxProgram;
	GLuint skyboxVAO, skyboxVBO;
	GLuint cubemapTexture;

	//shadow
	GLuint shadowProgram;
	GLuint shadowFBO;
	GLuint shadowDepthTex;
	GLuint SHADOW_W, SHADOW_H;
	glm::mat4 lightSpaceMatrix;
};

LuoTY::LuoTY()
{
	window = nullptr;
	filePath = "";
}

LuoTY::~LuoTY()
{
	shutdown();
}

void LuoTY::Init()
{
	//check glfw init
	if (!glfwInit())
	{
		std::cout << "glfw init failed" << std::endl;
		glfwTerminate();
	}
	//set version of OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1600, 1000, "LuoTY", nullptr, nullptr);
	//check window create
	if (!window)
	{
		std::cout << "window creates failed" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	//设置回调函数
	glfwSetFramebufferSizeCallback(window, framebufferSize_callback);
	//glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//check glad init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "glad init failed" << std::endl;
		glfwTerminate();
	}
	glfwSetWindowUserPointer(window, this);
	//load PMX file
	filePath = "D:/Models/LTY/luotianyi_v4_chibi_ver3.0.pmx";
	loadPMX(filePath);
	//shader
	//LuoTY
	const char* vs =
		R"(
		#version 450
		layout(location = 0) in vec4 position;
		layout(location = 1) in vec3 normal;
		layout(location = 2) in vec3 color;
		layout(location = 3) in vec2 uv;

		out VS_OUT
		{
			vec2 uv;
			vec3 wnormal;
			vec3 wlightDir;
			vec3 wviewDir;
			vec4 fragPosLightSpace;
		} vs_out;

		uniform mat4 M;
		uniform mat4 MVP;
		uniform mat4 lightSpaceMatrix;
		uniform vec3 wlightPos;
		uniform vec3 wviewPos;

		void main()
		{
			vs_out.uv = uv;
			gl_Position = MVP * position;

			vec3 worPos = vec3(M * position);
			mat3 NM = transpose(inverse(mat3(M)));
			vs_out.wnormal     = normalize(NM * normal);
			vs_out.wlightDir   = normalize(wlightPos - worPos);
			vs_out.wviewDir    = normalize(wviewPos - worPos);
			vs_out.fragPosLightSpace = lightSpaceMatrix * M * position;
		}
	)";

	const char* fs =
		R"(
		#version 450
		in VS_OUT
		{
			vec2 uv;
			vec3 wnormal;
			vec3 wlightDir;
			vec3 wviewDir;
			vec4 fragPosLightSpace;
		} fs_in;

		out vec4 out_color;

		uniform sampler2D diffuse_map;
		uniform sampler2D shadowMap;
		uniform vec3 wlightPos;

		float ShadowCalculation(vec4 fragPosLightSpace)
		{
			// 透视除法 -> [0,1]
			vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
			projCoords = projCoords * 0.5 + 0.5;

			if (projCoords.z > 1.0) return 0.0;

			float closestDepth = texture(shadowMap, projCoords.xy).r;
			float currentDepth = projCoords.z;
			float bias = 0.005;                          // 可调

			return (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
		}

		void main()
		{
			vec3 lightCol = vec3(0.7);
			vec3 col = texture(diffuse_map, fs_in.uv).rgb;

			float NdotL = max(dot(fs_in.wnormal, fs_in.wlightDir), 0.0);
			vec3 diffuse = NdotL * col;

			vec3 halfway = normalize(fs_in.wlightDir + fs_in.wviewDir);
			float NdotH = max(dot(fs_in.wnormal, halfway), 0.0);
			vec3 specular = pow(NdotH, 32.0) * vec3(0.3);

			float shadow = ShadowCalculation(fs_in.fragPosLightSpace);

			vec3 ambient = col * 0.2; // 环境光
			vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * lightCol;
			out_color = vec4(result, 1.0);
		}
	)";
	GLuint vs_sh = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs_sh, 1, &vs, NULL);
	glCompileShader(vs_sh);

	GLuint fs_sh = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs_sh, 1, &fs, NULL);
	glCompileShader(fs_sh);

	program = glCreateProgram();
	glAttachShader(program, vs_sh);
	glAttachShader(program, fs_sh);
	glLinkProgram(program);
	glUseProgram(program);

	GLint ok = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &ok);
	if (!ok) { char log[512]; glGetProgramInfoLog(program, 512, nullptr, log); std::cout << log; }

	//bind Uniform
	glUniform1i(glGetUniformLocation(program, "diffuse_map"), 0);
	//设置Camera参数
	camera.worPosition = glm::vec3(0, 0, 6);
	camera.center = glm::vec3(0, 3, 0);
	camera.up = glm::vec3(0, 1, 0);
	//设置Uniform变量
	uniforms.wlightPos = glm::vec3(-2, 3, 4);
	uniforms.wviewPos = camera.worPosition;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//vertex数据设置
	glBufferData(GL_ARRAY_BUFFER, getSubMeshSize(), NULL, GL_STATIC_DRAW);
	int offset_v = 0;
	for (Mesh mesh : subMesh)
	{
		glBufferSubData(GL_ARRAY_BUFFER, offset_v, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data());
		offset_v += mesh.vertices.size() * sizeof(Vertex);
	}
	//indices索引设置
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, getTotalIndeicesSize(), indices.data(), GL_STATIC_DRAW);

	//set the offset of data (test for one mesh)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	int offset = 0;
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);//position
	offset += sizeof(glm::vec4);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);//normal
	offset += sizeof(glm::vec3);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);//color
	offset += sizeof(glm::vec3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);//uv

	glUseProgram(0);
	//设置深度，剔除
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);

	//plane
	const char* planeVS =
		R"(
		#version 450
		layout(location = 0) in vec3 position;

		out vec4 fragPosLightSpace;

		uniform mat4 MVP;
		uniform mat4 lightSpaceMatrix;   // 地面也要进光源空间

		void main()
		{
			gl_Position = MVP * vec4(position, 1.0);
			fragPosLightSpace = lightSpaceMatrix * vec4(position, 1.0);
		}
		)";

	const char* planeFS =
		R"(
		#version 450
		in vec4 fragPosLightSpace;
		out vec4 outColor;

		uniform sampler2D shadowMap;

		float ShadowCalculation(vec4 fragPosLightSpace)
		{
			// 透视除法 -> [0,1]
			vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
			projCoords = projCoords * 0.5 + 0.5;

			if (projCoords.z > 1.0) return 0.0;

			float closestDepth = texture(shadowMap, projCoords.xy).r;
			float currentDepth = projCoords.z;
			float bias = 0.005;                          // 可调

			return (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
		}

		void main()
		{
			float shadow = ShadowCalculation(fragPosLightSpace);
			vec3 color = vec3(0.7) * (1.0 - shadow);
			outColor = vec4(color, 1.0);
		}
	)";
	GLuint vsPlane = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsPlane, 1, &planeVS, nullptr);
	glCompileShader(vsPlane);

	GLuint fsPlane = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsPlane, 1, &planeFS, nullptr);
	glCompileShader(fsPlane);

	programPlane = glCreateProgram();
	glAttachShader(programPlane, vsPlane);
	glAttachShader(programPlane, fsPlane);
	glLinkProgram(programPlane);
	glUseProgram(programPlane);

	// 平面顶点
	planeVertices = {
		-10.0f, -1.0f, -10.0f,
		 10.0f, -1.0f, -10.0f,
		 10.0f, -1.0f,  10.0f,
		-10.0f, -1.0f,  10.0f
	};

	// 平面索引
	planeIndices = { 0, 1, 2, 0, 2, 3 };

	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);

	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(float), planeVertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &planeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndices.size() * sizeof(unsigned int), planeIndices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glUseProgram(0);
	//天空盒
	float skyboxVertices[] = {
		// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// 天空盒 shader
	const char* skyboxVS =
		R"(
			#version 450
			layout(location = 0) in vec3 position;
			out vec3 TexCoords;

			uniform mat4 projection;
			uniform mat4 view;

			void main() 
			{
				TexCoords = position;
				vec4 pos = projection * view * vec4(position, 1.0);
				gl_Position = pos.xyww;
			}
		)";

	const char* skyboxFS =
		R"(
			#version 450
			in vec3 TexCoords;
			out vec4 FragColor;

			uniform samplerCube skybox;

			void main() 
			{
				FragColor = texture(skybox, TexCoords);
			}
		)";

	skyboxProgram = glCreateProgram();
	GLuint skybox_vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(skybox_vs, 1, &skyboxVS, nullptr);
	glCompileShader(skybox_vs);
	GLuint skybox_fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(skybox_fs, 1, &skyboxFS, nullptr);
	glCompileShader(skybox_fs);
	glAttachShader(skyboxProgram, skybox_vs);
	glAttachShader(skyboxProgram, skybox_fs);
	glLinkProgram(skyboxProgram);
	glUseProgram(skyboxProgram);

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces = {
		"D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_left.png",
		"D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_right.png",
		"D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_up.png",
		"D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_down.png",
		"D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_front.png",
		"D:/Unity/unity project/MoveAndAnimation/Assets/Fantasy Skybox FREE/Cubemaps/FS013/Images/FS013_Night_Cubemap_back.png"
	};
	cubemapTexture = loadCubemap(faces);
	glUseProgram(0);
	//shadow
	const char* shadowVS =
		R"(
		#version 450
		layout(location = 0) in vec4 position;
		uniform mat4 lightSpaceMatrix;
		uniform mat4 model;
		void main()
		{
			gl_Position = lightSpaceMatrix * model * position;
		}
	)";

	const char* shadowFS =
		R"(
		#version 450
		void main()
		{
		}
	)";
	shadowProgram = glCreateProgram();
	GLuint shadow_vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint shadow_fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shadow_vs, 1, &shadowVS, nullptr);
	glShaderSource(shadow_fs, 1, &shadowFS, nullptr);
	glCompileShader(shadow_vs); glCompileShader(shadow_fs);
	glAttachShader(shadowProgram, shadow_vs); glAttachShader(shadowProgram, shadow_fs);
	glLinkProgram(shadowProgram);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	SHADOW_W = width;
	SHADOW_H = height;
	initshadow();
}

void LuoTY::render()
{
	while (!glfwWindowShouldClose(window))
	{
		const GLfloat backGround[] = { 1.0,0.0,1.0,1.0 };
		//glClearBufferfv(GL_COLOR, 0, backGround);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		processInput(window);
		//shadow
		glm::vec3 lightPos = uniforms.wlightPos;
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		float orthoSize = 15.0f;
		//模拟方向光使用正交
		glm::mat4 lightProj = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 1.0f, 50.0f);
		lightSpaceMatrix = lightProj * lightView;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		SHADOW_W = width;
		SHADOW_H = height;
		//glEnable(GL_MULTISAMPLE);
		glViewport(0, 0, SHADOW_W, SHADOW_H);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glUseProgram(shadowProgram);
		glUniformMatrix4fv(glGetUniformLocation(shadowProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
		//画模型（仅写深度）
		glm::mat4 M_shadow = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
		glUniformMatrix4fv(glGetUniformLocation(shadowProgram, "model"),
			1, GL_FALSE, &M_shadow[0][0]);
		glBindVertexArray(vao);
		for (auto& mesh : subMesh)
		{
			glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT,
				(void*)(mesh.indexOffset * sizeof(unsigned int)));
		}

		//画地面（也要投阴影）
		glm::mat4 Mplane = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shadowProgram, "model"),
			1, GL_FALSE, &Mplane[0][0]);
		glBindVertexArray(planeVAO);
		glDrawElements(GL_TRIANGLES, planeIndices.size(), GL_UNSIGNED_INT, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//Uniform设置
		GLfloat time = (GLfloat)glfwGetTime();
		//glm::mat4 M = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0, 1, 0));
		glm::mat4 M = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
		glm::mat4 V = glm::lookAt(camera.worPosition, camera.center, camera.up);
		glm::mat4 P = glm::perspective(glm::radians(60.0f), 800.0f / 500.0f, 0.1f, 1000.0f);
		glm::mat4 MV = V * M;
		glm::mat4 MVP = P * V * M;
		//或者使用glm::value_ptr(MVP)获取指针
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"),
			1, GL_FALSE, &lightSpaceMatrix[0][0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
		glUniform1i(glGetUniformLocation(program, "shadowMap"), 1);
		glBindVertexArray(vao);
		glUniformMatrix4fv(glGetUniformLocation(program, "M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniform3fv(glGetUniformLocation(program, "wlightPos"), 1, glm::value_ptr(uniforms.wlightPos));
		glUniform3fv(glGetUniformLocation(program, "wviewPos"), 1, glm::value_ptr(uniforms.wviewPos));
		//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		for (size_t i = 0; i < subMesh.size(); i++)
		{
			Mesh mesh = subMesh[i];
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.index_diffuse);
			//GLint loc = glGetUniformLocation(program, "diffuse_map");
			//std::cout << "Uniform location: " << loc << std::endl;
			glDrawElements(GL_TRIANGLES, mesh.indexCount,
				GL_UNSIGNED_INT, (void*)(mesh.indexOffset * sizeof(unsigned int)));
		}
		//绘制平面
		glm::mat4 M_plane = glm::mat4(1.0f);
		glm::mat4 MVP_plane = P * V * M_plane;

		glUseProgram(programPlane);
		glUniformMatrix4fv(glGetUniformLocation(programPlane, "lightSpaceMatrix"),
			1, GL_FALSE, &lightSpaceMatrix[0][0]);
		glActiveTexture(GL_TEXTURE0);   // 地面只用阴影图
		glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
		glUniform1i(glGetUniformLocation(programPlane, "shadowMap"), 0);
		glBindVertexArray(planeVAO);
		glUniformMatrix4fv(glGetUniformLocation(programPlane, "MVP"), 1, GL_FALSE, &MVP_plane[0][0]);
		glDrawElements(GL_TRIANGLES, planeIndices.size(), GL_UNSIGNED_INT, 0);
		//绘制天空盒
		glDepthFunc(GL_LEQUAL); // 允许天空盒在最远处
		glDepthMask(GL_FALSE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glUseProgram(skyboxProgram);
		glm::mat4 view = glm::mat4(glm::mat3(V)); // 去掉平移
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projection"), 1, GL_FALSE, &P[0][0]);
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);   // 恢复深度写入
		glDepthFunc(GL_LESS);   // 恢复深度测试

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void LuoTY::shutdown()
{
	//释放 Assimp 的 CPU 内存
	importer.FreeScene();

	//释放 CPU 侧的顶点/索引数据
	subMesh.clear();
	subMesh.shrink_to_fit();
	indices.clear();
	indices.shrink_to_fit();
	tex_diffuse.clear();
	tex_diffuse.shrink_to_fit();

	//释放 GPU 资源
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteProgram(program);
	for (GLuint tex : tex_diffuse)
		glDeleteTextures(1, &tex);

	//plane
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &planeEBO);
	glDeleteProgram(programPlane);

	//销毁窗口
	glfwDestroyWindow(window);
	glfwTerminate();
}

void LuoTY::loadPMX(const std::string& filePath)
{
	int numMeshes;
	int baseVertex = 0;
	aiMaterial* material;

	scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

	numMeshes = scene->mNumMeshes;

	if (!scene)
	{
		std::cout << importer.GetErrorString() << std::endl;
		return;
	}
	else
	{
		std::cout << "model import success" << std::endl;
	}
	//遍历mesh
	subMesh.reserve(numMeshes);
	tex_diffuse.resize(scene->mNumMaterials, 0);
	for (size_t i = 0; i < numMeshes; i++)
	{
		int numVertices;
		int numIndices;

		aiMesh* mesh;
		mesh = scene->mMeshes[i];


		Mesh mymesh;
		numVertices = mesh->mNumVertices;
		numIndices = mesh->mFaces->mNumIndices;
		//获取顶点数据
		mymesh.vertices.reserve(numVertices);
		for (size_t j = 0; j < numVertices; j++)
		{
			Vertex v;

			v.position = glm::vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0);
			v.normal = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
			v.color = glm::vec3(0.4f, 0.4f, 0.4f);
			//二维数组的第一位表示第几组纹理坐标
			v.uv = glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
			mymesh.vertices.push_back(v);
		}
		mymesh.indexCount = mesh->mNumFaces * 3;
		mymesh.indexOffset = indices.size();
		//获取顶点索引
		indices.reserve(numIndices);
		for (size_t k = 0; k < mesh->mNumFaces; k++)
		{
			aiFace face;
			face = mesh->mFaces[k];
			for (size_t l = 0; l < face.mNumIndices; l++)
			{
				indices.push_back(face.mIndices[l] + baseVertex);
			}
		}
		baseVertex += mesh->mNumVertices;
		subMesh.push_back(mymesh);

		//subMesh.back().index_diffuse = tex_diffuse[mesh->mMaterialIndex];
	}
	//获取材质信息
	//这里存在一些问题，材质文件格式转化时间太长，如何缩短时间？
	for (size_t m = 0; m < scene->mNumMaterials; m++)
	{
		material = scene->mMaterials[m];
		aiString texPath;
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
			material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
		{
			std::string modelFolder = filePath.substr(0, filePath.find_last_of("/\\") + 1);
			std::string fullPath = modelFolder + texPath.C_Str();
			tex_diffuse[m] = loadTexFromFile(fullPath);
			std::cout << "loading... " << ((float)(m + 1) / scene->mNumMaterials) * 100 << "%" << std::endl;
		}
	}
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		unsigned int matIdx = scene->mMeshes[i]->mMaterialIndex;
		subMesh[i].index_diffuse = tex_diffuse[matIdx];
	}
}
int LuoTY::getSubMeshSize()
{
	int size = 0;
	int num = scene->mNumMeshes;
	for (size_t m = 0; m < num; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];
		size += sizeof(Vertex) * mesh->mNumVertices;
	}
	return size;
}
int LuoTY::getTotalIndeicesSize()
{
	int count = 0;
	for (size_t m = 0; m < scene->mNumMeshes; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];
		for (size_t n = 0; n < mesh->mNumFaces; n++)
		{
			aiFace* face = &mesh->mFaces[n];
			count += sizeof(unsigned int) * face->mNumIndices;
		}
	}
	return count;
}
GLuint LuoTY::loadTexFromFile(const std::string& path)
{
	int w, h, n;
	unsigned char* data = stbi_load(path.c_str(), &w, &h, &n, 4);
	if (!data)
	{
		std::cerr << "stb failed: " << path << std::endl;
		return 0;
	}
	//将texture转化为OpenGL可识别格式
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return tex;
}
GLuint LuoTY::loadCubemap(std::vector<std::string> faces) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			GLenum format = GL_RGB;
			if (nrChannels == 4) format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
void LuoTY::initshadow()
{
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glGenTextures(1, &shadowDepthTex);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_W, SHADOW_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Shadow FBO not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void LuoTY::resizeShadowMap(int width, int height) {
	SHADOW_W = width;
	SHADOW_H = height;

	// 重新分配阴影深度贴图
	glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_W, SHADOW_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void framebufferSize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	LuoTY* app = static_cast<LuoTY*>(glfwGetWindowUserPointer(window));
	if (app) {
		app->resizeShadowMap(width, height);
	}
}
//key弃用
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float sensitivity = 0.5f;
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwTerminate();
			break;
		case GLFW_KEY_ENTER:
			glfwTerminate();
			break;
		case GLFW_KEY_A:
			camera.worPosition.x -= sensitivity;
			//camera.center.x -= sensitivity;
			break;
		case GLFW_KEY_D:
			camera.worPosition.x += sensitivity;
			//camera.center.x += sensitivity;
			break;
		case GLFW_KEY_W:
			camera.worPosition.y += sensitivity;
			//camera.center.y += sensitivity;
			break;
		case GLFW_KEY_S:
			camera.worPosition.y -= sensitivity;
			//camera.center.y -= sensitivity;
			break;

		}

	}

}
//鼠标偏移控制视角
//这个版本可以解决跳变的问题,正向推偏航角和俯仰角会有跳变问题
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	float sensitivity = 0.1f;
	float offsetX, offsetY;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (isFirst)
		{
			lastX = xpos;
			lastY = ypos;
			// 反推yaw和pitch
			glm::vec3 dir = glm::normalize(camera.center - camera.worPosition);
			pitch = glm::degrees(asin(dir.y));
			yaw = glm::degrees(atan2(dir.z, dir.x));
			isFirst = false;
			return; // 本帧不做视角调整，防止跳变
		}

		offsetX = xpos - lastX;
		offsetY = lastY - ypos;
		lastX = xpos;
		lastY = ypos;
		// 偏航角
		yaw += offsetX * sensitivity;
		// 俯仰角
		pitch += offsetY * sensitivity;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		camera.center = camera.worPosition + glm::normalize(direction);
	}
	else
	{
		isFirst = true;
	}
}//鼠标滚轮实现缩放
//鼠标滚轮控制
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float sensitivity = 0.1f;
	glm::vec3 forward = glm::normalize(camera.center - camera.worPosition);
	//鼠标滚轮向上
	if (yoffset > 0)
	{
		camera.worPosition += forward * sensitivity;
		camera.center += forward * sensitivity;
	}
	//鼠标滚轮向下
	if (yoffset < 0)
	{
		camera.worPosition -= forward * sensitivity;
		camera.center -= forward * sensitivity;
	}
}
//键盘输入控制
void processInput(GLFWwindow* window)
{
	float sensitivity = 0.1f;
	glm::vec3 forward = glm::normalize(camera.center - camera.worPosition);
	glm::vec3 right = glm::normalize(glm::cross(forward, camera.up));
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwTerminate();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.worPosition += forward * sensitivity;
		camera.center += forward * sensitivity;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.worPosition -= forward * sensitivity;
		camera.center -= forward * sensitivity;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.worPosition -= right * sensitivity;
		camera.center -= right * sensitivity;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.worPosition += right * sensitivity;
		camera.center += right * sensitivity;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		camera.worPosition -= camera.up * sensitivity;
		camera.center -= camera.up * sensitivity;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.worPosition += camera.up * sensitivity;
		camera.center += camera.up * sensitivity;
	}

}

int main()
{
	LuoTY LTY;
	LTY.Init();

	LTY.render();
	LTY.shutdown();
	return 0;

}