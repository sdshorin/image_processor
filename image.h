
#pragma once

#include <vector>

#include <string>

const size_t BMP_SIGNATURE_BYTE_1 = 0x42;
const size_t BMP_SIGNATURE_BYTE_2 = 0x4D;
const size_t HEADER_SIZE = 54;
const size_t INFO_HEADER_SIZE = 40;
const size_t BITS_PER_PIXEL = 24;
const size_t AFTER_IMAGE_PARAMS_BYTE_COUNT = 24;
const size_t BITS_PER_BYTE = 8;
const size_t BYTES_PER_INT = 4;
const size_t BYTE_MASK = 0xFF;

struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    Color() = default;
    Color(int _r, int _g, int _b) {
        r = std::clamp(_r, 0, 255);
        g = std::clamp(_g, 0, 255);
        b = std::clamp(_b, 0, 255);
    }

    friend std::ostream& operator<<(std::ostream& os, Color const& c);
};

struct Image {
    std::vector<std::vector<Color>> data;
    int width;
    int height;
    Image() = default;
    Image(std::string file_path);

    void read_from_file(const std::string& file_path);
    void write_to_file(const std::string& file_path);
    Color get_pixel(int x, int y) const;
    bool is_pixel_in_box(int x, int y) const;
};
