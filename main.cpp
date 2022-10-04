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
    std::cout << file_path << PMP::volume(mesh) * 1e9 << std::endl;
    return mesh;
}

void str_remove_quotes(std::string &s)
{
    if ( s.front() == '"' ) {
        s.erase( 0, 1 ); // erase the first character
        s.erase( s.size() - 1 ); // erase the last character
    }
}


void compute_cell_number_per_tissue(std::vector<std::string> &row_info, std::string cell_info_folder, 
                                    std::unordered_map<std::string, Surface_mesh> &meshes_AS)
{

    // parse row information
    std::string tissue_block_id = row_info[0];
    std::string dataset_id = row_info[1];
    std::string renal_pyramid = row_info[2];
    double percentage_of_AS = std::stod(row_info[3]);
    double percentage_of_tissue = std::stod(row_info[4]);
    Surface_mesh &mesh = meshes_AS[renal_pyramid];

    Surface_mesh point_mesh;

    std::ofstream points_file;
    points_file.open("./data/output/cells_" + tissue_block_id + ".csv");
    points_file << "tissue_block_id" << "," << "dataset_id" << "," << "renal_pyramid" << "," << "cell_type" << "," << "x" << "," << "y" << "," <<  "z" << "\n";

    std::string cell_info_path = cell_info_folder + "/" + dataset_id + ".csv";
    std::ifstream tissue_cell_info_csv(cell_info_path);
    if (!tissue_cell_info_csv.is_open()) throw std::runtime_error("could not open " + cell_info_path);

    std::string line;
    std::getline(tissue_cell_info_csv, line);
    std::vector<std::string> row;
    std::string word;

    while (std::getline(tissue_cell_info_csv, line))
    {
        row.clear();
        std::stringstream ss(line);

        while (std::getline(ss, word, ','))
        {
            str_remove_quotes(word);
            row.push_back(word);
        }

        std::string cell_type = row[0];
        int count = std::stoi(row[5]);

        int estimated_count = (int)(1.0 * count * percentage_of_tissue / percentage_of_AS);
        std::vector<Point> points = generate_cells(mesh, estimated_count);

        for (auto p: points)
        {
            points_file << tissue_block_id << "," << dataset_id << "," << renal_pyramid << "," << cell_type << "," << p[0] << "," << p[1] << "," << p[2] << "\n"; 
            point_mesh.add_vertex(p);
        }
        // cell_type_map[renal_pyramid][cell_type] = count;
        
        
    }

    points_file.close();

    std::ofstream point_mesh_off("point_mesh.off");
    point_mesh_off << point_mesh;
    point_mesh_off.close();

}



int main(int argc, char **argv)
{

    srand(time(0));

    if (argc < 4)
    {
        std::cout << "Please provide the surface mesh folder and cell csv folder and cell number specification ~" << std::endl;
    }

    std::unordered_map<std::string, std::unordered_map<std::string, int>> cell_type_map;
    std::unordered_map<std::string, Surface_mesh> meshes_AS;

    auto body_path = argv[1];
    auto cell_info_folder = argv[2];
    auto specification_file_path = argv[3];

    for (fs::directory_entry& organ_path : fs::directory_iterator(body_path)) 
    {
        std::string organ_name = organ_path.path().stem().string();
        std::cout << organ_name << std::endl;   
        for (fs::directory_entry& AS : fs::directory_iterator(organ_path)) 
        {
            std::string file_path = AS.path().string();
            std::string AS_name = fs::path(file_path).stem().string();

            Surface_mesh mesh = load_mesh(file_path);
            if (!mesh.is_empty()) meshes_AS[AS_name] = mesh;

        }
    }

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

        compute_cell_number_per_tissue(row, cell_info_folder, meshes_AS);


    }



    // std::ifstream input(file_path);
    // Surface_mesh mesh;

    // if (!input || !(input >> mesh) || mesh.is_empty())
    // {
    //     std::cerr << file_path << " Not a valid mesh" << std::endl;
    //     return 0;

    // }
    // std::vector<Point> points = generate_cells(mesh, 1000);

    // Surface_mesh point_mesh;
    // std::ofstream points_file;
    // points_file.open("points.csv");
    // for (auto p: points)
    // {
    //     points_file << p[0] << "," << p[1] << "," << p[2] << "\n";
    //     point_mesh.add_vertex(p);
    // }

    // std::ofstream point_mesh_off("point_mesh.off");
    // point_mesh_off << point_mesh;
    // point_mesh_off.close();


    return 0;

}