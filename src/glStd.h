#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp> 
#include <glm\gtc\quaternion.hpp>

#include <vector>
#include <string>
#include <map>
#include <list>
#include <iostream>
#include <ctime>
#include <cassert>
#include <memory>
#include <algorithm>

#include "MathUtils.h"

using std::shared_ptr;
using std::weak_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::dvec2;
using glm::dvec3;
using glm::dvec4;
using glm::mat3;
using glm::mat4;

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }