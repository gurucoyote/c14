/**
 * @file shinyV.glsl
 *
 * $LicenseInfo:firstyear=2007&license=viewerlgpl$
 * $/LicenseInfo$
 */
 
#version 120

vec4 calcLighting(vec3 pos, vec3 norm, vec4 color, vec4 baseCol);

void calcAtmospherics(vec3 inPositionEye);

uniform vec4 origin;

void main()
{
	//transform vertex
	gl_Position = ftransform();
	
	vec4 pos = (gl_ModelViewMatrix * gl_Vertex);
	vec3 norm = normalize(gl_NormalMatrix * gl_Normal);
	vec3 ref = reflect(pos.xyz, -norm);

	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_TextureMatrix[1]*vec4(ref,1.0);

	calcAtmospherics(pos.xyz);

	gl_FrontColor = calcLighting(pos.xyz, norm, gl_Color, vec4(0.0));

	gl_FogFragCoord = pos.z;
}
