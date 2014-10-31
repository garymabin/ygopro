precision mediump float;
varying vec4 color;
varying vec4 hcolor;
varying vec2 texcoord;
uniform sampler2D texid;

void main(void) {
  if(gl_FrontFacing) {
    vec4 texcolor = texture2D(texid, texcoord);
    gl_FragColor = mix(texcolor * color, vec4(hcolor.r, hcolor.g, hcolor.b, 1.0), hcolor.a);
  } else {
    discard;
  }
}
