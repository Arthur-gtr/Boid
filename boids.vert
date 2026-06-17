#version 450
struct Boid { vec4 position; vec4 velocity; };
layout(std430, binding = 0) readonly buffer Boids { Boid boids[]; };
layout(push_constant) uniform Push { mat4 viewProj; } push;

const vec3 vertices[4] = vec3[](
    vec3(0.0, 0.0, 0.06), vec3(-0.02, -0.02, -0.02),
    vec3(0.02, -0.02, -0.02), vec3(0.0, 0.02, -0.02)
);
const int indices[12] = int[](0,1,2, 0,2,3, 0,3,1, 1,3,2);

layout(location = 0) out vec3 fragColor;

void main() {
    vec3 pos = boids[gl_InstanceIndex].position.xyz;
    vec3 vel = normalize(boids[gl_InstanceIndex].velocity.xyz);
    
    vec3 up = vec3(0.0, 1.0, 0.0);
    if (abs(dot(up, vel)) > 0.99) up = vec3(1.0, 0.0, 0.0);
    vec3 right = normalize(cross(up, vel));
    up = cross(vel, right);
    mat3 rot = mat3(right, up, vel);

    vec3 localPos = vertices[indices[gl_VertexIndex]];
    vec3 worldPos = pos + rot * localPos;
    gl_Position = push.viewProj * vec4(worldPos, 1.0);
    
    vec3 color = vec3(0.2, 0.8, 1.0);
    fragColor = color * (0.4 + 0.6 * max(0.0, localPos.z / 0.06));
}