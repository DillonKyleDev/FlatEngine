#version 450

layout(push_constant, std430) uniform pc {
    vec4 time;
    vec4 position;
    vec4 cameraPos;
    mat4 model;
    mat4 view;
    mat4 projection;
};

layout(set = 0, binding = 0) uniform UniformBufferObject {
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
    float zPos = (sin(localPos.x + time.x) + sin(localPos.y + time.x)) + (.5 * sin(localPos.x + (3 * time.x))) + sin(5 * localPos.y);
    vec4 worldPos = vec4(localPos.x + position.x, localPos.y + position.y, zPos, 1);
    gl_Position = ubo.projection * ubo.view * worldPos;    
    fragTexCoord = inTexCoord;
    fragColor = vec4(sin(localPos.x + time.x) + sin(localPos.y + time.x));
}