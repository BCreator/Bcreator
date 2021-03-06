#pragma once

#include"c2PreDefined.h"

//FIXME: 以下宏暂时定义注释于此，以后会用更现代化的方式来做这方面的事。把这个宏放在这里
//只是为了便于后面以此宏为线索清除和修改相关的代码。
//#define C2_CHECK_MEM

////////////////////////////////////////////////////////////////////////////////
/*Driving framework of the whole application*/
C2API void c2AppRun(int SwapInterval, int nWndWidth, int nWndHeight,
						const char *sWndCaption, bool isBlocked = true);
class GLFWwindow;
typedef void(*c2FrameFun)(GLFWwindow* pWindow, const double dElapsed, Uint64 nFixFrameStamp);
C2API void c2SetDrawCallback(c2FrameFun DrawFun);
C2API void c2SetUpdateFixFrameCallback(c2FrameFun DrawFun);

////////////////////////////////////////////////////////////////////////////////
/*Part & Factory*/
#include"./c2Foundation/c2Part.h"
using c2APart = c2Part::ARPart;
C2API c2APart c2CreatePart(const char *sClass, const char *sName = nullptr);
C2API bool _c2RegistPartClass(const char *sClass, c2Part::CreationFunc C);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create);

////////////////////////////////////////////////////////////////////////////////
#if (defined(__APPLE__) && TARGET_OS_IOS) 
#	include<glad/glad.h>
#	define GLFW_INCLUDE_NONE
#	define C2_USE_OPENGLES
#elif (defined(__ANDROID__)) || (defined(__EMSCRIPTEN__)) 
#	include<glad/glad.h>
#	define GLFW_INCLUDE_NONE	//FIXME: git rid
#	define C2_USE_OPENGLES

#else//XXX: 不记得写下面代码的时候为何同imgui的宏有关系，为了同imgui保持统一么？

#if !defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)     \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)     \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)     \
 && !defined(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
#define IMGUI_IMPL_OPENGL_LOADER_GL3W
#endif

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#endif
