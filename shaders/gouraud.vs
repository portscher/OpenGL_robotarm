#version 330

// Based on shaders of David Oberhollenzer

// THIS IS A VERTEX SHADER

// Uniform input
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

uniform vec3 LightPosition;
uniform vec3 LightColor;

uniform float DiffuseFactor;
uniform float SpecularFactor;
uniform float AmbientFactor;

// Content of the vertex data
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 Normal;

// Output sent to the fragment Shader
out vec4 vColor;

// Main function
void main()
{

    // VECTORS

    // Compute modelview matrix
    mat4 modelViewMatrix = ViewMatrix * ModelMatrix;
    mat4 modelViewProjectionMatrix = ProjectionMatrix * modelViewMatrix;
            
    // Compute a 4*4 normal matrix
    mat4 normalMatrix = transpose(inverse(modelViewMatrix));
    vec3 normal = normalize((normalMatrix * vec4(normalize(Normal), 1.0)).xyz);

    vec3 lightPosition = (ViewMatrix * vec4(LightPosition, 1.0)).xyz;

    // Compute vertex position in Model space
    vec4 vertexPositionModelSpace = modelViewMatrix * vec4(Position,1.0);

    // vertex to eye vector (V)
    vec3 vertexNormalized = normalize(-vertexPositionModelSpace.xyz);

    // vertex to lightsource vector (L)
    vec3 lightVector = normalize(lightPosition - vertexPositionModelSpace.xyz);
    
    // half vector (H = V + L)
    vec3 halfVector = normalize(lightVector + vertexNormalized);


    // SHADING 

    // Ambient term 
    vec3 ambientPart = vec3(Color * AmbientFactor);

    // Diffuse term
    vec3 diffusePart = clamp(dot(normal, lightVector), 0.0, 1.0)*LightColor;
    diffusePart *= vec3(DiffuseFactor);

    // Specular term
    vec3 specularPart = pow(clamp(dot(normal, halfVector),0.0,1.0),5.0 )*LightColor;
    specularPart *= vec3(SpecularFactor);

    // final color is the sum of 3 terms
    vColor = vec4(Color*diffusePart + specularPart + ambientPart, 1.0);

    gl_Position = modelViewProjectionMatrix * vec4(Position, 1.0);

}
