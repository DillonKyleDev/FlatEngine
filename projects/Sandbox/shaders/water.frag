#version 450



layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout (input_attachment_index = 0, binding = 2) uniform subpassInput inputDepth;

void main() {            
    outColor = texture(texSampler, fragTexCoord);
    if (fragColor.z / 10 > 0)
    {
        outColor += fragColor / 10;
    }
}