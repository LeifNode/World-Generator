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

#version 430 core

//From OpenGL programming guide 8th ed

layout (local_size_x = 512) in;

layout (rgba32f, binding = 0) uniform imageBuffer velocity_buffer;
layout (rgba32f, binding = 1) uniform imageBuffer position_buffer;

uniform float dt = 1.0f;

struct LeapControl
{
	vec4 position;
	vec3 velocity;
	float pad;
};

uniform AttractorUniformBlock
{
	vec4 attractors[4];
	LeapControl fingers[10];
};

void main()
{
	vec4 vel = imageLoad(velocity_buffer, int(gl_GlobalInvocationID.x));
    vec4 pos = imageLoad(position_buffer, int(gl_GlobalInvocationID.x));

    int i;

    pos.xyz += vel.xyz * dt;

    for (i = 0; i < 4; i++)
    {
        vec3 dist = (attractors[i].xyz - pos.xyz);
		float distanceSquared = dot(dist, dist);
        vel.xyz += dt * dt * (attractors[i].w * (dist /distanceSquared)  / (distanceSquared * 0.2 + 10.0));
    }
	
	//Add force based on nearby finger tips detected by LEAP motion controller
	for (i = 0; i < 10; i++)
	{
		vec3 dist = (fingers[i].position.xyz - pos.xyz);
		float distanceSquared = dot(dist, dist);
        vel.xyz += dt * dt * ((fingers[i].velocity * 1000.0f) / (distanceSquared + 1.0));
	}
	
    //if (pos.w <= 0.0)
    //{
    //    pos.xyz = -pos.xyz * 0.01;
    //    vel.xyz *= 0.01;
    //    pos.w += 1.0f;
    //}
	
	//Clamp positions within a box
	if (pos.x > 200.0f)
		pos.x = -200.0f;
	if (pos.x < -200.0f)
		pos.x = 200.0f;
		
	if (pos.y > 150.0f)
		pos.y = 0.0f;
	if (pos.y < 0.0f)
		pos.y = 150.0f;
		
	if (pos.z > 200.0f)
		pos.z = -200.0f;
	if (pos.z < -200.0f)
		pos.z = 200.0f;

	vel *= pow(0.9, dt);	
		
    imageStore(position_buffer, int(gl_GlobalInvocationID.x), pos);
    imageStore(velocity_buffer, int(gl_GlobalInvocationID.x), vel);
}