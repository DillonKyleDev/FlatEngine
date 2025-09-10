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
    gl_Position = projection * view * (model * vec4(inPosition.x, inPosition.y, inPosition.z + (sin(time) * inPosition.x), 1) + position);    
    fragColor = vec4(inColor.x, inColor.y, inColor.z, 1);
    fragTexCoord = inTexCoord;
    vec4 rotatedNormal = model * vec4(inNormal.x, inNormal.y, inNormal.z, 1);
    normal = vec3(rotatedNormal.x, rotatedNormal.y, rotatedNormal.z);
}