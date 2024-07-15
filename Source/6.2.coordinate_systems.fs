#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec4 vertexColorOut;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

vec2 pitch  = vec2(25, 25);

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	// FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);

	/*if (int(mod(gl_FragCoord.x, pitch[0])) == 0 || int(mod(gl_FragCoord.y, pitch[1])) == 0)
    {
        FragColor = vec4(0.7, 0.3, 0.5, 1.0);
    } else {
        FragColor = vec4(0);
    }*/

	FragColor = vertexColorOut;
}
