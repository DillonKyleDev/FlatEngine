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

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {    
    vec4 localPos = ubo.model * vec4(inPosition.x, inPosition.y, inPosition.z, 1);
    float xPos = localPos.x + ubo.meshPosition.x;
    float yPos = localPos.y + ubo.meshPosition.y;
    float zDistruption = (2 * sin(localPos.x + ubo.vec4s[0].x)) + (1.6 * sin(3 * (localPos.y + ubo.vec4s[0].x))) + (0.2 * cos(2 * (localPos.x + ubo.vec4s[0].x)));
    float zPos = ubo.meshPosition.z + localPos.z + zDistruption;
    vec4 worldPos = vec4(xPos, yPos, zPos, 1);
    gl_Position = ubo.projection * ubo.view * worldPos;    
    fragTexCoord = inTexCoord;
    fragColor = vec4(zDistruption);
}