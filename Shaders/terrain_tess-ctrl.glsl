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

//Tessellation control shader
//NOT USED

#version 430 core

#include <Shaders/uniforms_h.glsl>

layout (vertices = 4) out;

void main()
{
	gl_TessLevelInner[0] = 200.0;
    gl_TessLevelInner[1] = 200.0;
    gl_TessLevelOuter[0] = 200.0;
    gl_TessLevelOuter[1] = 200.0;
    gl_TessLevelOuter[2] = 200.0;
    gl_TessLevelOuter[3] = 200.0;

    gl_out[gl_InvocationID].gl_Position	= normalize(gl_in[gl_InvocationID].gl_Position);
	
	//gl_out[gl_InvocationID].gl_Position = gl_out[gl_InvocationID].gl_Position / 100.0;
}