
#include <iostream>

#include "arg_parser.h"
#include "filters.h"
#include "image.h"

void print_help() {
    std::cout << "Usage: image_processor {BMP file in} {BMP file out}" << std::endl;
    std::cout << "[-{filter name 1} [filter param 1] [filter param 2] ...]" << std::endl;
    std::cout << "[-{filter name 2} [filter param 1] [filter param 2] ...]" << std::endl;
    std::cout << "Avaliable filters:" << std::endl;
    std::cout << "-crop width[int] height[int]   # Crop" << std::endl;
    std::cout << "-gs                            # Grayscale" << std::endl;
    std::cout << "-neg                           # Negative" << std::endl;
    std::cout << "-sharp                         # Sharpening" << std::endl;
    std::cout << "-edge threshold[float]         # Edge Detection" << std::endl;
    std::cout << "-blur sigma[float]             # Gaussian Blur" << std::endl;
}

int main(int argc, char** argv) {
    Parser parser;
    ProgramArgs args;

    if (argc < 3) {
        print_help();
        return 1;
    }

    try {
        args = parser(argc, argv);
    } catch (std::exception& e) {
        std::cout << "Parsing error: " << e.what() << std::endl;
        return 1;
    }

    Image image;
    try {
        image.read_from_file(args.file_in);
    } catch (std::exception& e) {
        std::cout << "Can't read file: " << e.what() << std::endl;
        return 1;
    }

    for (auto& filter : args.filters) {
        filter->implement_filter(image);
    }

    image.write_to_file(args.file_out);
}
