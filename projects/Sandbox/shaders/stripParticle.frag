#version 450

layout(push_constant, std430) uniform pc {
    vec4 light1Dir;
    vec4 light1Color;
    vec4 light2Dir;
    vec4 light2Color;
    vec4 light3Dir;
    vec4 light3Color;
    vec4 light4Dir;
    vec4 light4Color;
};

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 viewDirection;
layout(location = 3) in vec4 noiseAtPoint;
layout(location = 4) in float time;
layout(location = 5) in vec4 color;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main() {    
    float smoothness = 1;
    vec4 specularTint = vec4(.5,.5,.5,1);

    vec4 reflectionDir = reflect(-light1Dir, vec4(normal, 1));    
    vec4 halfVector = normalize(light1Dir + viewDirection);
    vec2 newTexCoord = fragTexCoord;
    newTexCoord.x += time;
    vec4 albedo = texture(texSampler, fragTexCoord);     
    // albedo *= 1 - max(specularTint.x, max(specularTint.y, specularTint.z)); // energy conservation
    
    float phongIntensity = max(0, dot(viewDirection, reflectionDir));    
    float blinPhongIntensity = max(0.0, dot(halfVector, vec4(normal, 1)));

    vec4 diffuse = albedo * light1Color * clamp(dot(light1Dir, vec4(normal, 1)), 0, 1);
    diffuse.w = 1;
    vec4 specular = specularTint * light1Color * vec4(pow(blinPhongIntensity, smoothness * 100));

    outColor = albedo * color;
}