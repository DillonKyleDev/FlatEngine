#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    vec4 meshPosition;
    vec4 cameraPosition;
    mat4 model;
    mat4 viewAndProjection;    
    float time;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 normal;

void main() {    
    gl_Position = ubo.viewAndProjection * (ubo.model * vec4(inPosition.x, inPosition.y, inPosition.z + (sin(ubo.time) * inPosition.x), 1) + ubo.meshPosition);    
    fragColor = vec4(inColor.x, inColor.y, inColor.z, 1);
    fragTexCoord = inTexCoord;
    vec4 rotatedNormal = ubo.model * vec4(inNormal.x, inNormal.y, inNormal.z, 1);
    normal = vec3(rotatedNormal.x, rotatedNormal.y, rotatedNormal.z);
}