attribute vec4 position;
attribute vec2 texCoord;

varying vec2 texCoordVarying;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
    gl_Position = mvp * position;
    texCoordVarying = texCoord;
}