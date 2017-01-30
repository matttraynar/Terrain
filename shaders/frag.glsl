#version 330

in vec3 out_pos;
in vec3 out_norm;

uniform mat4 M;
uniform vec4 mCol;
uniform vec3 camPos;

out vec4 fragColour;

void main()
{
	vec4 lCol = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 lPosA = vec3(0,0,1);

	vec4 colour = mCol;

	if(mCol.w == 0.0f)
	{
		colour = vec4(out_norm.z / 2.0f, out_norm.z, 0.2f, 1.0f);
	}

    vec3 v_pos = vec4(M * vec4(out_pos,1.0)).xyz;
	vec3 c_pos = vec4(M * vec4(camPos,1.0)).xyz;

	float brightness = clamp(dot(out_norm,normalize(lPosA)), 0.0, 1.0);

//	fragColour = vec4(brightness * lCol.rgb * mCol.rgb, 1.0);
	fragColour = vec4(brightness * lCol.rgb * colour.rgb, 1.0);

}
