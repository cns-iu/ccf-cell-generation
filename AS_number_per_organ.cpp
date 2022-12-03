#include <iostream>
#include <boost/filesystem.hpp>
#include <fstream>
namespace fs = boost::filesystem;

int main(int argc, char **argv)
{

    std::ofstream output;
    output.open("number_of_AS.csv");
    output << "organ, number of AS\n";

    if (argc < 2)
    {
        std::cout << "Please provide the body_path" << std::endl;
        return 0;
    }

    auto body_path = argv[1];
    for (fs::directory_entry& organ_path : fs::directory_iterator(body_path)) 
    {
        fs::path organ_name = organ_path.path().stem();

        int count = 0;
        for (fs::directory_entry& AS : fs::directory_iterator(organ_path)) count++;

        std::cout << organ_name.string() << " has " << count << " anatomical structures\n";
        output << organ_name.string() << ","<< count << "\n"; 
    }

    output.close();

    return 0;
}