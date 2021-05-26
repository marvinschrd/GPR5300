#version 450 core

layout(location = 0) out vec4 FragColor;

in vec3 out_normal;
in vec3 out_pos;
in vec2 out_tex;
in vec3 camera_view;

uniform sampler2D textureDiffuse;

const float ambientStrength = 0.3;
const vec3 lightPos = vec3(0.0, -1.5, 3.0);
const vec3 lightColor = vec3(1.0, 0.0, 0.0);
const vec3 lightColor2 = vec3(1.0, 1.0, 1.0);


void main()
{
    vec3 ambient = ambientStrength * lightColor2;

    vec3 normal = normalize(out_normal);
    vec3 lightDir = normalize(lightPos - out_pos);
    float diff = max(dot(out_normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor2;

    vec3 result = (ambient + diffuse) * texture(textureDiffuse, out_tex).rgb;
    FragColor = vec4(result, 1.0);
}