#version 330 core
        
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aNumberTexCoord;
layout(location = 3) in vec2 aMaterialTexCoord;



uniform mat4 projectionTransform;
uniform mat4 viewTransform;

uniform mat4 uWorldTransform = mat4(1.0f);
uniform mat4 uInverseTransposedWorldTransform = mat4(1.0f);

uniform mat4 uLightSpaceMatrix;


out vec3 vPosition;
out vec3 vNormal;
out vec2 vNumberTexCoord;
out vec2 vMaterialTexCoord;

out vec4 vLightSpacePosition;


        
void main()
{
    gl_Position = projectionTransform * viewTransform * uWorldTransform * vec4(aPosition.xyz,1.0);

    vPosition = vec3(uWorldTransform * vec4(aPosition,1));
    vNormal = vec3( uInverseTransposedWorldTransform * vec4(aNormal,0));
    vNumberTexCoord = aNumberTexCoord;
    vMaterialTexCoord = aMaterialTexCoord;

    vLightSpacePosition = uLightSpaceMatrix * uWorldTransform * vec4(aPosition,1);
};