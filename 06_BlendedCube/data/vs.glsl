attribute vec3 a_position;
attribute vec2 a_texCoord;

varying vec2 v_texCoord;

uniform mat4 u_matrix;

void main(){
	gl_Position = u_matrix * vec4(a_position, 1.0);
	v_texCoord = a_texCoord;
}