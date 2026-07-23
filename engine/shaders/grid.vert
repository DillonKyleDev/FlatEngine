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
    vec4 worldPos = vec4(floor(localPos.x + ubo.cameraPosition.x), floor(localPos.y + ubo.cameraPosition.y), localPos.z, 1);
    gl_Position = ubo.projection * ubo.view * worldPos;   
    float xyDistance = distance(vec2(worldPos), vec2(ubo.cameraPosition));
    float zDistance = distance(ubo.meshPosition.z, ubo.cameraPosition.z);
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