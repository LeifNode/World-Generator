#include <Shaders/uniforms_h.glsl>
#include <Shaders/utilities.glsl>

layout(std140) uniform UniformAtmosphereBlock
{
	vec3 v3InvWavelength;	// 1 / pow(wavelength, 4) for the red, green, and blue channels
	float fOuterRadius;		// The outer (atmosphere) radius
	float fOuterRadius2;	// fOuterRadius^2
	float fInnerRadius;		// The inner (planetary) radius
	float fInnerRadius2;	// fInnerRadius^2
	float fKrESun;			// Kr * ESun
	float fKmESun;			// Km * ESun
	float fKr4PI;			// Kr * 4 * PI
	float fKm4PI;			// Km * 4 * PI
	float fScale;			// 1 / (fOuterRadius - fInnerRadius)
	float fScaleDepth;		// The scale depth (i.e. the altitude at which the atmosphere's average density is found)
	float fScaleOverScaleDepth;	// fScale / fScaleDepth
	float g;
	float g2;
};

float scale(float fCos)
{
	float x = 1.0 - fCos;
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}