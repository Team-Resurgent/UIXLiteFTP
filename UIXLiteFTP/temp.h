//#include <iostream>
//#include <cmath>
//
#define LAT_DIV 10  // Number of latitude divisions
#define LON_DIV 20  // Number of longitude divisions
#define TOTAL_VERTICES ((LAT_DIV + 1) * (LON_DIV + 1))
#define TOTAL_TRIANGLES (LAT_DIV * LON_DIV * 2)

struct Vertex {
    float x, y, z;  // Position
    float u, v;     // Texture coordinates
};

struct Triangle {
    int v1, v2, v3; // Indices for the three vertices
};

// Global arrays
Vertex vertices[TOTAL_VERTICES];
Triangle triangles[TOTAL_TRIANGLES];
//
//// Function to generate a textured sphere using triangles
void generateSphere(float radius) {
    int vertexIndex = 0;
    int triangleIndex = 0;

    // Generate vertices with UV coordinates
    for (int i = 0; i <= LAT_DIV; ++i) {
        float theta = M_PI * i / LAT_DIV; // Latitude angle [0, pi]
        float v = (float)i / LAT_DIV;     // UV V-coordinate

        for (int j = 0; j <= LON_DIV; ++j) {
            float phi = 2.0f * M_PI * j / LON_DIV; // Longitude angle [0, 2*pi]
            float u = (float)j / LON_DIV;         // UV U-coordinate

            float x = radius * sin(theta) * cos(phi);
            float y = radius * cos(theta);
            float z = radius * sin(theta) * sin(phi);

            vertices[vertexIndex++] = {x, y, z, u, v};
        }
    }

    // Generate triangles (2 per quad)
    for (int i = 0; i < LAT_DIV; ++i) {
        for (int j = 0; j < LON_DIV; ++j) {
            int v1 = i * (LON_DIV + 1) + j;
            int v2 = v1 + 1;
            int v3 = (i + 1) * (LON_DIV + 1) + j;
            int v4 = v3 + 1;

            // First triangle
            triangles[triangleIndex++] = {v1, v2, v4};

            // Second triangle
            triangles[triangleIndex++] = {v1, v4, v3};
        }
    }
}
//
//// Main function
//int main() {
//    float radius = 1.0f;
//    generateSphere(radius);
//
//    // Print vertices
//    std::cout << "Vertices:\n";
//    for (int i = 0; i < TOTAL_VERTICES; ++i) {
//        std::cout << i << ": (" << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z 
//                  << ") UV(" << vertices[i].u << ", " << vertices[i].v << ")\n";
//    }
//
//    // Print triangles
//    std::cout << "\nTriangles:\n";
//    for (int i = 0; i < TOTAL_TRIANGLES; ++i) {
//        std::cout << i << ": (" << triangles[i].v1 << ", " << triangles[i].v2 << ", " << triangles[i].v3 << ")\n";
//    }
//
//    return 0;
//}