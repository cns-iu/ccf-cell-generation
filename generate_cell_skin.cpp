#include <unordered_map>
#include <fstream>
#include <boost/filesystem.hpp>
#include <vector>
#include <fstream>

#include "cell_generation.h"

namespace fs = boost::filesystem;

Surface_mesh load_mesh(std::string file_path)
{
    std::ifstream input(file_path);
    Surface_mesh mesh;

    if (!input || !(input >> mesh) || mesh.is_empty())
    {
        std::cerr << file_path << " Not a valid mesh" << std::endl;
    }
    return mesh;
}


std::vector<Point> skin_cells_within_d(std::vector<Triangle> &triangles, double d, int count)
{
    std::vector<Point> result;

    while (count --)
    {
        double distance = ((double) rand() / RAND_MAX) * d;
        // double distance = d;
        // std::cout << distance << std::endl;

        double a = (double) rand() / RAND_MAX;
        double b = (double) rand() / RAND_MAX;
        double c;

        if (a > b) {c = 1 - a; a = a - b; }
        else {c = 1 - b; b = b - a; };

        // construct a point p inside the triangle
        int x = rand() % triangles.size();
        Triangle t = triangles[x];
        Point p1 = t[0], p2 = t[1], p3 = t[2];

        Vector v1 = Vector(p1[0], p1[1], p1[2]);
        Vector v2 = Vector(p2[0], p2[1], p2[2]);
        Vector v3 = Vector(p3[0], p3[1], p3[2]);

        Vector p = a * v1 + b * v2 + c * v3;
        Vector normal = CGAL::unit_normal(p1, p2, p3);
        Vector q = p - normal * distance; 

        result.push_back(Point(q[0], q[1], q[2]));

    }

    return result;

}

int main(int argc, char **argv)
{
    srand(time(0));


    // int count = std::stoi(argv[3]);
    
    auto body_path = argv[1];
    auto specification_file_path = argv[2];
    double thickness = std::stod(argv[3]);

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

        fs::path tmp = fs::path(body_path) / fs::path(organ) / fs::path(AS);
        std::string file_path = tmp.string() + ".off";

        if (!fs::exists(file_path)) 
        {
            std::cout << file_path << " not exist" << std::endl;
            continue; 
        }

        // for testing, use count/100
        if (organ.find("Skin") != std::string::npos)
        {
            count = count / 100;
            std::cout << "generating " << cell_type << " count " << count << std::endl;
            Surface_mesh mesh = load_mesh(file_path);

            std::vector<Triangle> triangles;

            for (auto f: mesh.faces())
            {

                halfedge_descriptor hd = halfedge(f, mesh);
                Point p1 = mesh.point(source(hd, mesh));
                Point p2 = mesh.point(target(hd, mesh));
                Point p3 = mesh.point(target(next(hd, mesh), mesh));
                triangles.push_back(Triangle(p1, p2, p3));

            }

            auto points = skin_cells_within_d(triangles, thickness, count);

            std::string output_csv_file = "cell_location_" + AS + ".csv"; 
            std::ofstream points_csv;
            points_csv.open(output_csv_file);
            points_csv << "organ, anatomical structure, cell_type, x, y, z\n";
            for (auto &p: points)
            {
                points_csv << organ << "," << AS << "," << cell_type << "," << p[0] << "," << p[1] << "," << p[2] << "\n";
            }
            points_csv.close();

        }


        // for (auto &p: points)
        // {
        //     points_csv << organ << "," << AS << "," << cell_type << "," << p[0] << "," << p[1] << "," << p[2] << "\n";

        //     if (AS == "VH_M_outer_cortex_of_kidney_R")
        //         point_mesh.add_vertex(p);
        // }

    }

    // std::ofstream point_mesh_off("point_mesh.off");
    // point_mesh_off << point_mesh;
    // point_mesh_off.close();
    // points_csv.close();


    // auto result = skin_cells_within_d(triangles, thickness, count);

    // Surface_mesh point_mesh;

    // for (Point &p: result) point_mesh.add_vertex(p);

    // std::string output_path = "point_mesh_" + std::string(argv[2]) + ".off";
    // std::ofstream point_mesh_off(output_path);
    // point_mesh_off << point_mesh;
    // point_mesh_off.close();

}