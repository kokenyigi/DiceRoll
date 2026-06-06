#version 330 core
        
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTextureCoords;

uniform mat4 uScreenTransform;

uniform vec2 uTextureOffset;
uniform vec2 uTextureScale;

out vec2 vTextureCoords;

void main()
{
    gl_Position = uScreenTransform * vec4(aPosition,0,1); //valami
    vTextureCoords = uTextureScale * aTextureCoords + uTextureOffset;
}