//https://github.com/pjako/msdf_c

#define MSDF_IMPLEMENTATION
#include <msdf.h>

/**
// example fragment shader
// scale is render_width/glyph_width
// render_width being the width of each rendered glyph
float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
  vec3 sample = texture(u_texture, uv).rgb;
  float dist = scale * (median(sample.r, sample.g, sample.b) - 0.5);
  float o = clamp(dist + 0.5, 0.0, 1.0);
  color = vec4(vec3(1.0), o);
}
 */
