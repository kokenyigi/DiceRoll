#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vNumberTexCoord;
in vec2 vMaterialTexCoord;
in vec4 vLightSpacePosition;


// 0 - Entity
// 1 - Object
// 2 - LightBulb
uniform int uObjectTypeId = 0; 

uniform sampler2D uNumberTexture;
uniform sampler2D uMaterialTexture;

uniform sampler2D uShadowMapTexture;

//Camera Data
uniform vec3 uCameraPosition = vec3(0,0,0);

//SpotLight Data
uniform vec3 uSpotLightPosition = vec3(0,10,0);
uniform vec3 uSpotLightDirection = vec3(0,-1,0); //Note: implement this on CPU side with uniforms
uniform float uSpotLightInnerCos = 0.2f;
uniform float uSpotLightOuterCos = 0.4f;

//Ambient Light Data
float ambientStrength = 0.9f;
vec3 ambientColor = vec3(0.1f,0.1f,0.1f);

//Diffuse Light Data
vec3 diffuseColor = vec3(0.8f,0.8f,0.6f);

//Specular Light Data
float specularShininess = 0.9f;
vec3 specularColor = vec3(0.8,0.8,0.8);
int specularPower = 32;





float CalculateShadowFactor()
{
    vec3 projFragLightSpacePos = vLightSpacePosition.xyz / vLightSpacePosition.w;

    vec2 fragSamplingCoords;
    fragSamplingCoords.x = projFragLightSpacePos.x * 0.5 + 0.5;
    fragSamplingCoords.y = projFragLightSpacePos.y * 0.5 + 0.5;
    float fragSamplingDepth = projFragLightSpacePos.z*0.5+0.5;

    float actualDepthValue = texture(uShadowMapTexture,fragSamplingCoords).r; // we only keep the first value in the texture(red)
                                                                              //Because we know it only contains one(depth value)
    float returnShadow;
    float bias = 0.0001;
    if(actualDepthValue + bias < fragSamplingDepth)
    {
        returnShadow =  0.9;
    }
    else
    {
        returnShadow = 0.0;
    }

    return returnShadow;
}




layout(location = 0) out vec4 fragcolor;
        
void main()
{
    
    //Ambiant Light Calculation
    vec3 ambientLight = ambientStrength * ambientColor ;

    
    vec3 fragNormal = normalize(vNormal);
    vec3 toLight =normalize(uSpotLightPosition - vPosition);
    vec3 fromLight = -toLight;

    

    //Quick Spotlight calculations
    float spotLightFactor = 0.0f;
    float fragToSpotLightCos = dot(fromLight,uSpotLightDirection);
    if(fragToSpotLightCos > uSpotLightInnerCos)
    {
        spotLightFactor = 1.0f;
        //fragcolor = vec4(0.4f,0,0.2f,1.0);
        //return;
        
    }
    else
    {
        float t = (uSpotLightInnerCos - fragToSpotLightCos) / (uSpotLightInnerCos-uSpotLightOuterCos);
        spotLightFactor = mix(1.0,0,t);
    }
    
    //Diffuse Light Calculation
    float diffuseStrength = spotLightFactor * max(dot(fragNormal,toLight),0); 
    vec3 diffuseLight = diffuseStrength * diffuseColor;

    //Specular Light Calculation
    vec3 toCamera = normalize(uCameraPosition - vPosition);
    vec3 reflectDirection = reflect(uSpotLightDirection,fragNormal);
    float specularStrength = pow(max(dot(reflectDirection,toCamera),0),specularPower);
    vec3 specularLight = spotLightFactor * specularStrength * specularShininess * specularColor;

    float shadowFactor = CalculateShadowFactor();

    vec4 shadedColor = vec4(ambientLight + (1.0-shadowFactor)*(diffuseLight + specularLight),1.0);



    if(uObjectTypeId == 0)
    {

        vec4 numberTexture = texture(uNumberTexture,vNumberTexCoord);
        vec4 materialTexture = texture(uMaterialTexture,vMaterialTexCoord);    
    
        if(numberTexture.w <=0.001)
        {
            fragcolor = shadedColor * materialTexture;
        }
        else
        {
            fragcolor = shadedColor * numberTexture;
        }

        return;
    }
    else if(uObjectTypeId == 1)
    {
        vec4 materialTexture = texture(uMaterialTexture,vMaterialTexCoord);
        fragcolor = shadedColor *  materialTexture;

        return;
    }

    fragcolor = vec4(0.7f,0.0f,0.2f,1.0f);
    return;
    
};