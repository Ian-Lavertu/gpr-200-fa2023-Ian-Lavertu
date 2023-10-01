#version 450
out vec4 FragColor;
in vec2 UV;
uniform vec2 iResolution;
uniform float iTime;

// UNIFORMS
uniform float iSunSpeed, iGlowRadius, iSunRadius;
uniform vec3 iTopDayLightCycle, iBottomDayLightCycle;
uniform vec3 iTopNightLightCycle, iBottomNightLightCycle;
uniform vec3 iSunColor, iHillColor;

void main() {
    // BACKGROUND COLORS
    vec3 daylightCycle = mix(iTopDayLightCycle, iBottomDayLightCycle, UV.y);
    vec3 nightCycle = mix(iTopNightLightCycle, iBottomNightLightCycle, UV.y);

    vec3 color = mix(nightCycle, daylightCycle, (sin(iTime) * 0.5) + 0.5);

    // SUN POSITIONING
    vec2 sunUV = UV * 2.0 - 1.0;
    sunUV.x *= iResolution.x / iResolution.y;

    // SUN MOVEMENT
    float sunPos = sin(iTime) - 0.5;
    float sunDistance = distance(sunUV, vec2(0.0, sunPos));
    sunDistance = step(sunDistance, iSunRadius);

    // SUN COLOR
    vec3 sunColor = iSunColor;
    
    color = mix(color, sunColor, sunDistance);

    // HILLS
    float hillHeight = (sin(UV.x * 4.5) * 0.25) + 0.35;
    float hill = 1.0 - step(hillHeight, UV.y);

    color = mix(color, iHillColor, hill);

    //FragColor = vec4(vec3(sunGlow), 1.0);
    FragColor = vec4(color, 1.0);
}