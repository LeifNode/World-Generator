#include <Shaders/grass_h.glsl>

layout (location = 0) in vec3 position;

void main()
{
	gl_Position = vec4(position, 1.0);
}