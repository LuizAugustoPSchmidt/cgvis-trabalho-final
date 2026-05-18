#ifndef MESHDATA_H
#define MESHDATA_H

#include <vector>
#include <glm/vec3.hpp>

struct MeshData {
    std::vector<float> model_coefficients;
    std::vector<float> normal_coefficients;
    std::vector<float> texture_coefficients;
    std::vector<unsigned int> indices;
    
    struct ShapeInfo {
        std::string name;
        size_t first_index;
        size_t num_indices;
        glm::vec3 bbox_min;
        glm::vec3 bbox_max;
    };
    std::vector<ShapeInfo> shapes;
};

#endif // MESHDATA_H
