

#include "filters.h"

#include <algorithm>
#include <thread>
#include <iostream>

void Crop::init(int& argc, char**& argv) {

    if (argc < arguments_quantity) {
        throw std::invalid_argument("Invalid arguments quantity in Crop: need 2 arguments");
    }

    try {
        h = std::max(1, std::stoi(argv[0]));
        w = std::max(1, std::stoi(argv[1]));
        // std::cout << "Create filter Crop with params: h " << h << ", w " << w << std::endl;
    } catch (std::exception& e) {
        throw std::invalid_argument("Invalid arguments in Crop: -crop [int] [int]");
    }
    argv += 2;
    argc -= 2;
}

void Crop::implement_filter(Image& image) {
    image.width = std::min(image.width, w);
    image.height = std::min(image.height, h);
    if (image.data.end() - image.height != image.data.begin()) {
        std::move(image.data.end() - image.height, image.data.end(), image.data.begin());
    }
    image.data.reserve(image.height);
    for (auto& row : image.data) {
        row.resize(image.width);
    }
}

template <typename T>
Color MatrixFilter<T>::get_filtered_pixel(const Image& image, int x, int y) {
    // std::cout << "matrix_size: " << GetMatrixSize() << std::endl;
    int r = 0, g = 0, b = 0;
    for (int i = -(matrix_x_size / 2); i <= matrix_x_size / 2; ++i) {
        for (int j = -(matrix_y_size / 2); j <= matrix_y_size / 2; ++j) {
            Color pixel = image.get_pixel(x + i, y + j);
            T matrix_mult = matrix[matrix_x_size / 2 + i][matrix_y_size / 2 + j];
            r += static_cast<int>(pixel.r) * matrix_mult;
            g += static_cast<int>(pixel.g) * matrix_mult;
            b += static_cast<int>(pixel.b) * matrix_mult;
        }
    }
    return Color(r, g, b);
}

void Grayscale::init([[maybe_unused]] int& argc, [[maybe_unused]] char**& argv) {
    std::cout << "Create filter Grayscale" << std::endl;
}

void Grayscale::implement_filter(Image& image) {
    for (auto& row : image.data) {
        for (auto& pixel : row) {
            uint8_t gray = 0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b;
            pixel.r = gray;
            pixel.g = gray;
            pixel.b = gray;
        }
    }
}

void Negative::init([[maybe_unused]] int& argc, [[maybe_unused]] char**& argv) {
    std::cout << "Create filter Negative" << std::endl;
}

void Negative::implement_filter(Image& image) {
    for (auto& row : image.data) {
        for (auto& pixel : row) {
            pixel.r = 255 - pixel.r;
            pixel.g = 255 - pixel.g;
            pixel.b = 255 - pixel.b;
        }
    }
}

void Sharpening::init([[maybe_unused]] int& argc, [[maybe_unused]] char**& argv) {
    std::cout << "Create filter Sharpening" << std::endl;
}

void Sharpening::implement_filter(Image& image) {

    matrix = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
    matrix_x_size = 3;
    matrix_y_size = 3;

    Image copy = image;
    for (size_t i = 0; i < image.data.size(); ++i) {
        for (size_t j = 0; j < image.data[0].size(); ++j) {
            copy.data[i][j] = get_filtered_pixel(image, i, j);
        }
    }
    std::cout << "Copy data for filter" << std::endl;
    image.data = copy.data;
}

void EdgeDetection::init(int& argc, char**& argv) {

    if (argc < arguments_quantity) {
        throw std::invalid_argument("Invalid arguments quantity in EdgeDetection: need 1 arguments");
    }

    try {
        threshold = std::stof(argv[0]);
        // std::cout << "Create filter Crop with params: h " << h << ", w " << w << std::endl;
    } catch (std::exception& e) {
        throw std::invalid_argument("Invalid arguments in EdgeDetection: -edge [float]");
    }
    argv += 1;
    argc -= 1;
}

void EdgeDetection::implement_filter(Image& image) {
    matrix = {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}};
    matrix_x_size = 3;
    matrix_y_size = 3;
    Grayscale grayscale;
    grayscale.implement_filter(image);
    Image copy = image;
    for (size_t i = 0; i < image.data.size(); ++i) {
        for (size_t j = 0; j < image.data[0].size(); ++j) {
            if (get_filtered_pixel(image, i, j).r > (threshold * 255.0)) {
                copy.data[i][j] = Color(0, 0, 0);
            } else {
                copy.data[i][j] = Color(255, 255, 255);
            }
        }
    }
    std::cout << "Copy data for filter" << std::endl;
    image.data = copy.data;
}

void GaussianBlur::init(int& argc, char**& argv) {

    if (argc < arguments_quantity) {
        throw std::invalid_argument("Invalid arguments quantity in GaussianBlur: need 1 arguments");
    }

    try {
        sigma = std::stoi(argv[0]);
        // std::cout << "Create filter Crop with params: h " << h << ", w " << w << std::endl;
    } catch (std::exception& e) {
        throw std::invalid_argument("Invalid arguments in GaussianBlur: -blur [int]");
    }
    if (sigma < 1) {
        throw std::invalid_argument("Invalid arguments in GaussianBlur: -blur must be greater than 0");    
    }
    argv += 1;
    argc -= 1;
}

Color GaussianBlur::get_filtered_pixel(const Image& image, int x, int y) {
    int r = 0, g = 0, b = 0;
    float sum = 0;
    for (int i = 0; i < matrix_x_size; ++i) {
        for (int j = 0; j < matrix_y_size; ++j) {
            if (!image.is_pixel_in_box(x + i - matrix_x_size / 2, y + j - matrix_y_size / 2)) {
                continue;
            }
            Color pixel = image.get_pixel(x + i - matrix_x_size / 2, y + j - matrix_y_size / 2);
            float matrix_mult = matrix[i][j];
            r += static_cast<int>(pixel.r) * matrix_mult;
            g += static_cast<int>(pixel.g) * matrix_mult;
            b += static_cast<int>(pixel.b) * matrix_mult;
            sum += matrix_mult;
        }
    }
    return Color(r / sum, g / sum, b / sum);
}

// time ./image_processor examples/example.bmp out4.bmp -blur 10
// No Thread:  16,83s user 0,27s system 97% cpu 17,543 total

//  1 Threads:   14,89s user 0,24s system 95% cpu 15,786 total
//  8 Threads:   25,11s user 0,33s system 408% cpu 6,230 total
//  16 Threads:  27,72s user 0,34s system 491% cpu 5,714 total

// Using blocs:
//  1 Threads:  16,26s user 0,30s system 94% cpu 17,482 total
//  8 Threads:  26,47s user 0,25s system 450% cpu 5,929 total
//  16 Threads:  26,41s user 0,22s system 491% cpu 5,418 total

void GaussianBlur::implement_filter(Image& image) {
    matrix.resize(1);
    std::vector<float> line(sigma * 3);
    int size = sigma * 3;
    float s2 = 2 * sigma * sigma;
    for (int i = 0; i < size; ++i) {
        int distance = size / 2 - i;
        line[i] = std::exp(-distance * distance / s2);
    }

    matrix[0] = line;
    matrix_x_size = 1;
    matrix_y_size = size;

    Image copy = image;

    std::vector<std::thread> threads;
    int thread_count = std::thread::hardware_concurrency() * 2;
    // thread_count = 1;
    std::cout << "Thread quantity: " << thread_count << std::endl;

    size_t block_size = image.data.size() / thread_count;

    for (int thread_id = 0; thread_id < thread_count; ++thread_id) {
        auto worker = [&copy, &image, this, thread_id, block_size]() {
            int line_start = thread_id * (block_size + 1);
            size_t line_end = (thread_id + 1) * (block_size + 1);
            line_end = std::min(line_end, image.data.size());

            for (size_t i = line_start; i < line_end; ++i) {
                for (size_t j = 0; j < image.data[0].size(); ++j) {
                    copy.data[i][j] = this->get_filtered_pixel(image, i, j);
                }
            }
        };

        threads.emplace_back(worker);
    }

    for (auto& worker_thread : threads) {
        worker_thread.join();
    }

    image = copy;
    matrix.resize(size);
    for (int i = 0; i < size; ++i) {
        auto& row = matrix[i];
        row.resize(1);
        row[0] = line[i];
    }
    matrix_x_size = size;
    matrix_y_size = 1;

    threads.resize(0);

    for (int thread_id = 0; thread_id < thread_count; ++thread_id) {
        auto worker = [&copy, &image, this, thread_id, block_size]() {
            int line_start = thread_id * (block_size + 1);
            size_t line_end = (thread_id + 1) * (block_size + 1);
            line_end = std::min(line_end, image.data.size());

            for (size_t i = line_start; i < line_end; ++i) {
                for (size_t j = 0; j < image.data[0].size(); ++j) {
                    copy.data[i][j] = this->get_filtered_pixel(image, i, j);
                }
            }
        };

        threads.emplace_back(worker);
    }

    for (auto& worker_thread : threads) {
        worker_thread.join();
    }

    std::cout << "Copy data for filter" << std::endl;
    image = copy;
}
