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
    vec4 darkest = vec4(1, 0, 1, 1);
    for (int y = -R; y <= R; y++) {
        for (int x = -R; x <= R; x++) {
            vec4 sample = texture(texture0, fragTexCoord + vec2(x, y) * vec2(dx, dy));
            float value = hsvValue(sample.rgb);
            sum += value;
            if (value <= hsvValue(darkest.rgb)) darkest = sample;
        }
    }
    sum /= float((R * 2 + 1) * (R * 2 + 1));
    darkest.rgb /= hsvValue(darkest.rgb);

    gl_FragColor = vec4(vec3(0, 0, 0), (1.0 - abs(sum - 0.925) * 14.0) * 1.5);
}
