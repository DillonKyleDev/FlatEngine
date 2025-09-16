#version 450

layout(push_constant, std430) uniform pc {
    vec4 lightDir;
};

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
    // Textures are sampled using the built-in texture function.
    // It takes a sampler and coordinate as arguments. 
    // The sampler automatically takes care of the filtering and transformations in the background

    outColor = texture(texSampler, fragTexCoord);
    float intensity = dot(normal, vec3(lightDir.x, lightDir.y, lightDir.z));
    outColor.xyz *= intensity; 
}