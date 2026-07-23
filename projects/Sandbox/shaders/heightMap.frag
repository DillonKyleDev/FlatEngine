#version 450


layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main() {            
    vec4 sampledColor = texture(texSampler, fragTexCoord);
    float intensity = clamp(sampledColor.x + .5, 0.5, 1.5);
    outColor = sampledColor * vec4(0.55, 0.42, 0.27, 1);
}