#include <Shaders/uniforms_h.glsl>
#include <Shaders/utilities.glsl>

const float worldScale = 6378.1 * 1000.0;
const vec3 worldPosition = vec3(0.0f, -6378.1 * 1000.0, 0.0f);

layout(std140) uniform UniformOceanBlock
{
	mat4 projection;
	mat4 inverseView;
	mat4 inverseProj;
};

bool intersectRayWorld(vec3 o, vec3 d, vec3 center, float r, out float t, out vec3 q)
{
	vec3 m = o - center;
	
	float b = dot(m, d);
	float c = dot(m, m) - r * r;
	
	if (c > 0.0 && b > 0.0)
		return false;
		
	float discr = b * b - c;
	
	if (discr < 0.0)
		return false;
		
	t = -b - sqrt(discr);
	
	if (t < 0.0)
		t = 0.0;
		
	q = o + t * d;
	
	return true;
}