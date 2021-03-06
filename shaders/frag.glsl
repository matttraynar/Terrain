#version 330

in vec3 out_pos;
in vec3 out_norm;
in vec2 out_uv;

uniform mat4 M;
uniform vec4 mCol;
uniform vec3 camPos;
uniform vec3 lightPos;

uniform float min;
uniform float range;

uniform float waterLevel;

uniform sampler2D sandTexture;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;

uniform bool useShading;
uniform bool useHeightmap;

out vec4 fragColour;

void main()
{
	bool useTextures = true;

	vec4 lCol = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 lPosA = lightPos;

	vec4 colour = mCol;

	float colourHeight = (out_pos.y - min) / range;

	if(useTextures)
	{
		if(mCol.w == 0.0f)
		{
			float texHeight1 = 0.15f;
			float texHeight2 = 0.3f;
			float texHeight3 = 0.65f;
			float texHeight4 = 0.85f;

			float waterHeight = (waterLevel - min) / range;

//			if(colourHeight < waterHeight * 0.7f)
//			{
//				colour = vec4(0.0f, 0.0f, 0.2f, 1.0f);
//			}
//			else if(colourHeight < waterHeight * 1.0f)
//			{
//				float start = waterHeight * 0.7f;

//				float colourRange = waterHeight * 0.3f;

//				float colourScale = (colourHeight - start) / colourRange;

//				colour = vec4(0.0f, 0.0f, 0.2f, 1.0f) * (1 - colourScale);
//				colour += texture2D(sandTexture, out_uv) * colourScale;
//			}
//			else if(colourHeight < waterHeight * 1.1f)
//			{
//				colour = texture2D(sandTexture, out_uv);
//			}
//			else if(colourHeight < waterHeight * 1.2f)
//			{
//				float start = waterHeight * 1.1f;

//				float colourRange = waterHeight * 0.1f;

//				float colourScale = (colourHeight - start) / colourRange;

//				colour = texture2D(sandTexture, out_uv) * (1 - colourScale);
//				colour += texture2D(grassTexture, out_uv) * colourScale;
//			}

			if(colourHeight < waterHeight * 2.0f)
			{
				colour = texture2D(grassTexture, out_uv);
			}
			else if(colourHeight < waterHeight * 2.2f)
			{
				float start = waterHeight * 2.0f;

				float colourRange = waterHeight * 0.2f;

				float colourScale = (colourHeight - start) / colourRange;

				colour = texture2D(grassTexture, out_uv) * (1 - colourScale);
				colour += texture2D(rockTexture, out_uv) * colourScale;
			}
			else if(colourHeight < waterHeight * 2.9f)
			{
				colour = texture2D(rockTexture, out_uv);
			}
			else if(colourHeight < waterHeight * 3.1f)
			{
				float start = waterHeight * 2.9f;

				float colourRange = waterHeight * 0.2f;

				float colourScale = (colourHeight - start) / colourRange;

				colour = texture2D(rockTexture, out_uv) * (1 - colourScale);
				colour += texture2D(snowTexture, out_uv) * (colourScale);
			}
			else
			{
				colour = texture2D(snowTexture, out_uv);
			}

			if(out_norm.y < 0.7f)
			{
				colour = texture2D(rockTexture, out_uv);
			}
			else if(out_norm.y < 0.75f)
			{
				float start = 0.7f;

				float colourRange = 0.05f;

				float colourScale = (out_norm.y - start) / colourRange;

				vec4 colourCopy = colour;

				colour = texture2D(rockTexture, out_uv) * (1.0f - colourScale);
				colour += colourCopy * ( colourScale);
			}

			vec3 v_pos = vec4(M * vec4(out_pos,1.0)).xyz;
			vec3 c_pos = vec4(M * vec4(camPos,1.0)).xyz;

			float brightness = clamp(dot(out_norm,normalize(lightPos)), 0.0, 1.0);

			if(useShading)
			{
				fragColour = vec4(brightness * lCol.rgb * colour.rgb, 1.0);
			}
			else
			{
				fragColour = colour;
			}
		}
		else
		{
			float brightness = 1.0f;
			if(length(out_norm) > 0)
			{
				brightness = clamp(dot(out_norm,normalize(lPosA)), 0.0, 1.0);
			}

			if(useShading)
			{
				fragColour = vec4(brightness * lCol.rgb * mCol.rgb, 1.0);
			}
			else if(useHeightmap)
			{
				fragColour = vec4(colourHeight, colourHeight, colourHeight, 1.0);
			}
			else
			{
				fragColour = vec4(mCol.rgb, 1.0f);
			}
		}
	}
	else
	{
		if(mCol.w == 0.0f)
		{
			colour = vec4(colourHeight, colourHeight, 0.0f, 1.0f);

			if(waterLevel > 0.0f)
			{
				if((out_pos.y >= (waterLevel * 0.9f)) && ((out_pos.y <= (waterLevel * 1.1f))))
				{
					colour = vec4(0.91f, 0.88f, 0.46f, 1.0f);
				}
			}
			else
			{
				if((out_pos.y <= (waterLevel * 0.9f)) && ((out_pos.y >= (waterLevel * 1.1f))))
				{
					colour = vec4(0.91f, 0.88f, 0.46f, 1.0f);
				}
			}

			vec3 v_pos = vec4(M * vec4(out_pos,1.0)).xyz;
			vec3 c_pos = vec4(M * vec4(camPos,1.0)).xyz;

			float brightness = clamp(dot(out_norm,normalize(lightPos)), 0.0, 1.0);

			fragColour = vec4(brightness * lCol.rgb * colour.rgb, 1.0);
		}
		else
		{
			float brightness = clamp(dot(out_norm,normalize(lPosA)), 0.0, 1.0);

			fragColour = vec4(brightness * lCol.rgb * mCol.rgb, 1.0);
		}
	}

}
