#version 450



layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 baseColor;
layout(location = 2) in vec4 secondaryColor;
layout(location = 3) in float disturbance;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D innerShapeSampler;
layout(binding = 2) uniform sampler2D outerShapeSampler;
layout(binding = 3) uniform sampler2D noiseSampler;

float clamp(float toClamp) {
    if  (toClamp < 0.5)
    {
        return 0;
    }
    else
    {
        return 1;        
    }  
}

void main() {
    vec4 baseColorFrag = texture(innerShapeSampler, fragTexCoord);
    vec4 secondaryColorFrag = texture(outerShapeSampler, fragTexCoord);
    baseColorFrag *= baseColor;
    secondaryColorFrag *= secondaryColor;
    vec2 movingTexCoord = fragTexCoord;
    movingTexCoord *= .2;
    movingTexCoord.y += disturbance;
    vec4 noiseSample = texture(noiseSampler, movingTexCoord);

    outColor = baseColorFrag * noiseSample.x;
    outColor += secondaryColorFrag;
    
    if (outColor.w < 0.5)
    {
        outColor = vec4(0);
    }
    else
    {
        //outColor = secondaryColorFrag;
        //outColor.w = 1;
    }
}