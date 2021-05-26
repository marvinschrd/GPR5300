#version 450 core

layout(location = 0) out vec4 FragColor;

in vec3 out_position;
in vec3 out_normal;
in vec2 out_tex;
in vec3 out_camera;

uniform sampler2D Diffuse;
uniform sampler2D Specular;

const float ambientStrengh = 0.1;
const float specular_strength = .5;
const float specular_pow = 1;
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const vec3 lightPos = vec3(1.0, 1.0, 3.0);

void main()
{
    // Compute ambiant light.
    vec3 ambient = ambientStrengh * lightColor;

    // Compute diffuse light.
    vec3 normal = normalize(out_normal);
    vec3 lightDirection = normalize(lightPos - out_position);
    float diff = max(dot(out_normal, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // Compute specular light.
    vec3 view_direction = normalize(out_camera - out_position);
    vec3 reflection_direction = reflect(-lightPos, out_normal);
    float spec = pow(max(dot(view_direction, reflection_direction), 0.0), specular_pow);
    vec3 specular = specular_strength * spec * lightColor;

    // Total light.
    vec3 result_diffuse_ambient = 
        (ambient + diffuse + specular) * texture(Diffuse, out_tex).rgb; 
    vec3 result_specular =
        specular * texture(Specular, out_tex).r;
    FragColor = vec4(result_diffuse_ambient + result_specular, 1.0);
}
