#version 450



layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
    // Textures are sampled using the built-in texture function.
    // It takes a sampler and coordinate as arguments. 
    // The sampler automatically takes care of the filtering and transformations in the background

    vec3 lightDir = vec3(0,1,0);
    float intensity = (lightDir.x * normal.x / 2) + (lightDir.y * normal.y / 2) + (lightDir.z * normal.z / 2);
    outColor = texture(texSampler, fragTexCoord);
    outColor.xyz *= (intensity + 1);
}