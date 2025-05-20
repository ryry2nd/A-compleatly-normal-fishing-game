#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 fragWorldPos;

uniform mat4 uMVP;

void main()
{


    gl_Position = uMVP * vec4(aPos, 1.0);
    fragWorldPos = gl_Position.xyz;

    TexCoord = aTexCoord;
}
