#version 400 core

in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction;
	float exponent;
	float cutoff;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1; 
uniform LightInfo light2; 
uniform LightInfo light3; 
uniform LightInfo light4; 
uniform LightInfo light5; 
uniform LightInfo light6; 
uniform LightInfo light7; 
uniform LightInfo light8;
uniform LightInfo light9;
uniform LightInfo light10;
uniform LightInfo light11; 
uniform LightInfo light12; 
uniform LightInfo light13; 
uniform LightInfo light14; 
uniform LightInfo light15; 
uniform LightInfo light16; 
uniform LightInfo light17; 
uniform LightInfo light18;
uniform LightInfo light19;
uniform LightInfo light20;
uniform LightInfo light21;
uniform MaterialInfo material1; 


uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
in vec3 worldPosition;

in vec4 p;
in vec3 n;

vec3 BlinnPhongSpotlightModel(LightInfo light, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light.position - p));
	float angle = acos(dot(-s, light.direction));
	float cutoff = radians(clamp(light.cutoff, 0.0, 90.0));
	vec3 ambient = light.La * material1.Ma;
	if (angle < cutoff) {
		float spotFactor = pow(dot(-s, light.direction), light.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = light.Ld * material1.Md * sDotN;
		vec3 specular = vec3(0.0);
		if (sDotN > 0.0)
			specular = light.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
		return ambient + spotFactor * (diffuse + specular);
		 
		vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
		return red;
	} else 
		return ambient;
}

// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.

vec3 PhongModel(LightInfo light1,vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	
	return ambient + diffuse + specular;

}

in vec3 blinkColor;
out vec4 blinkOutColor;

float rho = 0.0009f;
vec3 fogColour = vec3(0.75f);

void main()
{

	vec3 vColour = PhongModel (light1,p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light2, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light3, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light4, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light5, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light6, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light7, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light8, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light9, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light10, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light11, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light12, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light13, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light14, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light15, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light16, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light17, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light18, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light19, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light20, p, normalize(n));
	vColour += BlinnPhongSpotlightModel (light21, p, normalize(n));

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);
		//blinkOutColor = vec4(vColour, 1.0);

	} else {

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture)
			vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture 
		else
			vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
	}
	
    float d = length(p.xyz);
    float w = exp(-rho*d);
    vOutputColour.rgb = mix(fogColour, vOutputColour.rgb, w);
}
