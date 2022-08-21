#include "cell_generation.h"

std::vector<Point> generate_cells(Surface_mesh &mesh, int count)
{
    std::shared_ptr<Tree> aabbtree = std::make_shared<Tree> (faces(mesh).first, faces(mesh).second, mesh);
    aabbtree->accelerate_distance_queries();

    Point_inside insider_tester(*aabbtree);
    CGAL::Bbox_3 bbox = PMP::bbox(mesh);

    std::vector<Point> result;

    while (count)
    {
        Point p = random_generate_cell(bbox);
        if (insider_tester(p) == CGAL::ON_BOUNDED_SIDE){
            result.push_back(p);
            count--;
        }
    }

    return result;

}


Point random_generate_cell(Bbox &bbox)
{
    double x_min = bbox.xmin();
    double y_min = bbox.ymin();
    double z_min = bbox.zmin();
    double x_max = bbox.xmax();
    double y_max = bbox.ymax();
    double z_max = bbox.zmax();

    double random_x = x_min + (x_max - x_min) * ((double) rand() / RAND_MAX);
    double random_y = y_min + (y_max - y_min) * ((double) rand() / RAND_MAX);
    double random_z = z_min + (z_max - z_min) * ((double) rand() / RAND_MAX);

    return Point(random_x, random_y, random_z);
}

Surface_mesh& merge_meshes(Surface_mesh &mesh1, Surface_mesh &mesh2)
{
    // copy source to target
    CGAL::copy_face_graph(mesh1, mesh2);
    PMP::stitch_borders(mesh2);

    return mesh2;
}