#version 330

precision mediump float;

varying vec2 fragTexCoord;
uniform sampler2D texture0;
uniform vec2 worldSize;

float brightness(int r) {
    float dx = (1.0 / worldSize.x);
    float dy = (1.0 / worldSize.y);

    float sum = 0;
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            vec4 sample = texture(texture0, fragTexCoord + vec2(x * dx, y * dy));
            sum += max(sample.r, max(sample.g, sample.b));
        }
    }
    sum /= (r * 2 + 1) * (r * 2 + 1);
    return sum;
}

void main() {
    float b = brightness(5);
    vec4 sample = texture(texture0, fragTexCoord);
    sample /= max(sample.r, max(sample.g, sample.b));
    gl_FragColor = vec4(sample.rgb * 0.7, (1.0 - abs(b - 0.925) * 14.0) * 1.5);
}
