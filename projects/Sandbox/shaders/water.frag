#version 450



layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main() {            
    outColor = texture(texSampler, fragTexCoord);
    float intensity = clamp(fragColor.x + .5, 0.5, 1.5);
    outColor.xyz *= intensity; 
}