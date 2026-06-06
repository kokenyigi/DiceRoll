#version 330 core
        
layout(location = 0) in vec3 aPosition;

uniform mat4 uWorldTransform = mat4(1.0f);
uniform mat4 uLightSpaceTransform = mat4(1.0f);

void main()
{
    gl_Position = uLightSpaceTransform * uWorldTransform * vec4(aPosition,1.0);
}