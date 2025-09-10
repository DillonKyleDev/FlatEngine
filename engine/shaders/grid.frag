#version 450



layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main() {    
    float xyAdjust = pow(fragColor.x, 3) * 10000;
    float zAdjust = clamp(pow(fragColor.z, 2) * 500, .5, 1);
    float transparency = xyAdjust;

    outColor = texture(texSampler, fragTexCoord);  
    outColor.w = clamp(transparency, 0, 1); 
}