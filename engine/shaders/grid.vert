#version 450

layout(push_constant, std430) uniform pc {
    layout(offset = 0)   vec4 position;
    layout(offset = 16)  vec4 cameraPos;
    layout(offset = 32)  float time;
    layout(offset = 64)  mat4 models;
    layout(offset = 128) mat4 views;
    layout(offset = 192) mat4 proj;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
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
    vec4 worldPos = vec4(floor(localPos.x + cameraPos.x), floor(localPos.y + cameraPos.y), localPos.z, 1);
    gl_Position = ubo.projection * ubo.view * worldPos;    
    float xyDistance = distance(vec2(worldPos), vec2(cameraPos));
    float zDistance = distance(position.z, cameraPos.z);
    if (xyDistance == 0)
    {
        xyDistance = 0.001;
    }
    if (zDistance == 0)
    {
        zDistance = 0.001;
    }
    fragColor = vec4(1 / xyDistance, 1, 1 / zDistance, 1);
    fragTexCoord = inTexCoord;
    vec4 rotatedNormal = ubo.model * vec4(inNormal.x, inNormal.y, inNormal.z, 1);
    normal = vec3(rotatedNormal.x, rotatedNormal.y, rotatedNormal.z);
}