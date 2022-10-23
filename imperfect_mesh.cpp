#include "cell_generation.h"
#include <fstream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;


int main(int argc, char **argv)
{

    auto file_path = argv[1];

    std::ifstream input(file_path);
    Surface_mesh mesh;

    if (!input || !(input >> mesh) || mesh.is_empty())
    {
        std::cerr << file_path << " Not a valid mesh" << std::endl;
        return 0;

    }
    std::vector<Point> points = generate_cells(mesh, 1000);

    Surface_mesh point_mesh;
    std::ofstream points_file;
    points_file.open("points.csv");
    for (auto p: points)
    {
        points_file << p[0] << "," << p[1] << "," << p[2] << "\n";
        point_mesh.add_vertex(p);
    }

    std::ofstream point_mesh_off("point_mesh.off");
    point_mesh_off << point_mesh;
    point_mesh_off.close();

}