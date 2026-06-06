#version 330 core

in vec2 vTextureCoords;

uniform sampler2D uGuiTexture;
uniform sampler2D uAlphabetTexture;

uniform int uObjectTypeId = 0; //0 means we are rendering a control's background
							   //1 means we are rendering a character to the screen


layout(location = 0) out vec4 fragColor;

void main()
{
	if(uObjectTypeId == 0)
	{
		fragColor = texture(uGuiTexture,vTextureCoords);
	}
	else
	{
		fragColor = texture(uAlphabetTexture,vTextureCoords);
	}
	
}