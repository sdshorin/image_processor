

#include "fabric.h"
#include "arg_parser.h"

ProgramArgs Parser::operator()(int argc, char** argv) {
    ProgramArgs args;

    if (argc < 3) {
        throw std::invalid_argument("Need minimum 3 arguments: ./image_processor [file_in] [file_out] [filter]");
    }
    args.file_in = argv[1];
    args.file_out = argv[2];
    argv += 3;
    argc -= 3;
    while (argc > 0) {
        std::string command = argv[0];
        ++argv;
        --argc;
        std::shared_ptr<BaseFilter> filter = filter_fabric(command);
        filter->init(argc, argv);
        args.filters.push_back(filter);
    }
    return args;
}
