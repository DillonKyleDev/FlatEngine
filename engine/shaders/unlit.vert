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
layout(location = 2) out vec3 normal;

void main() {    
    vec4 localPos = ubo.model * vec4(inPosition.x, inPosition.y, inPosition.z, 1);
    vec4 worldPos = vec4(localPos.x + ubo.meshPosition.x, localPos.y + ubo.meshPosition.y, localPos.z + ubo.meshPosition.z, 1);
    gl_Position = ubo.projection * ubo.view * worldPos;   
    fragTexCoord = inTexCoord;
    fragColor = vec4(inColor.x, inColor.y, inColor.z, 1);
    normal = inNormal;
}