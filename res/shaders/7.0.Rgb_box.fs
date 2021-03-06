#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;
in vec4 ourColor;
void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = ourColor*mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.00);
}