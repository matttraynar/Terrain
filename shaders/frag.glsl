#version 330

in vec3 out_pos;
in vec3 out_norm;

uniform mat4 M;
uniform vec4 mCol;
uniform vec3 camPos;
uniform vec3 lightPos;

uniform float min;
uniform float range;

out vec4 fragColour;

void main()
{
	vec4 lCol = vec4(1.0, 1.0, 1.0, 1.0);
//	vec3 lPosA = vec3(0,10,0);
	vec3 lPosA = lightPos;

	vec4 colour = mCol;

	float colourHeight = (out_pos.y - min) / range;

	if(mCol.w == 0.0f)
	{
		colour = vec4(1.0 - colourHeight, colourHeight, colourHeight, 1.0f);

		vec3 v_pos = vec4(M * vec4(out_pos,1.0)).xyz;
		vec3 c_pos = vec4(M * vec4(camPos,1.0)).xyz;

		float brightness = clamp(dot(out_norm,normalize(lightPos)), 0.0, 1.0);

		fragColour = vec4(brightness * lCol.rgb * colour.rgb, 1.0);
	}
	else
	{
		fragColour = mCol;
	}
}
