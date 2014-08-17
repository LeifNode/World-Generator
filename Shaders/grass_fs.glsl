#include <Shaders/grass_h.glsl>

uniform sampler2D grassTex;

in GS_Interface
{
	vec4 PositionW;
	vec2 TexCoord;
	float YellowingFactor;
	float DarknessFactor;
} gsOut;

out vec4 color;

const vec3 orange = vec3(1.0, 0.647, 0.0);

void main()
{
	vec4 sampledColor = texture(grassTex, gsOut.TexCoord);
	
	if (sampledColor.a <= 0.5)
		discard;
	
	color.xyz = mix(sampledColor.xyz, orange, max(0.0, gsOut.YellowingFactor - 0.3) * 0.2) * gsOut.DarknessFactor;
	color.a = sampledColor.a;
}