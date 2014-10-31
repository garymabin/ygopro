precision mediump float;
attribute vec3 v_position;
attribute vec4 v_color;
attribute vec4 v_hcolor;
attribute vec2 v_texcoord;
uniform mat4 mvp;
varying vec4 color;
varying vec4 hcolor;
varying vec2 texcoord;

void main(void) {
  color = v_color;
  hcolor = v_hcolor;
  texcoord = v_texcoord;
  gl_Position = mvp * vec4(v_position, 1.0);
}
