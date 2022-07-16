// This has been adapted from the Vulkan tutorial
#include<ctime>
#include "MyProject.hpp"

// The global ubo contains view and proj matrices which don't change between objects
// Set 0, binding 0 is the object with view and proj
struct globalUniformBufferObject {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	glm::vec3 lightDir;
	glm::vec3 lightPos;
	glm::vec3 lightColor;
	glm::vec3 eyePos;
	glm::vec4 coneInOutDecayExp;
	glm::vec4 selector;
};

// The ubo contains the model which changes between object and is set 1
// Set 1, binding 0 is the model
// Set 1, binding 1 is the texture
struct UniformBufferObject {
	alignas(16) glm::mat4 model;
};

// list of variables used
float randomRotYBigRock = 0.0f;
float randomRotYLittleRock = 0.0f;
float randomTranslationYLittleRock = -1.5f;
float randomTranslationYBigRock = -1.5f;
float pos = 0.0f;
float rock_pos = 0.0f;
float rock_pos2 = 0.0f;
float sea_pos = 5.0f;
float random_pos = 0.0f;
float random_pos2 = -6.0f;

float speeder = 0.0f;
float speederLimit = 0.1f;
float speederIncrement = 0.0000002;

float boatMovingPar = 0.025f;

float time_elapsed = 0.0f;
float vel = 1.0f;

bool gameOver = false;
bool gameStarted = false;

// MAIN ! 
class MyProject : public BaseProject {
protected:
	// Here you list all the Vulkan objects you need:

	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSLglobal;
	DescriptorSetLayout DSLobj;

	DescriptorSetLayout DSL_globalText;
	DescriptorSetLayout DSL_objText;


	// Pipelines [Shader couples]
	Pipeline P1;
	Pipeline P2; // this pipeline is for the gameover/newgame plane

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model M_Rock1;
	Texture T_Rock1;
	DescriptorSet DS_R1; // instance of DSLobj

	//NOTE: if we want more rocks to appear in the screen we need to
	//create multiple DescriptorSet and init them all

	Model M_Rock2;
	Texture T_Rock2;
	DescriptorSet DS_R2; // instance of DSLobj

	Model M_Boat;
	Texture T_Boat;
	DescriptorSet DS_Boat;

	Model M_Sea;
	Texture T_Sea;
	DescriptorSet DS_Sea;

	Model M_GameOver;
	Texture T_GameOver;
	DescriptorSet DS_GameOver;

	Texture T_NewGame;
	DescriptorSet DS_NewGame;

	DescriptorSet DS_global;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 1600;
		windowHeight = 1200;
		windowTitle = "Boat Runner";
		initialBackgroundColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Descriptor pool sizes
		uniformBlocksInPool = 6;
		texturesInPool = 6;
		setsInPool = 7;
	}

	// Here you load and setup all your Vulkan objects
	void localInit() {

		// Descriptor Layouts [what will be passed to the shaders]
		DSLobj.init(this, {
			// this array contains the binding:
			// first  element : the binding number
			// second element : the time of element (buffer or texture)
			// third  element : the pipeline stage where it will be used
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

		DSLglobal.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});

		DSL_objText.init(this, {
			// this array contains the binding:
			// first  element : the binding number
			// second element : the time of element (buffer or texture)
			// third  element : the pipeline stage where it will be used
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

		DSL_globalText.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});

		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		P1.init(this, "shaders/vert.spv", "shaders/frag.spv", { &DSLglobal , &DSLobj });

		// Models, textures and Descriptors (values assigned to the uniforms)
		M_Rock1.init(this, "models/Rock_1.obj");
		T_Rock1.init(this, "textures/Rock_1_Base_Color.jpg");
		DS_R1.init(this, &DSLobj, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Rock1}
			});

		M_Rock2.init(this, "models/rock1.obj");
		T_Rock2.init(this, "textures/rock_low_Base_Color.png");
		DS_R2.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Rock2}
			});

		M_Boat.init(this, "models/Boat.obj");
		T_Boat.init(this, "textures/boat_diffuse.bmp");
		DS_Boat.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Boat}
			});

		M_Sea.init(this, "models/LargePlane.obj");
		T_Sea.init(this, "textures/sea.jpeg");
		DS_Sea.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Sea}
			});

		DS_global.init(this, &DSLglobal, {
						{0, UNIFORM, sizeof(globalUniformBufferObject), nullptr}
			});

		P2.init(this, "shaders/vert2.spv", "shaders/menu_frag.spv", { &DSL_globalText , &DSL_objText });

		M_GameOver.init(this, "models/LargePlane.obj");
		T_GameOver.init(this, "textures/youdied3.png"); 
		DS_GameOver.init(this, &DSL_objText, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_GameOver}
			});

		T_NewGame.init(this, "textures/new_game.png"); 
		DS_NewGame.init(this, &DSL_objText, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_NewGame}
			});
	}

	// Here you destroy all the objects you created!		
	void localCleanup() {
		DS_R1.cleanup();
		T_Rock1.cleanup();
		M_Rock1.cleanup(); 

		DS_R2.cleanup();
		T_Rock2.cleanup();
		M_Rock2.cleanup();

		DS_Boat.cleanup();
		T_Boat.cleanup();
		M_Boat.cleanup();

		DS_Sea.cleanup();
		T_Sea.cleanup();
		M_Sea.cleanup();

		DS_GameOver.cleanup();
		T_GameOver.cleanup();
		M_GameOver.cleanup();

		DS_NewGame.cleanup();
		T_NewGame.cleanup();

		DS_global.cleanup();

		P1.cleanup();
		P2.cleanup();
		DSLglobal.cleanup();
		DSLobj.cleanup();
		DSL_globalText.cleanup();
		DSL_objText.cleanup();
	}

	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.graphicsPipeline);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 0, 1, &DS_global.descriptorSets[currentImage],
			0, nullptr);

		VkBuffer vertexBuffers[] = { M_Rock1.vertexBuffer };
		// property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		// property .indexBuffer of models, contains the VkBuffer handle to its index buffer
		vkCmdBindIndexBuffer(commandBuffer, M_Rock1.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		// property .pipelineLayout of a pipeline contains its layout.
		// property .descriptorSets of a descriptor set contains its elements.
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_R1.descriptorSets[currentImage],
			0, nullptr);
		// property .indices.size() of models, contains the number of triangles * 3 of the mesh.
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Rock1.indices.size()), 1, 0, 0, 0);
	
		VkBuffer vertexBuffers2[] = { M_Rock2.vertexBuffer };
		VkDeviceSize offsets2[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers2, offsets2);
		vkCmdBindIndexBuffer(commandBuffer, M_Rock2.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_R2.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Rock2.indices.size()), 1, 0, 0, 0);
	
		VkBuffer vertexBuffers3[] = { M_Boat.vertexBuffer };
		VkDeviceSize offsets3[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers3, offsets3);
		vkCmdBindIndexBuffer(commandBuffer, M_Boat.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_Boat.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Boat.indices.size()), 1, 0, 0, 0);

		VkBuffer vertexBuffers4[] = { M_Sea.vertexBuffer };
		VkDeviceSize offsets4[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers4, offsets4);
		vkCmdBindIndexBuffer(commandBuffer, M_Sea.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_Sea.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Sea.indices.size()), 1, 0, 0, 0);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			P2.graphicsPipeline);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P2.pipelineLayout, 0, 1, &DS_global.descriptorSets[currentImage],
			0, nullptr);

		VkBuffer vertexBuffers5[] = { M_GameOver.vertexBuffer };
		VkDeviceSize offsets5[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers5, offsets5);
		vkCmdBindIndexBuffer(commandBuffer, M_GameOver.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P2.pipelineLayout, 1, 1, &DS_GameOver.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_GameOver.indices.size()), 1, 0, 0, 0);

		VkBuffer vertexBuffers6[] = { M_GameOver.vertexBuffer };
		VkDeviceSize offsets6[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers6, offsets6);
		vkCmdBindIndexBuffer(commandBuffer, M_GameOver.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P2.pipelineLayout, 1, 1, &DS_NewGame.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_GameOver.indices.size()), 1, 0, 0, 0);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		/*static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();*/

		globalUniformBufferObject gubo{};
		UniformBufferObject ubo{};

		gubo.lightDir = glm::vec3(0.0f, 20.0f, -30.0f);
		gubo.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		gubo.lightPos = glm::vec3(0.0f, 20.0f, -25.0f);
		gubo.eyePos = glm::vec3(0.0f, 20.0f, -25.0f);
		gubo.coneInOutDecayExp = glm::vec4(0.9f, 0.92f, 2.0f, 0.0f);
		gubo.selector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

		void* data;

		//if the game is over, move the camera to another direction that displays the "GAME OVER" sign.
		if (gameOver == true || gameStarted == false) {
			gubo.view = glm::lookAt(glm::vec3(0.0f, 20.0f, -25.0f),
				glm::vec3(0.0f, 20.0f, -30.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
		} else {
			gubo.view = glm::lookAt(glm::vec3(0.0f, 20.0f, -25.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
		}
		gubo.proj = glm::perspective(glm::radians(45.0f),
			swapChainExtent.width / (float)swapChainExtent.height,
			0.1f, 1000.0f);
		gubo.proj[1][1] *= -1;

		vkMapMemory(device, DS_global.uniformBuffersMemory[0][currentImage], 0,
			sizeof(gubo), 0, &data);
		memcpy(data, &gubo, sizeof(gubo));
		vkUnmapMemory(device, DS_global.uniformBuffersMemory[0][currentImage]);

		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			gameStarted = true;
		}

		if (gameStarted == true) {
			// Here is where you actually update your uniforms
			// For rock 1
			if (20.0f + rock_pos * 4.0f > -20.0f) {
				rock_pos -= 0.0025f + speeder;
			}
			else {
				randomRotYLittleRock = rand() % 360;
				randomTranslationYLittleRock = (rand() % 10) * 0.1 - 0.5f;
				rock_pos = 7.0f;
				srand(time(NULL));
				random_pos2 = 10.0f - (rand() % 10) * 2; //change the position
			}
			ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(random_pos2, randomTranslationYLittleRock, 25.0f + rock_pos * 4.0f));
			ubo.model = glm::rotate(ubo.model, glm::radians(randomRotYLittleRock),
				glm::vec3(0.0f, 1.0f, 0.0f));
			vkMapMemory(device, DS_R1.uniformBuffersMemory[0][currentImage], 0,
				sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(device, DS_R1.uniformBuffersMemory[0][currentImage]);

			// For rock 2
			if (15.0f + rock_pos2 * 4.0f > -20.0f) {
				rock_pos2 -= 0.0025f + speeder;
				if (speeder < speederLimit) speeder += speederIncrement;
				else speeder = speederLimit;
			}
			else {
				randomRotYBigRock = rand() % 360;
				randomTranslationYBigRock = (rand() % 10) * 0.1 - 2.0f;
				rock_pos2 = 7.0f; // make the rock restart from the beginning
				srand(time(NULL));
				random_pos = 10.0f - (rand() % 10) * 2; //change the position
			}

			ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(random_pos, randomTranslationYBigRock, 15.0f + rock_pos2 * 4.0f));
			ubo.model = glm::rotate(ubo.model, glm::radians(randomRotYBigRock),
				glm::vec3(0.0f, 1.0f, 0.0f));
			vkMapMemory(device, DS_R2.uniformBuffersMemory[0][currentImage], 0,
				sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(device, DS_R2.uniformBuffersMemory[0][currentImage]);

			// For the boat
			float rotx = 0.0f;
			float roty = 90.0f;

			if (glfwGetKey(window, GLFW_KEY_D)) {
				if (pos > -10.0f) {
					pos -= boatMovingPar;
					rotx = -15.0f;
					roty = 85.0f;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_A)) {
				if (pos < 10.0f) {
					pos += boatMovingPar;
					rotx = 15.0f;
					roty = 95.0f;
				}
			}

			if ((glfwGetKey(window, GLFW_KEY_D)) && (glfwGetKey(window, GLFW_KEY_A))) {
				rotx = 0.0f;
				roty = 90.0f;
			}

			ubo.model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(pos, -0.2f, -8.0f)),
				glm::vec3(0.009f, 0.009f, 0.009f));
			ubo.model = glm::rotate(ubo.model, glm::radians(roty),
				glm::vec3(0.0f, 1.0f, 0.0f));
			ubo.model = glm::rotate(ubo.model, glm::radians(rotx),
				glm::vec3(1.0f, 0.0f, 0.0f));

			// CHECKS FOR COLLISIONS
			// collision with rock 1
			// from the left
			if ((pos + 1.0f >= random_pos2 && random_pos2 + 1.5f >= pos) && (-8.0f + 5.0f >= (25.0f + rock_pos * 4.0f) && (25.0f + rock_pos * 4.0f) + 2.5f >= -8.0f)) {
				//ubo.model = glm::rotate(ubo.model, glm::radians(-270.0f),
					//glm::vec3(1.0f, 0.0f, 0.0f));
				gameOver = true;
			}
			// from the right
			if ((pos - 1.0f <= random_pos2 && random_pos2 - 2.0f <= pos) && (-8.0f + 5.0f >= (25.0f + rock_pos * 4.0f) && (25.0f + rock_pos * 4.0f) + 2.5f >= -8.0f)) {
				//ubo.model = glm::rotate(ubo.model, glm::radians(270.0f),
					//glm::vec3(1.0f, 0.0f, 0.0f));
				gameOver = true;
			}

			// collision with rock 2
			float cosFactor = cos(glm::radians(randomRotYBigRock));
			if (cosFactor < 0) cosFactor *= -1;
			cosFactor = glm::max(cosFactor, 0.650f);
			float sinFactor = sin(glm::radians(randomRotYBigRock));
			if (sinFactor < 0) sinFactor *= -1;
			//sinFactor = glm::max(sinFactor, 0.650f);
			// from the left
			if ((pos + 1.0f >= random_pos && random_pos + 5.5f * cosFactor >= pos) && (-8.0f + 5.0f >= (13.0f + rock_pos2 * 4.0f - 2.0f * (sinFactor)) && ((15.0f + rock_pos2 * 4.0f) + 6.5f >= -8.0f))) {
				//ubo.model = glm::rotate(ubo.model, glm::radians(-270.0f),
					//glm::vec3(1.0f, 0.0f, 0.0f));
				gameOver = true;
			}
			//from the right
			if ((pos - 1.0f <= random_pos && random_pos - 5.0f * cosFactor <= pos) && (-8.0f + 5.0f >= (13.0f + rock_pos2 * 4.0f - 2.0f * (sinFactor)) && ((15.0f + rock_pos2 * 4.0f) + 6.5f >= -8.0f))) {
				//ubo.model = glm::rotate(ubo.model, glm::radians(270.0f),
					//glm::vec3(1.0f, 0.0f, 0.0f));
				gameOver = true;
			}

			rotx = 0.0f;
			roty = 90.0f;
			vkMapMemory(device, DS_Boat.uniformBuffersMemory[0][currentImage], 0,
				sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(device, DS_Boat.uniformBuffersMemory[0][currentImage]);

			// For the sea
			if (sea_pos * 4.0f > 0.0f) {
				sea_pos -= (0.001f) + speeder;
			}
			else {
				sea_pos = 5.0f; // make the sea restart from the beginning
			}
			ubo.model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, sea_pos * 4.0f)),
				glm::vec3(4.0f, 4.0f, 4.0f));
			vkMapMemory(device, DS_Sea.uniformBuffersMemory[0][currentImage], 0,
				sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(device, DS_Sea.uniformBuffersMemory[0][currentImage]);

			// GAME RESET
			if (glfwGetKey(window, GLFW_KEY_ENTER)) {
				randomRotYBigRock = 0.0f;
				randomRotYLittleRock = 0.0f;
				randomTranslationYLittleRock = -1.5f;
				randomTranslationYBigRock = -1.5f;
				pos = 0.0f;
				rock_pos = 0.0f;
				rock_pos2 = 0.0f;
				sea_pos = 5.0f;
				random_pos = 0.0f;
				random_pos2 = -6.0f;

				speeder = 0.0f;

				time_elapsed = 0.0f;
				vel = 1.0f;

				gameOver = false;
			}
		}

		//For the GameOver
		ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(-27.5f, 48.1f, -30.0f));
		ubo.model = glm::rotate(ubo.model, glm::radians(90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f));
		ubo.model = glm::rotate(ubo.model, glm::radians(180.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.model = glm::scale(ubo.model, glm::vec3(5.0f, 1.0f, 5.0f));
		if (gameOver == true) {
			vkMapMemory(device, DS_GameOver.uniformBuffersMemory[0][currentImage], 0,
				sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(device, DS_GameOver.uniformBuffersMemory[0][currentImage]);
		}
		else {
			vkMapMemory(device, DS_NewGame.uniformBuffersMemory[0][currentImage], 0,
				sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(device, DS_NewGame.uniformBuffersMemory[0][currentImage]);
		}
	}
};

// This is the main: probably you do not need to touch this!
int main() {
	MyProject app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}