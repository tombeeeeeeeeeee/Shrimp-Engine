#version 330 core

out vec3 direction;
uniform mat4 inversePV;
void main()
{
        vec2 pos = vec2( (gl_VertexID & 2)>>1, 1 - (gl_VertexID & 1)) * 2.0 - 1.0;
        vec4 front = inversePV * vec4(pos, -1.0, 1.0);
        vec4 back = inversePV * vec4(pos,  1.0, 1.0);

        direction = back.xyz / back.w - front.xyz / front.w;
        gl_Position = vec4(pos,1.0,1.0);
}

//credit to derhass (stack overflow)