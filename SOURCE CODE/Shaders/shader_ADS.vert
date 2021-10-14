#version 440 core
layout (location = 0) in vec3 Position; // vertex positions
layout (location = 1) in vec2 texCoord;	// tex coords
layout (location = 2) in vec3 normal;	// vertex normals

 						
out vec2 textureCoordinate;
out vec3 normals;
out vec3 fragmentPosition;
out vec3 lightColour;
out vec3 lightPosition;
out vec3 viewPosition;
out float time;
out vec3 vPosition;

uniform mat4 uNormalMatrix;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uLightColour;
uniform vec3 uAmbientIntensity;
uniform vec3 lightCol;
uniform vec3 uLightPosition;
uniform vec3 uViewPosition;
uniform float uTime;

// This shader is largely unchanged from Prof' Doughty's implementation.

void main()
{
	gl_Position = uProjection * uView * uModel * vec4(Position.x, Position.y, Position.z, 1.0);
	
	textureCoordinate = vec2(texCoord.x, 1 - texCoord.y);
	
	//get the fragment position in world coordinates as this is where the lighting will be calculated
	fragmentPosition = vec3(uModel * vec4(Position, 1.0f));
	
	//pass normals to fragment shader after modifying for scaling
	//calculate a 'normal matrix' and multiply by the unmodified normal
	normals = mat3(uNormalMatrix) * normal;
	
	lightColour = uLightColour;
	lightPosition = uLightPosition;
	viewPosition = uViewPosition;
	time = uTime;
	
	// ***Only change to the vert shader.***
	// While lighting is calculated in relation to the world, certain on-model effects (equatorial glowing cracks)
	// require positions of the fragment in relation to the model only.
	// See the '
	vPosition = Position;
}