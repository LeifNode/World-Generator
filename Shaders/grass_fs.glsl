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