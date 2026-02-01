#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;

void main()
{
    vec4 base = texture(u_Texture1, v_TexCoord);
    vec4 overlay = texture(u_Texture2, v_TexCoord);

    FragColor = mix(base, overlay, overlay.a);
}