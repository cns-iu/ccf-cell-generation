#include "cell_generation.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Please provide two surface meshes path" << std::endl;
    }

    auto file_path_1 = argv[1];
    auto file_path_2 = argv[2];
    std::ifstream input_1(file_path_1);
    std::ifstream input_2(file_path_2);

    Surface_mesh mesh_1;
    Surface_mesh mesh_2;

    if (!input_1 || !(input_1 >> mesh_1) || mesh_1.is_empty())
    {
        std::cerr << file_path_1 << " Not a valid mesh" << std::endl;
        return 0;

    }

    if (!input_2 || !(input_2 >> mesh_2) || mesh_2.is_empty())
    {
        std::cerr << file_path_2 << " Not a valid mesh" << std::endl;
        return 0;

    }

    Surface_mesh merged_mesh = merge_meshes(mesh_1, mesh_2);

    std::ofstream merged_off("merged.off");
    merged_off << merged_mesh;
    merged_off.close();
    
}