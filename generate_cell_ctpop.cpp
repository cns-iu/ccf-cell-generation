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

    if (argc < 3)
    {
        std::cout << "Please provide the surface mesh folder" << std::endl;
    }

    auto body_path = argv[1];
    auto specification_file_path = argv[2];
    // for testing
    Surface_mesh point_mesh;

    std::ofstream points_csv;
    auto output_file_path = argv[3];
    points_csv.open(output_file_path);
    points_csv << "organ, anatomical structure, cell_type, x, y, z\n";

    std::ifstream specification_csv(specification_file_path);
    if (!specification_csv.is_open()) throw std::runtime_error("could not open specification table!");

    std::string line;
    std::getline(specification_csv, line);
    std::vector<std::string> row;
    std::string word;

    while (std::getline(specification_csv, line))
    {
        row.clear();
        std::stringstream ss(line);

        while (std::getline(ss, word, ','))
        {
            row.push_back(word);
        }

        std::string organ = row[0];
        std::string AS = row[1];
        std::string cell_type = row[2];
        int count = std::stod(row[3]);

        // skip Skin because skin is special
        if (organ.find("Skin") != std::string::npos) continue;
        // if (AS.find("VH_F_renal_pyramid_L_a") == std::string::npos && AS.find("VH_F_outer_cortex_of_kidney_L") == std::string::npos) continue;

        fs::path tmp = fs::path(body_path) / fs::path(organ) / fs::path(AS);
        std::string file_path = tmp.string() + ".off";

        if (!fs::exists(file_path)) 
        {
            std::cout << file_path << " not exist" << std::endl;
            continue; 
        }

        // for testing, use count/100
        count = count / 100;
        std::cout << "generating " << cell_type << " count " << count << std::endl;
        Surface_mesh mesh = load_mesh(file_path);
        auto points = generate_cells(mesh, count);


        for (auto &p: points)
        {
            points_csv << organ << "," << AS << "," << cell_type << "," << p[0] << "," << p[1] << "," << p[2] << "\n";

            if (AS == "VH_M_outer_cortex_of_kidney_R")
                point_mesh.add_vertex(p);
        }

    }

    std::ofstream point_mesh_off("point_mesh.off");
    point_mesh_off << point_mesh;
    point_mesh_off.close();
    points_csv.close();

}