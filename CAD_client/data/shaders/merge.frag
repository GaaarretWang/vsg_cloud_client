#version 450
#extension GL_ARB_separate_shader_objects : enable

#define VIEW_DESCRIPTOR_SET 0
#define MATERIAL_DESCRIPTOR_SET 1

layout(set = MATERIAL_DESCRIPTOR_SET, binding = 0) uniform sampler2D cadColor;

layout(set = MATERIAL_DESCRIPTOR_SET, binding = 1) uniform sampler2D cadDepth;

layout(set = MATERIAL_DESCRIPTOR_SET, binding = 2) uniform sampler2D planeColor;

layout(set = MATERIAL_DESCRIPTOR_SET, binding = 3) uniform sampler2D planeDepth;

layout(set = MATERIAL_DESCRIPTOR_SET, binding = 4) uniform sampler2D shadowColor;

layout(set = MATERIAL_DESCRIPTOR_SET, binding = 5) uniform sampler2D shadowDepth;

layout(location = 0) out vec4 outColor;

void main()
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy);
    ivec2 screenSize = ivec2(640, 480);
    vec2 texCoord = vec2(fragCoord * 1.f) / vec2(screenSize);

    float intCadDepth = texture(cadDepth, texCoord).r;
    float intPlaneDepth = texture(planeDepth, texCoord).r;
    float intShadowDepth = texture(shadowDepth, texCoord).r;

    if(intCadDepth > intPlaneDepth)
        outColor.rgb = texture(cadColor, texCoord).rgb;
    else
        outColor.rgb = texture(planeColor, texCoord).rgb * texture(shadowColor, texCoord).rgb;
    outColor.a = 1.f;
}
