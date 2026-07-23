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

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D bumpSampler;

void main() {
    // outColor = texture(texSampler, fragTexCoord);
    // float intensity = dot(normal, vec3(light1Dir.x, light1Dir.y, light1Dir.z));
    // outColor.xyz *= light1Color.xyz * clamp(intensity, .6, 1.5); 
    // outColor.w = 1;

    vec4 bumpHeight = texture(bumpSampler, fragTexCoord);  
    vec2 textureDimensions = textureSize(bumpSampler, 1);
    float delta = 1 / textureDimensions.x;
    vec2 adjacentTexCoord = fragTexCoord + delta;  
    vec4 adjacentBumpHeight = texture(bumpSampler, adjacentTexCoord);
    float difference = adjacentBumpHeight.x - bumpHeight.x;

    vec3 bumpTangent = normalize(vec3(0, delta, difference));
    vec3 bumpNormal = vec3(-bumpTangent.y, bumpTangent.x, 0);
    
    float smoothness = .1;
    vec4 specularTint = vec4(.5,.5,.5,1);

    vec4 reflectionDir = reflect(-light1Dir, vec4(bumpNormal, 1));    
    vec4 halfVector = normalize(light1Dir + viewDirection);
    vec4 albedo = texture(texSampler, fragTexCoord);     
    albedo *= 1 - max(specularTint.x, max(specularTint.y, specularTint.z)); // energy conservation
    
    float phongIntensity = max(0, dot(viewDirection, reflectionDir));    
    float blinPhongIntensity = max(0.0, dot(halfVector, vec4(bumpNormal, 1)));

    vec4 diffuse = albedo * light1Color * clamp(dot(light1Dir, vec4(bumpNormal, 1)), 0, 1);
    vec4 specular = specularTint * light1Color * vec4(pow(blinPhongIntensity, smoothness * 100));
    specular *= max(0, dot(light1Dir, viewDirection)); // To remove incorrect highlight when lit from behind, but it's not the best approach, becomes 0 at 90 degrees
    outColor = diffuse + specular;
    outColor.w = 1;

    //vec3 normalizedLightDir = normalize(vec3(light1Dir.x, light1Dir.y, light1Dir.z));
    //outColor = texture(texSampler, fragTexCoord);
    //float intensity = dot(bumpNormal, normalizedLightDir);
    //outColor.xyz *= light1Color.xyz * clamp(intensity, .6, 1.5); 
    //outColor.w = 1;
}