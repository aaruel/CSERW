#version 410 core

in vec2 Uvs;
out vec4 color;
uniform sampler2D sampler;

void main() {
	color = texture(sampler,Uvs);
    
//    color = vec4(0.5,0.5,0.0,0.0);
    
//    vec2 u = vec2(Uvs.x, Uvs.y);
//    vec2 t = u * 8;
//    if (fract(t.s) < 0.1 || fract(t.t) < 0.1) color = vec4(0.0,0.0,1.0,1.0);
//    else discard;
}