#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"
#include "control.hpp"

#include "defaults.hpp"
#include "simple_mesh.hpp"
#include "texture.hpp"
#include "loadobj.hpp"
#include "cylinder.hpp"
#include "spaceship.hpp"
#include "particle.hpp"
#include "rectangle.hpp"

#include <stdio.h>	// malloc, free, fopen, fclose, ftell, fseek, fread
#include <string.h> // memset
#include "../vmlib-test/empty.cpp"
// #define GLFONTSTASH_IMPLEMENTATION
// #include "modern_glfont.hpp"

namespace
{
	constexpr char const *kWindowTitle = "COMP3811 - Texturing";

	constexpr float kPi_ = 3.1415926f;

	constexpr float kMovementPerSecond_ = 5.f;	// units per second
	constexpr float kMouseSensitivity_ = 0.01f; // radians per pixel

	void glfw_callback_error_(int, char const *);

	void glfw_callback_key_(GLFWwindow *, int, int, int, int);
	void glfw_callback_motion_(GLFWwindow *, double, double);
	void glfw_callback_mouse_button_(GLFWwindow *, int, int, int);

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow *window;
	};

	bool isSpeedUp = false;

	Control leftController;
	Control rightController;
	SpaceShip spaceship;
	ParticleSystem particles;

	auto last = Clock::now();

	bool isSplit = 0;
	bool isShiftPress = false;

	bool buttonAPress = false;
	bool buttonAHover = false;
	bool buttonBPress = false;
	bool buttonBHover = false;

}

int main()
try
{
	// Initialize GLFW
	if (GLFW_TRUE != glfwInit())
	{
		char const *msg = nullptr;
		int ecode = glfwGetError(&msg);
		throw Error("glfwInit() failed with '%s' (%d)", msg, ecode);
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback(&glfw_callback_error_);

	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	// glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_DEPTH_BITS, 24);

#if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // ~ !NDEBUG

	GLFWwindow *window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr);

	if (!window)
	{
		char const *msg = nullptr;
		int ecode = glfwGetError(&msg);
		throw Error("glfwCreateWindow() failed with '%s' (%d)", msg, ecode);
	}

	GLFWWindowDeleter windowDeleter{window};

	// Set up event handling
	// State_ state{};

	glfwSetWindowUserPointer(window, &leftController);

	glfwSetKeyCallback(window, &glfw_callback_key_);
	glfwSetCursorPosCallback(window, &glfw_callback_motion_);
	glfwSetMouseButtonCallback(window, &glfw_callback_mouse_button_);

	// Set up drawing stuff
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if (!gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress))
		throw Error("gladLoaDGLLoader() failed - cannot load GL API!");

	std::printf("RENDERER %s\n", glGetString(GL_RENDERER));
	std::printf("VENDOR %s\n", glGetString(GL_VENDOR));
	std::printf("VERSION %s\n", glGetString(GL_VERSION));
	std::printf("SHADING_LANGUAGE_VERSION %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Ddebug output
#if !defined(NDEBUG)
	setup_gl_debug_output();
#endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	// TODO: global GL setup goes here
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_FRAMEBUFFER_SRGB);

	// glEnable(GL_CULL_FACE);
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	// This can be different from the window size, as standard window
	// decorations (title bar, borders, ...) may be included in the window size
	// but not be part of the drawable surface area.
	int iwidth, iheight;
	glfwGetFramebufferSize(window, &iwidth, &iheight);

	glViewport(0, 0, iwidth, iheight);

	// Load shader program
	ShaderProgram prog({{GL_VERTEX_SHADER, "assets/default.vert"},
						{GL_FRAGMENT_SHADER, "assets/default.frag"}});

	ShaderProgram lunchPad({{GL_VERTEX_SHADER, "assets/lunchpad.vert"},
							{GL_FRAGMENT_SHADER, "assets/lunchpad.frag"}});

	ShaderProgram font({{GL_VERTEX_SHADER, "assets/font.vert"},
						{GL_FRAGMENT_SHADER, "assets/font.frag"}});
	ShaderProgram spaceship_prog(
		{{GL_VERTEX_SHADER, "assets/spaceship.vert"},
		 {GL_FRAGMENT_SHADER, "assets/spaceship.frag"}});

	ShaderProgram particleProg(
		{{GL_VERTEX_SHADER, "assets/particle.vert"},
		 {GL_FRAGMENT_SHADER, "assets/particle.frag"}});

	// state.prog = &prog;
	// state.camControl.radius = 10.f;

	// Animation state
	last = Clock::now();

	auto map_obj = load_wavefront_obj("assets/parlahti.obj");
	auto landingpad_obj = load_wavefront_obj("assets/landingpad.obj");

	auto texture = load_texture_2d("assets/L4343A-4k.jpeg");
	auto particle_tex = load_texture_2d("assets/particles.png");
	// auto indian_workstation_tex = load_texture_2d("assets/skintest.jpg");

	// auto indian_workstation = load_wavefront_obj("assets/scan.obj");

	auto map_vao = create_vao(map_obj);
	auto map_vertex_cnt = map_obj.positions.size();

	auto landingpad_vao = create_vao(landingpad_obj);
	auto landingpad_vertex_cnt = landingpad_obj.positions.size();

	auto spaceshi_vao = spaceship.createSpaceShip();
	auto spaceshi_vertex_cnt = spaceship.getNumVertices();

	std::vector<std::pair<GLuint, particle>> particles_vao;

	auto test_vao = create_vao(make_cylinder(true, 20, Vec3f{1.f, 1.f, 0.8f},
											 kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
												 make_scaling(1.5f, 0.2f, 0.2f)));
	auto test_vertex_cnt = make_cylinder(true, 20, Vec3f{1.f, 1.f, 0.8f},
										 kIdentity44f * make_rotation_z(3.1415926f / 2.f) *
											 make_scaling(1.5f, 0.2f, 0.2f))
							   .positions.size();

	auto light_model = create_vao(make_cube(Vec3f{0.01f, 0.01f, 0.01f}, kIdentity44f * make_scaling(0.1f, 0.1f, 0.1f)));
	auto light_model_cnt = make_cube(Vec3f{0.01f, 0.01f, 0.01f}, kIdentity44f)
							   .positions.size();

	auto button_model = make_rectangle_2d(true, Vec3f{0.0f, 0.5f, 0.0f}, kIdentity44f * make_scaling(0.1f, 0.1f, 0.1f));
	auto button_vao = create_vao(button_model);
	auto button_vertex_cnt = button_model.positions.size();

	leftController = Control();
	// struct FONScontext* fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
	GLuint queryID[8];
	glGenQueries(8, queryID);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Let GLFW process events
		glfwPollEvents();

		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize(window, &nwidth, &nheight);

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if (0 == nwidth || 0 == nheight)
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize(window, &nwidth, &nheight);
				} while (0 == nwidth || 0 == nheight);
			}

			if (isSplit)
			{
				glViewport(0, 0, nwidth / 2, nheight);
			}
			else
			{
				glViewport(0, 0, nwidth, nheight);
			}
		}

		// Update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();

		if (leftController.getTrackingMode() != 0)
		{
			if (leftController.getTrackingMode() == 1)
			{
				leftController.setPos(spaceship.getPos() + Vec3f{0.f, 5.f, 5.f});
				leftController.setTheta(0.25f);
				leftController.setPhi(3.f);
			}
			else if (leftController.getTrackingMode() == 2)
			{
				if (!spaceship.isRunning)
				{
					leftController.setPos(spaceship.getPos() - Vec3f{5.f, -5.f, 5.f});
				}
				std::cout << spaceship.getPos().x << " " << spaceship.getPos().y << " " << spaceship.getPos().z << std::endl;
				Vec3f direction = normalize(leftController.getPos() - spaceship.getPos());
				leftController.rotate(0, 0);

				float theta = std::asin(direction.y);
				float phi = std::tanh(direction.z / direction.x);
				leftController.setTheta(theta);
				// if (!spaceship.isRunning)
				// {
				// 	phi += M_PI + M_PI / 3;
				// }
				leftController.setPhi(phi + M_PI * 0.5);
			}
		}

		if (rightController.getTrackingMode() != 0)
		{
			if (rightController.getTrackingMode() == 1)
			{
				rightController.setPos(spaceship.getPos() + Vec3f{0.f, 5.f, 5.f});
				rightController.setTheta(0.25f);
				rightController.setPhi(3.f);
			}
			else if (rightController.getTrackingMode() == 2)
			{
				if (!spaceship.isRunning)
				{
					rightController.setPos(spaceship.getPos() - Vec3f{5.f, -5.f, 5.f});
				}
				std::cout << spaceship.getPos().x << " " << spaceship.getPos().y << " " << spaceship.getPos().z << std::endl;
				Vec3f direction = normalize(rightController.getPos() - spaceship.getPos());
				rightController.rotate(0, 0);

				float theta = std::asin(direction.y);
				float phi = std::tanh(direction.z / direction.x);
				rightController.setTheta(theta);
				// if (!spaceship.isRunning)
				// {
				// 	phi += M_PI + M_PI / 3;
				// }
				rightController.setPhi(phi + M_PI * 0.5);
			}
		}

		spaceship.moveUp(dt);
		auto tmp = particles.createParticles(spaceship.getPos(), dt);
		particles_vao.insert(particles_vao.end(), tmp.begin(), tmp.end());

		// std::cout << "Angles: " << leftController.getPhi() << " " << leftController.getTheta() << std::endl;
		// std::cout << "left: " << leftController.getPos().x << " " << leftController.getPos().y << " " << leftController.getPos().z << std::endl;
		// std::cout << "right: " << rightController.getPos().x << " " << rightController.getPos().y << " " << rightController.getPos().z << std::endl;
		last = now;

		// TODO: draw frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(prog.programId());

		// Update: compute matrices
		// TODO: define and compute projCameraWorld matrix
		auto pos = leftController.getPos();

		Mat44f model2world = {{1.f, 0.f, 0.f, 0.f,
							   0.f, 1.f, 0.f, 0.f,
							   0.f, 0.f, 1.f, 0.f,
							   0.f, 0.f, 0.f, 1.f}};
		// Mat44f world2camera = controller.getViewMatrix();
		Mat44f projection;
		if (isSplit)
		{
			projection = make_perspective_projection(
				60.f * 3.1415926f / 180.f, // Yes, a proper π would be useful.
				(fbwidth / 2) / float(fbheight),
				0.1f, 100.0f);
		}
		else
		{

			projection = make_perspective_projection(
				60.f * 3.1415926f / 180.f, // Yes, a proper π would be useful.
				fbwidth / float(fbheight),
				0.1f, 100.0f);
		}

		// Mat44f world2camera = make_rotation_x(controller.getTheta()) * make_rotation_y(controller.getPhi()) * make_translation({ 0.f, 0.f, controller.getRadius() }) * make_translation(-controller.getPos());

		Mat44f projCameraWorld = projection * leftController.getViewMatrix() * model2world;
		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(model2world)));

		// Draw scene
		OGL_CHECKPOINT_DEBUG();

		// std::cout << texture << std::endl;

		Vec3f lightDir = normalize(Vec3f{0.f, 1.f, -1.f});
		glQueryCounter(queryID[0], GL_TIMESTAMP);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(map_vao);
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 0.9f, 0.9f, 0.6f);
		glUniform3f(4, 0.0f, 0.0f, 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, map_vertex_cnt);
		glQueryCounter(queryID[1], GL_TIMESTAMP);

		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, indian_workstation_tex);
		// glBindVertexArray(indian_workstation_vao);

		// glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		// glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		// glUniform3fv(2, 1, &lightDir.x);
		// glUniform3f(3, 0.9f, 0.9f, 0.6f);
		// glUniform3f(4, 0.05f, 0.05f, 0.05f);
		// glDrawArrays(GL_TRIANGLES, 0, indian_workstation_vertex_cnt);

		glUseProgram(lunchPad.programId());
		glQueryCounter(queryID[2], GL_TIMESTAMP);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		model2world = make_translation(Vec3f{-21.6833f, -0.97, -37.0551f});
		projCameraWorld = projection * leftController.getViewMatrix() * model2world;
		normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		glBindVertexArray(landingpad_vao);
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(2, 1, GL_TRUE, kIdentity44f.v);
		// glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 0.9f, 0.9f, 0.6f);
		glUniform3f(4, 0.05f, 0.05f, 0.05f);
		glUniform3f(6, 1.f, 0.f, 0.f); // light pos
		glUniform3f(16, leftController.getPos().x, leftController.getPos().y, leftController.getPos().z);
		glDrawArrays(GL_TRIANGLES, 0, landingpad_vertex_cnt);
glQueryCounter(queryID[3], GL_TIMESTAMP);
glQueryCounter(queryID[4], GL_TIMESTAMP);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		model2world = make_translation(Vec3f{0.f, -0.97f, 0.f});
		projCameraWorld = projection * leftController.getViewMatrix() * model2world;
		normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		glBindVertexArray(landingpad_vao);
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(2, 1, GL_TRUE, kIdentity44f.v);
		// glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 0.9f, 0.9f, 0.6f);
		glUniform3f(4, 0.05f, 0.05f, 0.05f);
		glUniform3f(6, 1.f, 0.f, 0.f); // light pos
		glUniform3f(16, leftController.getPos().x, leftController.getPos().y, leftController.getPos().z);

		glDrawArrays(GL_TRIANGLES, 0, landingpad_vertex_cnt);
		
		glQueryCounter(queryID[5], GL_TIMESTAMP);

		// draw spaceship
		glUseProgram(spaceship_prog.programId());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (spaceship.isRunning)
		{
			model2world = make_translation(spaceship.getPos()) * make_rotation_z(-spaceship.getRotateXY());
		}
		else
		{
			model2world = make_translation(spaceship.getPos());
		}
		projCameraWorld = projection * leftController.getViewMatrix() * model2world;
		normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		glBindVertexArray(spaceshi_vao);
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(2, 1, GL_TRUE, model2world.v);

		// uMaterialAmbient
		// uMaterialDiffuse
		// uMaterialSpecular
		// uMaterialShininess
		glUniform3f(36, 1.0f, 1.0f, 1.0f);
		glUniform3f(37, 1.0f, 1.0f, 1.f);
		glUniform3f(38, 1.f, 1.f, 1.f);
		glUniform1f(39, 32.0f);
		glUniform3f(40, 0.0f, 0.0f, 0.0f);
		Vec3f lightPos_1 = spaceship.getPos() + Vec3f{0.f, 0.4f, 0.5f};
		Vec3f lightPos_2 = spaceship.getPos() + Vec3f{0.f, 0.8f, 0.5f};
		Vec3f lightPos_3 = spaceship.getPos() + Vec3f{0.f, 1.6f, 0.5f};
		// light 1
		glUniform3f(21, lightPos_1.x, lightPos_1.y, lightPos_1.z); // light pos
		glUniform3f(22, 0.5f, 0.5f, 0.5f);					   // ambient
		glUniform3f(23, 0.9f, 0.9f, 0.8f);						   // diffuse
		glUniform3f(24, 0.f, 0.f, 1.f);							   // aColor
		glUniform3f(25, 0.f, 1.f, 1.f);
		// light 2
		glUniform3f(26, lightPos_2.x, lightPos_2.y, lightPos_2.z); // light pos
		glUniform3f(27, 0.5f, 0.5f, 0.5f);					   // ambient
		glUniform3f(28, 0.9f, 0.9f, 0.8f);						   // diffuse
		glUniform3f(29, 0.f, 1.f, 0.f);							   // aColor
		glUniform3f(30, 0.f, 1.f, 0.f);

		// light 3
		glUniform3f(31, lightPos_3.x, lightPos_3.y, lightPos_3.z); // light pos
		glUniform3f(32, 0.5f, 0.5f, 0.5f);					   // ambient
		glUniform3f(33, 0.9f, 0.9f, 0.8f);						   // diffuse
		glUniform3f(34, 1.f, 0.f, 0.f);							   // aColor
		glUniform3f(35, 1.f, 1.f, 1.f);

		glUniform3f(16, leftController.getPos().x, leftController.getPos().y + 1.f, leftController.getPos().z);
		glDrawArrays(GL_TRIANGLES, 0, spaceshi_vertex_cnt);
		

		// draw light model
		glUseProgram(lunchPad.programId());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		model2world = make_translation(lightPos_1);
		projCameraWorld = projection * leftController.getViewMatrix() * model2world;
		normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		glBindVertexArray(light_model);
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(2, 1, GL_TRUE, kIdentity44f.v);
		// glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 0.9f, 0.9f, 0.6f);
		glUniform3f(4, 0.05f, 0.05f, 0.05f);
		glUniform3f(6, 1.f, 0.f, 0.f); // light pos
		glUniform3f(16, leftController.getPos().x, leftController.getPos().y, leftController.getPos().z);
		glDrawArrays(GL_TRIANGLES, 0, light_model_cnt);

		glUseProgram(lunchPad.programId());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		model2world = make_translation(lightPos_2);
		projCameraWorld = projection * leftController.getViewMatrix() * model2world;
		normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		glBindVertexArray(light_model);
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(2, 1, GL_TRUE, kIdentity44f.v);
		// glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 0.9f, 0.9f, 0.6f);
		glUniform3f(4, 0.05f, 0.05f, 0.05f);
		glUniform3f(6, 1.f, 0.f, 0.f); // light pos
		glUniform3f(16, leftController.getPos().x, leftController.getPos().y, leftController.getPos().z);
		glDrawArrays(GL_TRIANGLES, 0, light_model_cnt);

		glUseProgram(lunchPad.programId());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		model2world = make_translation(lightPos_3);
		projCameraWorld = projection * leftController.getViewMatrix() * model2world;
		normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		glBindVertexArray(light_model);
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
		glUniformMatrix4fv(2, 1, GL_TRUE, kIdentity44f.v);
		// glUniform3fv(2, 1, &lightDir.x);
		glUniform3f(3, 0.9f, 0.9f, 0.6f);
		glUniform3f(4, 0.05f, 0.05f, 0.05f);
		glUniform3f(6, 1.f, 0.f, 0.f); // light pos
		glUniform3f(16, leftController.getPos().x, leftController.getPos().y, leftController.getPos().z);
		glDrawArrays(GL_TRIANGLES, 0, light_model_cnt);

		glUseProgram(particleProg.programId());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, particle_tex);
		model2world = kIdentity44f;
		projCameraWorld = projection * leftController.getViewMatrix() * model2world;
		normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
		// glUniform1f(5, alpha);
		glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
		glBindVertexArray(light_model);
		glDrawArrays(GL_TRIANGLES, 0, light_model_cnt);

		// draw particles
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(particleProg.programId());
		for (std::size_t i = 0; i < particles_vao.size(); i++)
		{
			// check if running
			bool isRunning = particles.getRunning();
			if (!isRunning)
			{
				break;
			}
			// check time
			if (particles_vao[i].second.time > 0.0f)
			{
				// update

				int rd = rand();
				if (rd % 2)
				{
					particles_vao[i].second.time -= dt;
					particles_vao[i].second.vel.y += 0.10f * dt;
					particles_vao[i].second.pos.x += 5 * particles_vao[i].second.vel.x * dt;
					particles_vao[i].second.pos.y += 5 * particles_vao[i].second.vel.y * dt;
					particles_vao[i].second.pos.z += 5 * particles_vao[i].second.vel.z * dt;
				}
				else
				{
					particles_vao[i].second.time -= dt;
					particles_vao[i].second.vel.y += 0.1f * dt;
					particles_vao[i].second.pos.x -= 5 * particles_vao[i].second.vel.x * dt;
					particles_vao[i].second.pos.y -= 5 * particles_vao[i].second.vel.y * dt;
					particles_vao[i].second.pos.z -= 5 * particles_vao[i].second.vel.z * dt;
				}

				if (particles_vao[i].second.pos.y < 0.0f)
				{
					particles_vao[i].second.time = 0.0f;
				}
				float alpha = particles_vao[i].second.time / 2.0f;

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, particle_tex);
				model2world = make_translation(particles_vao[i].second.pos);
				projCameraWorld = projection * leftController.getViewMatrix() * model2world;
				normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
				glUniform1f(50, alpha);
				glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
				glBindVertexArray(particles_vao[i].first);
				glDrawArrays(GL_TRIANGLES, 0, test_vertex_cnt);
			}
			else
			{
				particles_vao.erase(particles_vao.begin() + i);
			}
		}

		glDisable(GL_BLEND);

		// define startTime and endTime and elapsed by gl
		GLuint64 startTime, endTime, elapsed;

		for (int i = 0; i < 5; i += 2)
		{
			glGetQueryObjectui64v(queryID[i], GL_QUERY_RESULT, &startTime);
			glGetQueryObjectui64v(queryID[i + 1], GL_QUERY_RESULT, &endTime);
			if (startTime && endTime)
			{
				elapsed = endTime - startTime;
				// std::cout << "Time for section " << (i / 2) << ": " << elapsed << " ns" << std::endl;
			}
		}

		OGL_CHECKPOINT_DEBUG();

		if (isSplit)
		{
			glViewport(fbwidth / 2, 0, fbwidth / 2, fbheight);
			glUseProgram(prog.programId());
			Mat44f model2world = {{1.f, 0.f, 0.f, 0.f,
								   0.f, 1.f, 0.f, 0.f,
								   0.f, 0.f, 1.f, 0.f,
								   0.f, 0.f, 0.f, 1.f}};
			Mat44f projection;
			if (isSplit)
			{
				projection = make_perspective_projection(
					60.f * 3.1415926f / 180.f, // Yes, a proper π would be useful.
					(fbwidth / 2) / float(fbheight),
					0.1f, 100.0f);
			}
			else
			{

				projection = make_perspective_projection(
					60.f * 3.1415926f / 180.f, // Yes, a proper π would be useful.
					fbwidth / float(fbheight),
					0.1f, 100.0f);
			}

			// Mat44f world2camera = make_rotation_x(controller.getTheta()) * make_rotation_y(controller.getPhi()) * make_translation({ 0.f, 0.f, controller.getRadius() }) * make_translation(-controller.getPos());

			Mat44f projCameraWorld = projection * rightController.getViewMatrix() * model2world;
			Mat33f normalMatrix = mat44_to_mat33(transpose(invert(model2world)));

			// Draw scene
			OGL_CHECKPOINT_DEBUG();

			// std::cout << texture << std::endl;

			Vec3f lightDir = normalize(Vec3f{0.f, 1.f, -1.f});
			glQueryCounter(queryID[0], GL_TIMESTAMP);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glBindVertexArray(map_vao);
			glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniform3fv(2, 1, &lightDir.x);
			glUniform3f(3, 0.9f, 0.9f, 0.6f);
			glUniform3f(4, 0.0f, 0.0f, 0.0f);
			glDrawArrays(GL_TRIANGLES, 0, map_vertex_cnt);
			glQueryCounter(queryID[1], GL_TIMESTAMP);

			// glActiveTexture(GL_TEXTURE0);
			// glBindTexture(GL_TEXTURE_2D, indian_workstation_tex);
			// glBindVertexArray(indian_workstation_vao);

			// glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
			// glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			// glUniform3fv(2, 1, &lightDir.x);
			// glUniform3f(3, 0.9f, 0.9f, 0.6f);
			// glUniform3f(4, 0.05f, 0.05f, 0.05f);
			// glDrawArrays(GL_TRIANGLES, 0, indian_workstation_vertex_cnt);

			glUseProgram(lunchPad.programId());
			glQueryCounter(queryID[2], GL_TIMESTAMP);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			model2world = make_translation(Vec3f{-21.6833f, -0.97, -37.0551f});
			projCameraWorld = projection * rightController.getViewMatrix() * model2world;
			normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
			glBindVertexArray(landingpad_vao);
			glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniformMatrix4fv(2, 1, GL_TRUE, kIdentity44f.v);
			// glUniform3fv(2, 1, &lightDir.x);
			glUniform3f(3, 0.9f, 0.9f, 0.6f);
			glUniform3f(4, 0.05f, 0.05f, 0.05f);
			glUniform3f(6, 1.f, 0.f, 0.f); // light pos
			glUniform3f(16, rightController.getPos().x, rightController.getPos().y, rightController.getPos().z);
			glDrawArrays(GL_TRIANGLES, 0, landingpad_vertex_cnt);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			model2world = make_translation(Vec3f{0.f, -0.97f, 0.f});
			projCameraWorld = projection * rightController.getViewMatrix() * model2world;
			normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
			glBindVertexArray(landingpad_vao);
			glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniformMatrix4fv(2, 1, GL_TRUE, kIdentity44f.v);
			// glUniform3fv(2, 1, &lightDir.x);
			glUniform3f(3, 0.9f, 0.9f, 0.6f);
			glUniform3f(4, 0.05f, 0.05f, 0.05f);
			glUniform3f(6, 1.f, 0.f, 0.f); // light pos
			glUniform3f(16, rightController.getPos().x, rightController.getPos().y, rightController.getPos().z);

			glDrawArrays(GL_TRIANGLES, 0, landingpad_vertex_cnt);
			glQueryCounter(queryID[3], GL_TIMESTAMP);
			glQueryCounter(queryID[4], GL_TIMESTAMP);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			if (spaceship.isRunning)
			{
				model2world = make_translation(spaceship.getPos()) * make_rotation_z(-spaceship.getRotateXY());
			}
			else
			{
				model2world = make_translation(spaceship.getPos());
			}
			std::cout << spaceship.getPos().x << " " << spaceship.getPos().y << " " << spaceship.getPos().z << std::endl;
			projCameraWorld = projection * rightController.getViewMatrix() * model2world;
			normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
			glBindVertexArray(spaceshi_vao);
			glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
			glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);
			glUniformMatrix4fv(2, 1, GL_TRUE, kIdentity44f.v);
			// glUniform3fv(2, 1, &lightDir.x);
			glUniform3f(3, 0.9f, 0.9f, 0.6f);
			glUniform3f(4, 0.05f, 0.05f, 0.05f);
			glUniform3f(6, 1.f, 0.f, 0.f); // light pos
			glUniform3f(16, rightController.getPos().x, rightController.getPos().y, rightController.getPos().z);
			glDrawArrays(GL_TRIANGLES, 0, spaceshi_vertex_cnt);
			glQueryCounter(queryID[5], GL_TIMESTAMP);

			// loop through particles
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUseProgram(particleProg.programId());
			for (std::size_t i = 0; i < particles_vao.size(); i++)
			{
				// check if running
				bool isRunning = particles.getRunning();
				if (!isRunning)
				{
					break;
				}
				// check time
				if (particles_vao[i].second.time > 0.0f)
				{
					// update

					int rd = rand();
					if (rd % 2)
					{
						particles_vao[i].second.time -= dt;
						particles_vao[i].second.vel.y += 0.10f * dt;
						particles_vao[i].second.pos.x += 5 * particles_vao[i].second.vel.x * dt;
						particles_vao[i].second.pos.y += 5 * particles_vao[i].second.vel.y * dt;
						particles_vao[i].second.pos.z += 5 * particles_vao[i].second.vel.z * dt;
					}
					else
					{
						particles_vao[i].second.time -= dt;
						particles_vao[i].second.vel.y += 0.1f * dt;
						particles_vao[i].second.pos.x -= 5 * particles_vao[i].second.vel.x * dt;
						particles_vao[i].second.pos.y -= 5 * particles_vao[i].second.vel.y * dt;
						particles_vao[i].second.pos.z -= 5 * particles_vao[i].second.vel.z * dt;
					}

					if (particles_vao[i].second.pos.y < 0.0f)
					{
						particles_vao[i].second.time = 0.0f;
					}
					float alpha = particles_vao[i].second.time / 2.0f;

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, 0);
					model2world = make_translation(particles_vao[i].second.pos);
					projCameraWorld = projection * rightController.getViewMatrix() * model2world;
					normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
					glUniform1f(50, alpha);
					glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
					glBindVertexArray(particles_vao[i].first);
					glDrawArrays(GL_TRIANGLES, 0, test_vertex_cnt);
				}
				else
				{
					particles_vao.erase(particles_vao.begin() + i);
				}
			}
			glDisable(GL_BLEND);
		}

		// draw fonts
		glViewport(0, 0, fbwidth, fbheight);
		auto spaceship_pos = spaceship.getPos();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(font.programId());
		char speedStr[] = "altitude";
		float offset = 0.1f;
		for (int idx = 0; idx < 9; idx++)
		{
			auto chr = speedStr[idx];
			if (chr == '\0')
			{
				break;
			}
			// std::cout << chr << std::endl;
			auto atlasP = getAtlasPosByChar(chr);
			// std::cout << atlasP.row << " " << atlasP.col << std::endl;
			auto test_tex = load_font_atlas_texture_2d("assets/fonts.png", atlasP.row, atlasP.col, 64, 64);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, test_tex);

			auto rect = make_rectangle_2d(
				true,
				{0.f, 0.f, 0.f},
				kIdentity44f * make_translation({-0.9f + offset / 2, 0.9f, 0.f}) * make_scaling(0.08f, 0.1f, 0.08f) * make_rotation_z(3.1415926f) * make_rotation_y(3.1415926f));
			offset += 0.1f;

			auto vao = create_vao(rect);
			glBindVertexArray(vao);
			// glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
			glDrawArrays(GL_TRIANGLES, 0, rect.positions.size());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		offset = 0.1f;
		auto str = spaceship_pos.y > 0.f ? std::to_string(spaceship_pos.y) : "0";
		for (int idx = 0; idx < str.length(); idx++)
		{
			auto chr = str[idx];
			if (chr == '\0')
			{
				break;
			}
			// std::cout << chr << std::endl;
			auto atlasP = getAtlasPosByChar(chr);
			// std::cout << atlasP.row << " " << atlasP.col << std::endl;
			auto test_tex = load_font_atlas_texture_2d("assets/fonts.png", atlasP.row, atlasP.col, 64, 64);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, test_tex);

			auto rect = make_rectangle_2d(
				true,
				{0.f, 0.f, 0.f},
				kIdentity44f * make_translation({-0.9f + offset / 2, 0.8f, 0.f}) * make_scaling(0.08f, 0.1f, 0.08f) * make_rotation_z(3.1415926f) * make_rotation_y(3.1415926f));
			offset += 0.1f;

			auto vao = create_vao(rect);
			glBindVertexArray(vao);
			// glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
			glDrawArrays(GL_TRIANGLES, 0, rect.positions.size());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// two buttons
		Vec3f hoverColor = {0.0f, 1.0f, 0.0f};
		Vec3f normalColor = {0.0f, 0.0f, 0.0f};
		Vec3f presedColor = {1.0f, 0.0f, 0.0f};

		// left
		offset = 0.1f;

		glUseProgram(font.programId());
		char leftStr[] = "Reset";
		for (int idx = 0; idx < 6; idx++)
		{
			auto chr = leftStr[idx];
			if (chr == '\0')
			{
				break;
			}
			// std::cout << chr << std::endl;
			auto atlasP = getAtlasPosByChar(chr);
			// std::cout << atlasP.row << " " << atlasP.col << std::endl;
			auto test_tex = load_font_atlas_texture_2d("assets/fonts.png", atlasP.row, atlasP.col, 64, 64);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, test_tex);
			Vec3f currentColor;
			if (buttonAHover)
			{
				currentColor = hoverColor;
			}
			else if (buttonAPress)
			{
				currentColor = presedColor;
			}
			else
			{
				currentColor = normalColor;
			}

			auto rect = make_rectangle_2d(
				true,
				currentColor,
				kIdentity44f * make_translation({-0.5f + offset / 2, -0.9f, 0.f}) * make_scaling(0.08f, 0.1f, 0.08f) * make_rotation_z(3.1415926f) * make_rotation_y(3.1415926f));
			offset += 0.1f;

			auto vao = create_vao(rect);
			glBindVertexArray(vao);
			// glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
			glDrawArrays(GL_TRIANGLES, 0, rect.positions.size());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// right
		glUseProgram(font.programId());
		offset = 0.1f;

		char rightStr[] = "Lunch";
		for (int idx = 0; idx < 6; idx++)
		{
			auto chr = rightStr[idx];
			if (chr == '\0')
			{
				break;
			}
			// std::cout << chr << std::endl;
			auto atlasP = getAtlasPosByChar(chr);
			// std::cout << atlasP.row << " " << atlasP.col << std::endl;
			auto test_tex = load_font_atlas_texture_2d("assets/fonts.png", atlasP.row, atlasP.col, 64, 64);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, test_tex);
			Vec3f currentColor;
			if (buttonBHover)
			{
				currentColor = hoverColor;
			}
			else if (buttonBPress)
			{
				currentColor = presedColor;
			}
			else
			{
				currentColor = normalColor;
			}

			auto rect = make_rectangle_2d(
				true,
				currentColor,
				kIdentity44f * make_translation({-0.f + offset / 2, -0.9f, 0.f}) * make_scaling(0.08f, 0.1f, 0.08f) * make_rotation_z(3.1415926f) * make_rotation_y(3.1415926f));
			offset += 0.1f;

			auto vao = create_vao(rect);
			glBindVertexArray(vao);
			// glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);
			glDrawArrays(GL_TRIANGLES, 0, rect.positions.size());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glDisable(GL_BLEND);

		// Display results
		glfwSwapBuffers(window);
	}

	// Cleanup.
	// state.prog = nullptr;

	// TODO: additional cleanup

	return 0;
}
catch (std::exception const &eErr)
{
	std::fprintf(stderr, "Top-level Exception (%s):\n", typeid(eErr).name());
	std::fprintf(stderr, "%s\n", eErr.what());
	std::fprintf(stderr, "Bye.\n");
	return 1;
}

namespace
{
	void glfw_callback_error_(int aErrNum, char const *aErrDesc)
	{
		std::fprintf(stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum);
	}

	void glfw_callback_key_(GLFWwindow *aWindow, int aKey, int, int aAction, int)
	{
		if (GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction)
		{
			glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
			return;
		}

		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;
		// float totalDelta = 1.f;
		// float deltaX = cos(state->camControl.phi) * totalDelta;
		// float deltaY = sin(state->camControl.phi) * totalDelta;
		// std::cout << deltaX << " " << deltaY << std::endl;
		leftController.updatePos(dt);
		if (isSplit)
		{
			rightController.updatePos(dt);
		}
		if (GLFW_KEY_W == aKey)
		{
			if (GLFW_PRESS == aAction)
			{
				leftController.setForward(true);
				if (isSplit)
				{
					rightController.setForward(true);
				}
			}
			else if (GLFW_RELEASE == aAction)
			{
				leftController.setForward(false);
				if (isSplit)
				{
					rightController.setForward(false);
				}
			}
			// leftController.moveForward(dt);
		}
		if (GLFW_KEY_S == aKey)
		{
			if (GLFW_PRESS == aAction)
			{
				leftController.setBackward(true);
				if (isSplit)
				{
					rightController.setBackward(true);
				}
			}
			else if (GLFW_RELEASE == aAction)
			{
				leftController.setBackward(false);
				if (isSplit)
				{
					rightController.setBackward(false);
				}
			}
		}
		if (GLFW_KEY_A == aKey)
		{
			if (GLFW_PRESS == aAction)
			{
				leftController.setLeft(true);
				if (isSplit)
				{
					rightController.setLeft(true);
				}
			}
			else if (GLFW_RELEASE == aAction)
			{
				leftController.setLeft(false);
				if (isSplit)
				{
					rightController.setLeft(false);
				}
			}
		}
		if (GLFW_KEY_D == aKey)
		{
			if (GLFW_PRESS == aAction)
			{
				leftController.setRight(true);
				if (isSplit)
				{
					rightController.setRight(true);
				}
			}
			else if (GLFW_RELEASE == aAction)
			{
				leftController.setRight(false);
				if (isSplit)
				{
					rightController.setRight(false);
				}
			}
		}
		if (GLFW_KEY_Q == aKey)
		{
			if (GLFW_PRESS == aAction)
			{
				leftController.setDown(true);
				if (isSplit)
				{
					rightController.setDown(true);
				}
			}
			else if (GLFW_RELEASE == aAction)
			{
				leftController.setDown(false);
				if (isSplit)
				{
					rightController.setDown(false);
				}
			}
		}
		if (GLFW_KEY_E == aKey)
		{
			if (GLFW_PRESS == aAction)
			{
				leftController.setUp(true);
				if (isSplit)
				{
					rightController.setUp(true);
				}
			}
			else if (GLFW_RELEASE == aAction)
			{
				leftController.setUp(false);
				if (isSplit)
				{
					rightController.setUp(false);
				}
			}
		}
		if (GLFW_KEY_LEFT_SHIFT == aKey)
		{
			if (GLFW_PRESS == aAction)
			{
				isSpeedUp = true;
				leftController.setSpeed(leftController.getSpeed() * 2.f);
				isShiftPress = true;

				if (isSplit)
				{
					rightController.setSpeed(rightController.getSpeed() * 2.f);
				}
			}
			else if (GLFW_RELEASE == aAction)
			{
				isSpeedUp = false;
				leftController.setSpeed(leftController.getSpeed() / 2.f);
				isShiftPress = false;

				if (isSplit)
				{
					rightController.setSpeed(rightController.getSpeed() / 2.f);
				}
			}
		}
		if (GLFW_KEY_F == aKey)
		{
			if (GLFW_PRESS == aAction)
			{

				spaceship.setRunning(true);
				particles.setRunning(true);
			}
		}
		if (GLFW_KEY_R == aKey)
		{
			spaceship.reset();
			particles.reset();
		}
		if (GLFW_KEY_C == aKey)
		{
			if (GLFW_PRESS == aAction && !isShiftPress)
			{
				leftController.setTrackingMode((leftController.getTrackingMode() + 1) % 3);
				// std::cout << leftController.getTrackingMode() << std::endl;
			}
			else if (GLFW_PRESS == aAction && isShiftPress)
			{
				rightController.setTrackingMode((rightController.getTrackingMode() + 1) % 3);
				// std::cout << rightController.getTrackingMode() << std::endl;
			}
		}
		if (GLFW_KEY_V == aKey)
		{
			if (GLFW_PRESS == aAction)
			{
				isSplit = !isSplit;
				if (isSplit)
				{
					// std::cout << "Split screen mode" << std::endl;
					rightController = Control(leftController);
				}
				else
				{
					// std::cout << "Single screen mode" << std::endl;
				}
			}
		}
		// mouse left aKey
		// }
	}
	// }

	void glfw_callback_motion_(GLFWwindow *aWindow, double aX, double aY)
	{
		// left button area (327 666) -  (505 701)
		// right button area (648 667) - (824 700)
		std::cout << aX << " " << aY << std::endl;
		// check if in left button area
		if (aX > 327 && aX < 505 && aY > 666 && aY < 701)
		{
			// std::cout << "left button" << std::endl;
			buttonAHover = true;
		}
		else
		{
			buttonAHover = false;
		}
		// check if in right button area
		if (aX > 648 && aX < 824 && aY > 667 && aY < 700)
		{
			// std::cout << "right button" << std::endl;
			buttonBHover = true;
		}
		else
		{
			buttonBHover = false;
		}

		leftController.rotate(aX, aY);
		if (isSplit)
		{
			rightController.rotate(aX, aY);
		}
	}

	void glfw_callback_mouse_button_(GLFWwindow *aWindow, int aButton, int aAction, int aMods)
	{
		if (GLFW_MOUSE_BUTTON_LEFT == aButton)
		{
			if (GLFW_PRESS == aAction && buttonAHover)
			{
				buttonAPress = true;
			}
			else if (GLFW_RELEASE == aAction)
			{
				buttonAPress = false;
				spaceship.reset();
				particles.reset();
			}
		}
		if (GLFW_MOUSE_BUTTON_RIGHT == aButton)
		{
			if (GLFW_PRESS == aAction && buttonBHover)
			{
				buttonBPress = true;
			}
			else if (GLFW_RELEASE == aAction)
			{
				buttonBPress = false;
			}
		}
	}
}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if (window)
			glfwDestroyWindow(window);
	}
}
