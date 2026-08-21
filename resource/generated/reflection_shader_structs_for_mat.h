#include <stddef.h>
#include "runtime_registry.h"
typedef struct Pbr { 
    vec4 color;
    float ambient_strength;
    float roughness_strength;
    float metallic_strength;
    float normal_strength;
    float ambient_occlusion_strength;
    float emissive_strength;
  } Pbr;
