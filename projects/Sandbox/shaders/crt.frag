#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D inputImage;

// ─────────────────────────────────────────────
// SHADOW MASK PATTERNS
// ─────────────────────────────────────────────

vec3 shadowMask(vec2 fragCoord) {
    vec2 cell = mod(fragCoord, vec2(3.0, 2.0));
    
    if(mod(floor(fragCoord.y / 2.0), 2.0) > 0.5) {
        cell.x = mod(cell.x + 1.5, 3.0);
    }
    
    float col = floor(cell.x);
    vec3 phosphorColor = vec3(0.0);
    
    vec2 dotCenter = vec2(cell.x - floor(cell.x) - 0.5,
                          cell.y - 1.0);
    float dotDist = length(dotCenter);
    float dotGlow = exp(-dotDist * dotDist * 8.0);
    
    if(col < 0.5)       phosphorColor = vec3(dotGlow, 0.0,     0.0);
    else if(col < 1.5)  phosphorColor = vec3(0.0,     dotGlow, 0.0);
    else                phosphorColor = vec3(0.0,     0.0,     dotGlow);
    
    return phosphorColor;
}

vec3 apertureGrille(vec2 fragCoord) {
    float stripe = mod(fragCoord.x, 3.0);
    
    float edgeSoftness = 0.5;
    
    float r = smoothstep(0.0, edgeSoftness, stripe) * 
              (1.0 - smoothstep(1.0 - edgeSoftness, 1.0, stripe));
    float g = smoothstep(1.0, 1.0 + edgeSoftness, stripe) * 
              (1.0 - smoothstep(2.0 - edgeSoftness, 2.0, stripe));
    float b = smoothstep(2.0, 2.0 + edgeSoftness, stripe) * 
              (1.0 - smoothstep(3.0 - edgeSoftness, 3.0, stripe));
              
    return vec3(r, g, b);
}

vec3 slotMask(vec2 fragCoord) {
    vec3 grille = apertureGrille(fragCoord);
    
    float slotPeriod = 3.0;
    float slotPos = mod(fragCoord.y, slotPeriod) / slotPeriod;
    float slotDark = 1.0 - 0.4 * smoothstep(0.7, 0.9, slotPos);
    
    return grille * slotDark;
}

// ─────────────────────────────────────────────
// SCANLINE
// ─────────────────────────────────────────────

float scanlineGaussian(vec2 fragCoord, 
                       float sourceHeight,
                       float outputHeight,
                       float scanlineWidth,
                       float scanlineOpacity,
                       float scanlinePhase) {
    float linePos = fract((fragCoord.y / outputHeight) 
                          * sourceHeight * 0.5 + scanlinePhase);
    
    float sigma = scanlineWidth * 0.5;
    float beam = exp(-pow(linePos - 0.5, 2.0) / (2.0 * sigma * sigma));
    
    return mix(1.0 - scanlineOpacity, 1.0, beam);
}

// ─────────────────────────────────────────────
// BLOOM
// ─────────────────────────────────────────────

vec3 sampleWithBloom(vec2 uv,
                     float outputWidth,
                     float outputHeight,
                     float bloomStrength,
                     float bloomRadius) {
    vec2 texelSize = vec2(1.0 / outputWidth, 1.0 / outputHeight);
    
    vec3 color = texture(inputImage, uv).rgb;
    
    if(bloomStrength < 0.001) return color;
    
    vec3 bloom = vec3(0.0);
    float totalWeight = 0.0;
    
    for(int x = -2; x <= 2; x++) {
        for(int y = -2; y <= 2; y++) {
            vec2 offset = vec2(float(x), float(y)) 
                          * texelSize * bloomRadius;
            vec3 s = texture(inputImage, uv + offset).rgb;
            
            vec3 bright = max(s - 0.6, 0.0);
            float dist = float(x*x + y*y);
            float weight = exp(-dist * 0.5);
            
            bloom += bright * weight;
            totalWeight += weight;
        }
    }
    
    bloom /= totalWeight;
    return color + bloom * bloomStrength;
}

// ─────────────────────────────────────────────
// MAIN
// ─────────────────────────────────────────────

void main() {

    // ── Tweak these ───────────────────────────

    // Scanline controls
    float scanlineOpacity = 0.7;   // 0.0 = off, 1.0 = fully dark gaps
    float scanlineWidth   = 0.4;   // 0.1 = thin beam, 0.9 = wide beam
    float scanlinePhase   = 0.0;   // 0.0 or 0.5 — flip each frame for alternating

    // Shadow mask controls
    float maskOpacity     = 0.9;   // 0.0 = off, 1.0 = full phosphor structure
    float maskBrightness  = 1.5;   // Compensate for darkening, try 1.3 - 1.8
    int   maskType        = 1;     // 0 = shadow mask, 1 = aperture grille, 2 = slot mask

    // Bloom controls
    float bloomStrength   = 0.4;   // 0.0 = off, 1.0 = heavy glow
    float bloomRadius     = 1.5;   // Size of glow spread in pixels

    // Source and output dimensions
    // Set these to match your actual textures
    float sourceWidth     = 640.0;
    float sourceHeight    = 640.0;
    float outputWidth     = 640.0;
    float outputHeight    = 640.0;

    // ─────────────────────────────────────────

    vec2 fragCoord = uv * vec2(outputWidth, outputHeight);
    
    vec3 color = sampleWithBloom(uv, 
                                 outputWidth, outputHeight,
                                 bloomStrength, bloomRadius);
    
    // ── Phosphor mask ─────────────────────────
    vec3 mask;
    if(maskType == 0)      mask = shadowMask(fragCoord);
    else if(maskType == 1) mask = apertureGrille(fragCoord);
    else                   mask = slotMask(fragCoord);
    
    vec3 masked = color * mask * maskBrightness;
    vec3 result = mix(color, masked, maskOpacity);
    
    // ── Scanlines ─────────────────────────────
    float scanline = scanlineGaussian(fragCoord,
                                      sourceHeight, outputHeight,
                                      scanlineWidth, scanlineOpacity,
                                      scanlinePhase);
    result *= scanline;
    
    // ── Vignette ──────────────────────────────
    vec2 centered = uv - 0.5;
    float vignette = 1.0 - dot(centered, centered) * 1.2;
    vignette = clamp(vignette, 0.0, 1.0);
    result *= vignette;
    
    outColor = vec4(result, 1.0);
}