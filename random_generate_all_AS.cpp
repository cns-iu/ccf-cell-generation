#include "cell_generation.h"
#include <unordered_map>
#include <fstream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;


Surface_mesh load_mesh(std::string file_path)
{
    std::ifstream input(file_path);
    Surface_mesh mesh;

    if (!input || !(input >> mesh) || mesh.is_empty())
    {
        std::cerr << file_path << " Not a valid mesh" << std::endl;
    }
    // std::cout << file_path << PMP::volume(mesh) * 1e9 << std::endl;
    return mesh;
}


int main(int argc, char **argv)
{

    srand(time(0));

    if (argc < 2)
    {
        std::cout << "Please provide the surface mesh folder~" << std::endl;
    }

    auto body_path = argv[1];
    auto output_dir = argv[2];

    // csv file of point locations
    std::ofstream points_file;
    points_file.open("points.csv");
    points_file << "organ,AS,x,y,z\n";

    if (!fs::exists(output_dir)) fs::create_directory(output_dir);
    fs::path dir(output_dir);

    for (fs::directory_entry& organ_path : fs::directory_iterator(body_path)) 
    {
        fs::path organ_name = organ_path.path().stem();
        fs::path output_organ_dir = dir / organ_name;

        if (!fs::exists(output_organ_dir)) fs::create_directory(output_organ_dir);

        for (fs::directory_entry& AS : fs::directory_iterator(organ_path)) 
        {
            std::string file_path = AS.path().string();
            std::string AS_name = AS.path().stem().string();

            Surface_mesh mesh = load_mesh(file_path);
            if (!mesh.is_empty())
            {
                // generate cells
                auto points = generate_cells(mesh, 1000);

                // save all the points to a mesh
                Surface_mesh point_mesh;
                for (auto p: points)
                {
                    points_file << organ_name.string() << "," << AS_name << "," << p[0] << "," << p[1] << "," << p[2] << "\n";
                    point_mesh.add_vertex(p);
                }
                
                fs::path output_file = output_organ_dir / AS.path().stem(); 
                std::ofstream point_mesh_off(output_file.string() + "_cells.off");
                point_mesh_off << point_mesh;
                point_mesh_off.close();

            }

        }
    }


}