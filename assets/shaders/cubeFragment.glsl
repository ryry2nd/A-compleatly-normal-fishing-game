#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

in vec3 fragWorldPos;

uniform float u_CullRadius;

uniform sampler2D texture1;

void main()
{

    if (length(fragWorldPos) < u_CullRadius) discard;

    FragColor = texture(texture1, TexCoord);
}
