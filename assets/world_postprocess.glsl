#version 330

#if __VERSION__ < 130
#define texture texture2D
#endif

precision mediump float;

varying vec2 fragTexCoord;
uniform sampler2D texture0;
uniform vec2 worldSize;

float hsvValue(vec3 color) {
    return max(max(color.r, color.g), color.b);
}

void main() {
    const int R = 3;
    float dx = (1.0 / worldSize.x);
    float dy = (1.0 / worldSize.y);

    float sum = 0.0;
    for (int y = -R; y <= R; y++) {
        for (int x = -R; x <= R; x++) {
            vec4 sample = texture(texture0, fragTexCoord + vec2(x, y) * vec2(dx, dy));
            float value = hsvValue(sample.rgb);
            if (value < 0.5) value = 0.85;
            sum += value;
        }
    }
    sum /= float((R * 2 + 1) * (R * 2 + 1));

    vec4 sample = texture(texture0, fragTexCoord);
    float value = hsvValue(sample.rgb);
    if (value < 0.925) {
        sample.a = 0.0;
    }
    gl_FragColor = mix(sample, vec4(0, 0, 0, 1), (1.0 - abs(sum - 0.925) * 14.0) * 1.5);
}
