varying vec2 texCoordVarying;
uniform sampler2D texture;

void main() {
    gl_FragColor = texture2D(texture, texCoordVarying);
}