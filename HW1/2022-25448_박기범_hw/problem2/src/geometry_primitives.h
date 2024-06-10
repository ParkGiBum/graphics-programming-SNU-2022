#ifndef GEOMETRY_PRIMITIVES_H
#define GEOMETRY_PRIMITIVES_H
struct GeometryPrimitives
{
    float* vertices;
    unsigned int* indices;
};
float triangle_position_colors[] = {
    // positions         // colors
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
    0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
};
unsigned int triangle_indices[] = { 0,1,2 };
float quad_positions_colors[] = {
    // positions         // colors
    0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // top right
    -0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // top left
    -0.5f,  -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom left
    0.5f,  -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
};
unsigned int quad_indices[] = { 0,1,3,1,2,3 };
#endif