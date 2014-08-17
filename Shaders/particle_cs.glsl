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