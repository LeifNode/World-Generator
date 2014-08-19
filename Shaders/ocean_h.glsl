//The MIT License (MIT)
//
//Copyright (c) 2014 Leif Erkenbrach
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

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