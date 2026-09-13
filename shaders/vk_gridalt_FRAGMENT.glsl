#version 450

layout(location = 0) in vec3 vWorldPos;
layout(location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform GridBuffer {
    float gridSize;
    float lineThickness;
    float maxRange;
    float zoomSensitivity;
    vec3 colorMajor; // Color for lines at multiples of (gridSize * majorDivisor) e.g., 100m
    float _p1;
    vec3 colorMinor; // Color for standard lines e.g., 10m
    float majorDivisor; // How often a major line appears (e.g., 5)
    vec3 cameraPosition;
    float _p2;
} gridSettings;

void main() {
    float distToCamera = distance(vWorldPos, gridSettings.cameraPosition);

    // Early cull past the visible range. The distance fade below is exactly
    // 0.0 here, so nothing pops: the grid has already fully faded out.
    if (distToCamera > gridSettings.maxRange) {
        discard;
    }

    // Distance fade: 1.0 near the camera, smoothly to 0.0 at maxRange.
    // The grid disappears before it reaches the edge of its backing quad,
    // so no hard border is ever visible.
    float fade = clamp(1.0f - (distToCamera / gridSettings.maxRange), 0.0f, 1.0f);
    fade = fade * fade;

    // Coordinates within the current grid cell.
    // mod() handles negative world coordinates naturally.
    float xLocal = mod(vWorldPos.x, gridSettings.gridSize);
    float yLocal = mod(vWorldPos.y, gridSettings.gridSize);

    // Distance to the nearest line in each direction.
    float distToXLine = min(xLocal, gridSettings.gridSize - xLocal);
    float distToYLine = min(yLocal, gridSettings.gridSize - yLocal);

    // Zoom compensation: far away, lines shrink on screen. Scaling the
    // world-space thickness with camera distance keeps the on-screen width
    // roughly constant (the "infinite grid" look). Clamped to half a cell
    // so lines can never merge into a solid fill.
    // (zoomSensitivity = 0 disables the compensation.)
    float zoomScale = mix(1.0f, distToCamera * 0.5f, gridSettings.zoomSensitivity);
    float thickness = min(gridSettings.lineThickness * zoomScale, gridSettings.gridSize * 0.5f);

    // Antialiased line edges: solid core up to 0.5 * thickness,
    // smooth falloff to 0 at 1.5 * thickness.
    float lineX = 1.0f - smoothstep(thickness * 0.5f, thickness * 1.5f, distToXLine);
    float lineY = 1.0f - smoothstep(thickness * 0.5f, thickness * 1.5f, distToYLine);
    float lineAlpha = max(lineX, lineY);

    if (lineAlpha < 0.001f) {
        discard;
    }

    // Every majorDivisor-th line is a major line; color follows the
    // stronger of the two lines. mod() on the rounded index stays
    // correct for negative coordinates.
    bool isMajor = (lineX >= lineY)
        ? (mod(round(vWorldPos.x / gridSettings.gridSize), gridSettings.majorDivisor) == 0.0f)
        : (mod(round(vWorldPos.y / gridSettings.gridSize), gridSettings.majorDivisor) == 0.0f);

    vec3 color = isMajor ? gridSettings.colorMajor : gridSettings.colorMinor;

    outFragColor = vec4(color, lineAlpha * fade);
}
