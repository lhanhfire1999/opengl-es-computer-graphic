precision mediump float;

varying vec2 v_texCoord;

uniform sampler2D u_sampler;
uniform float u_opacity;

void main()
{
	gl_FragColor.rgb = texture2D(u_sampler, v_texCoord).rgb;

	gl_FragColor.a = gl_FrontFacing ? u_opacity:1.0-u_opacity;
}