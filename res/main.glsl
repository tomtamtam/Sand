#define vertex

#version 330 core

layout (location = 0) in vec2 a_Position;

uniform mat4 u_Proj;
uniform vec2 u_Size;

out vec4 v_Color;
out vec2 v_UV;

void main()
{
    v_UV = a_Position;
    v_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    gl_Position = u_Proj *  vec4(a_Position * u_Size, 0.0f, 1.0f);
}

#define fragment

#version 330 core

in vec4 v_Color;
in vec2 v_UV;

uniform sampler2D u_Texture;

out vec4 FragColor;

void main()
{
    FragColor = texture(u_Texture, v_UV) * v_Color;
}
