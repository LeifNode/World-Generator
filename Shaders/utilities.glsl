#include <Shaders/uniforms_h.glsl>

void logDepthFunction()
{
	gl_Position.z = 2.0*log(gl_Position.w/perFrame.nearClippingPlane)/log(perFrame.farClippingPlane/perFrame.nearClippingPlane) - 1; 
    gl_Position.z *= gl_Position.w;
}