#version 450

layout(push_constant, std430) uniform pc {
    layout(offset = 0)   vec4 position;
    layout(offset = 16)  vec4 cameraPos;
    layout(offset = 32)  float time;
    layout(offset = 64)  mat4 model;
    layout(offset = 128) mat4 view;
    layout(offset = 192) mat4 projection;
};

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 normal;

void main() {    
    vec4 localPos = model * vec4(inPosition.x, inPosition.y, inPosition.z, 1);
    vec4 worldPos = vec4(localPos.x + position.x, localPos.y + position.y, sin(localPos.x + time) + sin(localPos.y + time), 1);
    gl_Position = projection * view * worldPos;    
    fragTexCoord = inTexCoord;
    fragColor = vec4(sin(localPos.x + time) + sin(localPos.y + time));
}