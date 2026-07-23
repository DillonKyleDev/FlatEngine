#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    vec4 meshPosition;
    vec4 cameraPosition;
    mat4 model;
    mat4 view;
    mat4 projection;        
    vec4 vec4s[32];
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 baseColor;
layout(location = 2) out vec4 secondaryColor;
layout(location = 3) out float disturbance;


void main() {    
    gl_Position = ubo.projection * ubo.view * (ubo.model * vec4(inPosition.x, inPosition.y, inPosition.z, 1) + ubo.meshPosition);        
    fragTexCoord = inTexCoord;
    baseColor = ubo.vec4s[0];
    secondaryColor = ubo.vec4s[1];
    disturbance = ubo.vec4s[2].x;
}