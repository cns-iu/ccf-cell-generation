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
        Vector q = p + CGAL::normal(p1, p2, p3) * distance; 

        result.push_back(Point(q[0], q[1], q[2]));

    }

    return result;

}

int main(int argc, char **argv)
{
    srand(time(0));

    auto skin_path = std::string(argv[1]);
    double thickness = std::stod(argv[2]);
    int count = std::stoi(argv[3]);


    Surface_mesh mesh = load_mesh(skin_path);

    std::vector<Triangle> triangles;

    for (auto f: mesh.faces())
    {

        halfedge_descriptor hd = halfedge(f, mesh);
        Point p1 = mesh.point(source(hd, mesh));
        Point p2 = mesh.point(target(hd, mesh));
        Point p3 = mesh.point(target(next(hd, mesh), mesh));
        triangles.push_back(Triangle(p1, p2, p3));

    }

    auto result = skin_cells_within_d(triangles, thickness, count);

    Surface_mesh point_mesh;

    for (Point &p: result) point_mesh.add_vertex(p);

    std::ofstream point_mesh_off("point_mesh.off");
    point_mesh_off << point_mesh;
    point_mesh_off.close();

}