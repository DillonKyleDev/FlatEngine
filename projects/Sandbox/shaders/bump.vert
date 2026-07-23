#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    vec4 meshPosition;
    vec4 cameraPosition;
    mat4 model;
    mat4 viewAndProjection;        
    vec4 vec4s[32];
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec4 viewDirection;

void main() {    
    // vec4 bumpHeight = texture(bumpSampler, inTexCoord);  
    // vec2 textureDimensions = textureSize(bumpSampler, 1);
    // float delta = .5 / textureDimensions.y;
    // vec2 adjacentTexCoord = inTexCoord + delta;  
    // vec4 adjacentBumpHeight = texture(bumpSampler, adjacentTexCoord);
    // float difference = adjacentBumpHeight.x - bumpHeight.x;

    // vec3 bumpNormal = normalize(vec3(0, 0, bumpHeight.z));

    // vec3 bumpDirection = inNormal * (bumpHeight.x - 0.5) * 0.1;
    // vec4 localPos = ubo.model * vec4(inPosition.x, inPosition.y, inPosition.z, 1);
    // localPos += vec4(bumpDirection, 0);

    vec4 localPos = ubo.model * vec4(inPosition.x, inPosition.y, inPosition.z, 1);
    vec4 worldPos = vec4(localPos.x + ubo.meshPosition.x, localPos.y + ubo.meshPosition.y, localPos.z + ubo.meshPosition.z, 1);
    gl_Position = ubo.viewAndProjection * worldPos;    
    fragTexCoord = inTexCoord;
    vec4 rotatedNormal = ubo.model * vec4(inNormal.x, inNormal.y, inNormal.z, 1);
    normal = normalize(vec3(rotatedNormal.x, rotatedNormal.y, rotatedNormal.z));
    viewDirection = normalize(ubo.cameraPosition - ubo.meshPosition);
}