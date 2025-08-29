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

float hsvSaturation(vec3 color) {
    return (hsvValue(color) - min(min(color.r, color.g), color.b)) / hsvValue(color);
}

vec4 outline(vec4 sample, vec4 color, float low, float high, float sum) {
    if (sum < low || sum > high) return sample;
    float mid = (low + high) / 2.0;
    float diff = high - low;
    return mix(sample, color, (1.0 - abs(sum - mid) / diff * 2.0) * 1.5);
}

void main() {
    const int R = 3;
    float dx = (1.0 / worldSize.x);
    float dy = (1.0 / worldSize.y);

    float sum = 0.0;
    float vmin = 1.0;
    float vmax = 0.0;
    for (int y = -R; y <= R; y++) {
        for (int x = -R; x <= R; x++) {
            vec4 sample = texture(texture0, fragTexCoord + vec2(x, y) * vec2(dx, dy));
            float value = hsvValue(sample.rgb);
            if (hsvSaturation(sample.rgb) < 0.75 && value > 0.06) value = 0.85;
            vmin = min(vmin, value);
            vmax = max(vmax, min(min(sample.r, sample.g), sample.b));
            sum += value;
        }
    }
    sum /= float((R * 2 + 1) * (R * 2 + 1));

    vec4 sample = texture(texture0, fragTexCoord);
    float value = hsvValue(sample.rgb);
    if (value < 0.925 || hsvSaturation(sample.rgb) < 0.75) {
        sample.a = 0.0;
    }
    if (vmax < 0.9) sample = outline(sample, vec4(0, 0, 0, 1), 0.85, 1.0, sum);
    if (vmin < 0.06) sample = outline(sample, vec4(1, 1, 1, 1), 0.0, 0.85, sum);
    gl_FragColor = sample;
}
