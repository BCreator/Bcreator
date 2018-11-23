#include<iostream>
#include<stdio.h>

#include<boost/log/trivial.hpp>

/*TODO: Use the math library based on OpenCL in the future*/
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<ThirdParty/imgui/imgui.h>
#include<c2Foundation/c2Part.h>
#include<c2PreDefined.h>
#include<c2DefEvent.h>
#include<c2Application.h>
#include<c2PartController/c2PartCamera.h>
#include<Render/Shader.h>

#include<GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include<stb/stb_image.h>

////////////////////////////////////////////////////////////////////////////////
static int g_nWindWidth = 1680, g_nWindHeight = 1050;
static GLuint vao_block, vao_lamp;
/*view control*/
static Camera camera;
static float lastX = 0;
static float lastY = 0;
static bool firstMouse = true;
/*lighting*/
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
static Shader lightingShader;
static Shader lampShader;

#define PI 3.14159f

////////////////////////////////////////////////////////////////////////////////
class onUpdateFixFrame : public c2IAction {
public:
	virtual Status update() {
		const c2SysEvt::updatefixframe& evt = *(static_cast<const c2SysEvt::updatefixframe*>(_pEvt));
		/*--------------------------------------------------------------------*/
		ImGui::Begin("C2 Director");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		/*--------------------------------------------------------------------*/
		/*glfwGetFramebufferSize（window，＆width，＆height); TODO: to used in onSize*/
		glViewport(0, 0, g_nWindWidth, g_nWindHeight);//FIXME:放到framebuffer resize的地方
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*--------------------------------------------------------------------*/
		lightingShader.use();
		/*Camera keyboard control*/
		if (glfwGetKey(evt._pWnd, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, evt._dElapsed);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, evt._dElapsed);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, evt._dElapsed);
		if (glfwGetKey(evt._pWnd, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, evt._dElapsed);
		/*--------------------------------------------------------------------*/
		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
			(float)g_nWindWidth / (float)g_nWindHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model;
		lightingShader.setMat4("model", model);
		// render the cube
		glBindVertexArray(vao_block);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		/*--------------------------------------------------------------------*/
		// also draw the lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		// world transformation
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);
		glBindVertexArray(vao_lamp);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		/*--------------------------------------------------------------------*/
		return Status::Success;
	}
};

////////////////////////////////////////////////////////////////////////////////
class onSysInitialized : public c2IAction {
	virtual Status update() {
		const c2SysEvt::initialized& evt = *(static_cast<const c2SysEvt::initialized*>(_pEvt));
		glfwSetInputMode(evt._pWnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.15f, 0.15f, 0.10f, 1.00f);
		/*------------------------------------------------------------------------*/
#ifdef C2_USE_OPENGLES
		lightingShader.create("es3_block.vs", "es3_block.fs");
		lampShader.create("es3_lamp.vs", "es3_lamp.fs");
#else
		lightingShader.create("block.vs", "block.fs");
		lampShader.create("lamp.vs", "lamp.fs");
#endif//C2_USE_OPENGLES

		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		/*--------------------------------------------------------------------*/
		// first, configure the cube's VAO (and VBO)
		glGenVertexArrays(1, &vao_block);
		glBindVertexArray(vao_block);
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		/*--------------------------------------------------------------------*/
		// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
		glGenVertexArrays(1, &vao_lamp);
		glBindVertexArray(vao_lamp);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// note that we update the lamp's position attribute's stride to reflect the updated buffer data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		/*--------------------------------------------------------------------*/
		//Rest
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		/*====================================================================*/
		BOOST_LOG_TRIVIAL(info) << "C2engine intialized.";
		return Status::Success;
	}
};

/*TODO: onTerminin free everything.*/
class onTerminate : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onTerminate";
		return Status::Success;
	}
};


////////////////////////////////////////////////////////////////////////////////
class onMouseButton : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onMouseButton";
		return Status::Success;
	}
};

class onCursorMoved : public c2IAction {
public:
	virtual Status update() {
		const c2SysEvt::cursor_moved& evt = *(static_cast<const c2SysEvt::cursor_moved*>(_pEvt));
		if (firstMouse) {
			lastX = evt._x;
			lastY = evt._y;
			firstMouse = false;
		}
		float xoffset = evt._x - lastX;
		float yoffset = lastY - evt._y; // reversed since y-coordinates go from bottom to top
		lastX = evt._x;
		lastY = evt._y;
		camera.ProcessMouseMovement(xoffset, yoffset);
		return Status::Success;
	}
};

class onCursorEnter : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCursorEnter";
		return Status::Success;
	}
};

class onScrolled : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onScrolled";
		return Status::Success;
	}
};

const float cameraSpeed = .05;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
class onKey : public c2IAction {
public:
	virtual Status update() {
		const c2SysEvt::key& evt = *(static_cast<const c2SysEvt::key*>(_pEvt));
		if (evt._nAction != GLFW_PRESS)
			return Status::Success;
		switch (evt._nKey) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(evt._pWnd, true);
			break;
		}
		// 		const char *ts = glfwGetKeyName(evt._nKey, evt._nScancode);
		// 		if(ts)
		// 			BOOST_LOG_TRIVIAL(info) << "+++++++++++++  onKey   " << ts;
	}
};

class onCharInput : public c2IAction {
public:
	virtual Status update() {
		//		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCharInput";
		return Status::Success;
	}
};

class onCharModsInput : public c2IAction {
public:
	virtual Status update() {
		//		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCharModsInput";
		return Status::Success;
	}
};


////////////////////////////////////////////////////////////////////////////////
int main() {
	Uint32 syset_chunkoffet = 0;
	onSysInitialized initialized;
	c2asActSubEvt(initialized, syset_chunkoffet + c2SysET::initialized,
		sizeof(c2SysEvt::initialized));
	onTerminate terminate;
	c2asActSubEvt(terminate, syset_chunkoffet + c2SysET::terminate,
		sizeof(c2SysEvt::terminate));
	onUpdateFixFrame updatefixframe;
	c2asActSubEvt(updatefixframe, syset_chunkoffet + c2SysET::updatefixframe,
		sizeof(c2SysEvt::updatefixframe));

	onMouseButton mouse_button;
	c2asActSubEvt(mouse_button, syset_chunkoffet + c2SysET::mouse_button,
		sizeof(c2SysEvt::mouse_button));
	onCursorMoved cursor_moved;
	c2asActSubEvt(cursor_moved, syset_chunkoffet + c2SysET::cursor_moved,
		sizeof(c2SysEvt::cursor_moved));
	onCursorEnter cursor_enter;
	c2asActSubEvt(cursor_enter, syset_chunkoffet + c2SysET::cursor_enter,
		sizeof(c2SysEvt::cursor_enter));
	onScrolled scrolled;
	c2asActSubEvt(scrolled, syset_chunkoffet + c2SysET::scrolled,
		sizeof(c2SysEvt::scrolled));
	onKey key;
	c2asActSubEvt(key, syset_chunkoffet + c2SysET::key,
		sizeof(c2SysEvt::key));
	onCharInput char_input;
	c2asActSubEvt(char_input, syset_chunkoffet + c2SysET::char_input,
		sizeof(c2SysEvt::char_input));
	onCharModsInput charmods_input;
	c2asActSubEvt(charmods_input, syset_chunkoffet + c2SysET::charmods_input,
		sizeof(c2SysEvt::charmods_input));

	/**************************************************************************/
	c2AppRun(1, g_nWindWidth, g_nWindHeight, "C2engine.Creator", false);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
static const char* get_gl_error_text(int code) {
	switch (code) {
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";
	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";
	default:
		return "undefined error";
	}
}
int gl_check_errors(const char *file, int line) {
	int errors = 0;
	while (true) {
		GLenum x = glGetError();
		if (x == GL_NO_ERROR)
			return errors;
		BOOST_LOG_TRIVIAL(error) << file << ":" << line << ": OpenGL error: "
			<< x << "(" << get_gl_error_text(x) << ")";
		errors++;
	}
}
#if _DEBUG
#  define GL(line) {							\
       line;									\
       if (gl_check_errors(__FILE__, __LINE__))	\
			BOOST_ASSERT(false);}
#else
#  define GL(line) line
#endif

inline void CheckOpenGLError(const char* stmt, const char* fname, int line) {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("OpenGL error %08x, at %s:%i - for %s.\n", err, fname, line, stmt);
		exit(1);
	}
}

// helper macro that checks for GL errors.
#define GL_C(stmt) do {					\
	stmt;						\
	CheckOpenGLError(#stmt, __FILE__, __LINE__);	\
    } while (0)
