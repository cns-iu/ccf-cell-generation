#pragma once

#include <iostream>
#include <vector>
#include <ctime>


// CGAL library
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/polygon_mesh_processing.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/Side_of_triangle_mesh.h>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point;
typedef Kernel::Triangle_3 Triangle;
typedef Kernel::Vector_3 Vector;
typedef CGAL::Surface_mesh<Point> Surface_mesh;
typedef CGAL::AABB_face_graph_triangle_primitive<Surface_mesh> Primitive;
typedef CGAL::AABB_traits<Kernel, Primitive> Traits;
typedef CGAL::Side_of_triangle_mesh<Surface_mesh, Kernel> Point_inside;
typedef CGAL::AABB_tree<Traits> Tree;
typedef CGAL::Bbox_3 Bbox;
typedef Surface_mesh::Face_range Face_range;
typedef Surface_mesh::Halfedge_index halfedge_descriptor;
typedef Surface_mesh::Vertex_index vertex_descriptor;
typedef Surface_mesh::Face_index face_descriptor;

namespace PMP = CGAL::Polygon_mesh_processing;

std::vector<Point> generate_cells(Surface_mesh &mesh, int count);
Point random_generate_cell(Bbox &bbox);
Surface_mesh& merge_meshes(Surface_mesh &mesh1, Surface_mesh &mesh2);