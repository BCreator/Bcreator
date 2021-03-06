#include<iostream>
#include<stdio.h>

#include<boost/log/trivial.hpp>

/*TODO: Use the math library based on OpenCL in the future*/
//#define GLM_FORCE_MESSAGES
//#define GLM_FORCE_SIMD_
//#define GLM_FORCE_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_INLINE
//#define GLM_FORCE_CTOR_INIT//GLM的mat默认是不强制identity的，要用这个宏，太容易造成错误了！manual里居然没有提。
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<ThirdParty/imgui/imgui.h>
#include<c2DefEvent.h>
#include<c2Application.h>
#include<c2PartController/c2PartCamera.h>//FIXME
#include<c2PartVisual/c2PartLight.h>
#include<Render/Render.h>
#include<c2PartVoxelSpace/VoxelOctree.h>

#include<GLFW/glfw3.h>//FIXME: git rid

//1/////////////////////////////////////////////////////////////////////////////
//#define USE_INTEGER

////////////////////////////////////////////////////////////////////////////////
static int			g_nWindWidth = 1680, g_nWindHeight = 1050;
static Camera		camera;
static Render		g_Render(camera, g_nWindWidth, g_nWindHeight);
static const c2VNode*	g_pVoxelSpaceRoot= nullptr;
static c2PartLight	g_Light;

/*view control*/
static float		lastX = 0;
static float		lastY = 0;
static bool			g_bDirtyfirstMouse = true;

static void UpdateFixFrameFun(GLFWwindow *pWnd, const double dElapsed, const Uint64 nFixFrameStamp) {
}
extern int g_lbd_drawcounter;
static void DrawFun(GLFWwindow *pWnd, const double dElapsed, const Uint64 nFixFrameStamp) {
	ImGui::Begin("C2 Director");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Camera Pos:\tx=%.3f\ty=%.3f\tz=%.3f",
		camera.Position[0], camera.Position[1], camera.Position[2]);
	ImGui::End();
	/*--------------------------------------------------------------------*/
	/*Camera keyboard control*/
	float artifact_delta = static_cast<float>(dElapsed);
	if (glfwGetKey(pWnd, GLFW_KEY_LEFT_SHIFT))
		artifact_delta *= 5.0f;
	if (glfwGetKey(pWnd, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, artifact_delta);
	if (glfwGetKey(pWnd, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, artifact_delta);
	if (glfwGetKey(pWnd, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, artifact_delta);
	if (glfwGetKey(pWnd, GLFW_KEY_F) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, artifact_delta);
	if (glfwGetKey(pWnd, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, artifact_delta);
	if (glfwGetKey(pWnd, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, artifact_delta);
	g_Render.update(dElapsed);
	/*--------------------------------------------------------------------*/
	/*glfwGetFramebufferSize（window，＆width，＆height); TODO: to used in onSize*/
	glViewport(0, 0, g_nWindWidth, g_nWindHeight);//FIXME:放到framebuffer resize的地方
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (g_pVoxelSpaceRoot)	g_pVoxelSpaceRoot->draw(g_Render);
	g_Light.draw(g_Render);

//	std::cout << "g_lbd_drawcounter: " << g_lbd_drawcounter << std::endl;
	g_lbd_drawcounter = 0;
}

////////////////////////////////////////////////////////////////////////////////
class onSysInitializedVoxel : public c2IAction {
	virtual Status update() {
		c2SetDrawCallback(DrawFun);
		c2SetUpdateFixFrameCallback(UpdateFixFrameFun);

		int mx, my, mz;
		const c2VNode* plut= c2MakeVoxelLUTFromImage(mx, my, mz, "d:/qjf2017.png");
		g_pVoxelSpaceRoot = c2BuildVoxelOctree(plut, mx, my, mz);

		const c2SysEvt::initialized& evt = *(static_cast<const c2SysEvt::initialized*>(_pEvt));
		glfwSetInputMode(evt._pWnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glEnable(GL_DEPTH_TEST);
		BOOST_LOG_TRIVIAL(info) << "C2engine initialized.";
#ifdef TEST_EFFICIENT
		void _set_shader(const Render &Rr);
//		_set_shader(g_Render);
#endif
		return Status::Success;
	}
};

/*TODO: onTerminin free everything.*/
class onTerminateVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onTerminate";
		return Status::Success;
	}
};


////////////////////////////////////////////////////////////////////////////////
class onMouseButtonVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onMouseButton";
		return Status::Success;
	}
};

class onCursorMovedVoxel : public c2IAction {
public:
	virtual Status update() {
		const c2SysEvt::cursor_moved& evt = *(static_cast<const c2SysEvt::cursor_moved*>(_pEvt));
		if (g_bDirtyfirstMouse) {
			lastX = evt._x;
			lastY = evt._y;
			g_bDirtyfirstMouse = false;
		}
		float xoffset = evt._x - lastX;
		float yoffset = lastY - evt._y; // reversed since y-coordinates go from bottom to top
		lastX = evt._x;
		lastY = evt._y;
		camera.ProcessMouseMovement(xoffset, yoffset);
		return Status::Success;
	}
};

class onCursorEnterVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCursorEnter";
		return Status::Success;
	}
};

class onScrolledVoxel : public c2IAction {
public:
	virtual Status update() {
		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onScrolled";
		return Status::Success;
	}
};

static const float cameraSpeed = .05;
static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
class onKeyVoxel : public c2IAction {
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
		return Status::Success;
	}
};

class onCharInputVoxel : public c2IAction {
public:
	virtual Status update() {
		//		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCharInput";
		return Status::Success;
	}
};

class onCharModsInputVoxel : public c2IAction {
public:
	virtual Status update() {
		//		BOOST_LOG_TRIVIAL(info) << "||||||||||||||||||||  onCharModsInput";
		return Status::Success;
	}
};


////////////////////////////////////////////////////////////////////////////////
int main_voxel() {
	Uint32 syset_chunkoffet = 0;

	onSysInitializedVoxel initialized;
	c2asActSubEvt(initialized, syset_chunkoffet + c2SysEvtType::initialized,
		sizeof(c2SysEvt::initialized));
	onTerminateVoxel terminate;
	c2asActSubEvt(terminate, syset_chunkoffet + c2SysEvtType::terminate,
		sizeof(c2SysEvt::terminate));

	onMouseButtonVoxel mouse_button;
	c2asActSubEvt(mouse_button, syset_chunkoffet + c2SysEvtType::mouse_button,
		sizeof(c2SysEvt::mouse_button));
	onCursorMovedVoxel cursor_moved;
	c2asActSubEvt(cursor_moved, syset_chunkoffet + c2SysEvtType::cursor_moved,
		sizeof(c2SysEvt::cursor_moved));
	onCursorEnterVoxel cursor_enter;
	c2asActSubEvt(cursor_enter, syset_chunkoffet + c2SysEvtType::cursor_enter,
		sizeof(c2SysEvt::cursor_enter));
	onScrolledVoxel scrolled;
	c2asActSubEvt(scrolled, syset_chunkoffet + c2SysEvtType::scrolled,
		sizeof(c2SysEvt::scrolled));
	onKeyVoxel key;
	c2asActSubEvt(key, syset_chunkoffet + c2SysEvtType::key,
		sizeof(c2SysEvt::key));
	onCharInputVoxel char_input;
	c2asActSubEvt(char_input, syset_chunkoffet + c2SysEvtType::char_input,
		sizeof(c2SysEvt::char_input));
	onCharModsInputVoxel charmods_input;
	c2asActSubEvt(charmods_input, syset_chunkoffet + c2SysEvtType::charmods_input,
		sizeof(c2SysEvt::charmods_input));

	/**************************************************************************/
	c2AppRun(1, g_nWindWidth, g_nWindHeight, "C2engine.Creator", false);

	return 0;
}
