

#include "image.h"

#include <fstream>

void write_byte(std::ostream& out, uint8_t value) {
    out.write(reinterpret_cast<char*>(&value), 1);
}

void write_int(std::ostream& out, uint32_t value) {
    for (size_t i = 0; i < BYTES_PER_INT; ++i) {
        write_byte(out, (value >> (i * BITS_PER_BYTE)) & BYTE_MASK);
    }
}

void write_zeros(std::ostream& out, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        write_byte(out, 0);
    }
}

void write_bmp_header(std::ostream& out, size_t height, size_t width) {
    write_byte(out, BMP_SIGNATURE_BYTE_1);
    write_byte(out, BMP_SIGNATURE_BYTE_2);
    int padding = (4 - (width * 3) % 4) % 4;
    int file_size = HEADER_SIZE + height * (width * 3 + padding);
    write_int(out, file_size);
    write_zeros(out, 4);
    write_int(out, HEADER_SIZE);
    write_int(out, INFO_HEADER_SIZE);
    write_int(out, width);
    write_int(out, height);
    write_byte(out, 1);
    write_zeros(out, 1);
    write_byte(out, 24);
    write_zeros(out, 1);
    write_zeros(out, 24);
}

void write_color(std::ostream& out, const Color& color) {
    write_byte(out, color.b);
    write_byte(out, color.g);
    write_byte(out, color.r);
}

uint hex_to_int(char* x, size_t start, size_t finish) {
    uint tmp = 0;
    for (size_t i = finish - 1; i >= start; --i) {
        tmp <<= 8;
        tmp += x[i];
    }
    return tmp;
}

Image::Image(std::string file_path) {
    read_from_file(file_path);
}

void Image::read_from_file(const std::string& file_path) {

    std::ifstream in(file_path, std::ios::binary);

    char header[54];
    in.read(header, 54);
    if (!in) {
        throw std::invalid_argument("Can't read image");
    }
    if (header[0] != BMP_SIGNATURE_BYTE_1 || header[1] != BMP_SIGNATURE_BYTE_2) {
        throw std::invalid_argument("Can't find bmp magic header");
    }

    width = hex_to_int(header, 18, 22);
    height = hex_to_int(header, 22, 26);
    int data_offset = hex_to_int(header, 10, 14);
    int byte_count = hex_to_int(header, 28, 30) / 8;
    data = std::vector<std::vector<Color>>(height, std::vector<Color>(width));
    int real_size = byte_count * width;
    int padding = (4 - real_size % 4) % 4;
    int start = data_offset;
    int finish = start + real_size;
    for (int i = 0; i < height; ++i) {
        in.seekg(start);
        for (int j = start; j < finish;) {
            in.read(reinterpret_cast<char*>(&data[i][(j - start) / 3].b), 1);
            in.read(reinterpret_cast<char*>(&data[i][(j - start) / 3].g), 1);
            in.read(reinterpret_cast<char*>(&data[i][(j - start) / 3].r), 1);
            j += 3;
        }
        start = finish + padding;
        finish = start + real_size;
    }
}

void Image::write_to_file(const std::string& file_path) {
    std::ofstream out(file_path, std::ios::binary);
    write_bmp_header(out, height, width);
    int padding = (4 - (width * 3) % 4) % 4;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            write_byte(out, data[i][j].b);
            write_byte(out, data[i][j].g);
            write_byte(out, data[i][j].r);
        }
        write_zeros(out, padding);
    }
}

bool Image::is_pixel_in_box(int x, int y) const {
    return !(x < 0 || y < 0 || x >= height || y >= width);
}
Color Image::get_pixel(int x, int y) const {
    x = std::clamp(x, 0, height - 1);
    y = std::clamp(y, 0, width - 1);
    return data[x][y];
}

std::ostream& operator<<(std::ostream& os, Color const& c) {
    os << "Color (" << static_cast<int>(c.r) << ", " << static_cast<int>(c.g) << ", " << static_cast<int>(c.b) << ")";
    return os;
}
