
#pragma once

#include "image.h"
#include <cmath>

class BaseFilter {
public:
    virtual void init(int& argc, char**& argv) = 0;
    virtual void implement_filter([[maybe_unused]] Image& image) = 0;
    virtual ~BaseFilter() = default;
};

template <typename T>
class MatrixFilter : public BaseFilter {
protected:
    int matrix_x_size;
    int matrix_y_size;
    std::vector<std::vector<T>> matrix{};

public:
    Color get_filtered_pixel(const Image& image, int x, int y);
};

class Crop : public BaseFilter {
    const static int arguments_quantity = 2;
    int h;
    int w;

public:
    void init(int& argc, char**& argv);

    void implement_filter(Image& image);
};

class Grayscale : public BaseFilter {
public:
    void init([[maybe_unused]] int& argc, [[maybe_unused]] char**& argv);

    void implement_filter(Image& image);
};

class Negative : public BaseFilter {
public:
    void init([[maybe_unused]] int& argc, [[maybe_unused]] char**& argv);

    void implement_filter(Image& image);
};

class Sharpening : public MatrixFilter<int> {
public:
    void init([[maybe_unused]] int& argc, [[maybe_unused]] char**& argv);

    void implement_filter(Image& image);
};

class EdgeDetection : public MatrixFilter<int> {
    const static int arguments_quantity = 1;
    float threshold;

public:
    void init(int& argc, char**& argv);

    void implement_filter(Image& image);
};

class GaussianBlur : public MatrixFilter<float> {
    const static int arguments_quantity = 1;
    int sigma;

public:
    Color get_filtered_pixel(const Image& image, int x, int y);
    void init(int& argc, char**& argv);
    void implement_filter(Image& image);
};
