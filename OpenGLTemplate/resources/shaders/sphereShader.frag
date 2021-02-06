#version 400 core

in vec3 vColour;			// Note: colour is smoothly interpolated (default)
out vec4 vOutputColour;

in float fIntensity;

out vec4 fragColour;

void main()
{	
	// vOutputColour = vec4(vColour, 1.0);

	if (fIntensity > 0.75)
		fragColour = vec4(0.0, 1.0, 0.0, 1.0);  //alpha is 0.5!!!
	else if (fIntensity > 0.5)
		fragColour = vec4(0.6, 0.6, 0.0, 0.0);
	else
		fragColour = vec4(0.0, 1.0, 0.0, 1.0);

}
