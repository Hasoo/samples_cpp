#include <boost/filesystem.hpp>
#include <iostream>

int main(int argc, char** argv) {
    if (2 > argc) {
        std::cout << "usage: test path" << std::endl;
        return 1;
    }

    std::cout << argv[1] << " " << boost::filesystem::file_size(argv[1])
              << std::endl;

    return 0;
}