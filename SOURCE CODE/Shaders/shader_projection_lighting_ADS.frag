#version 440 core
out vec4 vertColour;
in vec2 textureCoordinate;
in vec3 normals;
in vec3 fragmentPosition;
in vec3 lightColour;
in vec3 lightPosition;
in vec3 viewPosition;
in vec3 vPosition;

uniform sampler2D brickTex;

void main()
{
	//diffuse component (Unchanged from Prof' Doughty's Implementation)
	vec3 nNormal = normalize(normals);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
	float diffuseStrength = max(dot(nNormal, lightDirection), 0.1f);
	vec3 objectDiffuseReflectionCoeff = vec3(1.0f, 1.0f, 1.0f);
    vec3 diffuse = (diffuseStrength * objectDiffuseReflectionCoeff) * lightColour;
	
	// Sets the RGB colour to correspond to the vertex's XYZ
	vec3 funny = normalize(vec3(vPosition.x, vPosition.y, vPosition.z));
	
	// Output
	vertColour = (vec4(diffuse,1.0f) * vec4(funny, 1.0f));
}

// This shader file exists to satisfy the following requirement: "...multiple meshes" & "A directional light source to light the objects."
// Essentially, this sgader enables the presence of the small cube mesh and nothing else.