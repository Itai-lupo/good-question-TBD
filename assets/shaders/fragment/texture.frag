#version 460 core

out layout(location = 0) vec4 fragColor;
in layout(location = 0) vec2 TexCoord;

uniform layout(binding = 0) sampler2D ourTexture;

void main()
{
    fragColor = texture(ourTexture, TexCoord);
    // fragColor = vec4(TexCoord.x, TexCoord.y, 0, 1);
}
