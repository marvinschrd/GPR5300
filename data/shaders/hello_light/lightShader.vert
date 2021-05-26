#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;

out vec3 out_normal;
out vec3 out_camera_view;
out vec3 out_pos;
out vec2 out_tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 inv_model;
uniform vec3 camera_pos;



void main()
{
    mat4 pvm = projection * view * model;
    out_pos = (pvm * vec4(aPos, 1.0)).xyz;
    gl_Position = pvm * vec4(aPos, 1.0);
    out_tex = aTex;
    out_normal = vec3(inv_model * vec4(aNormal, 1.0));
    out_camera_view = camera_pos;
}