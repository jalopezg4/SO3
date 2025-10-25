#include "rle.h"


std::vector<unsigned char> rle_compress(const std::vector<unsigned char> &in) {
    std::vector<unsigned char> out;
    size_t n = in.size();
    size_t i = 0;
    while (i < n) {
        unsigned char val = in[i];
        size_t run = 1;
        while (i + run < n && in[i + run] == val && run < 255) run++;
        out.push_back(static_cast<unsigned char>(run));
        out.push_back(val);
        i += run;
    }
    return out;
}


std::vector<unsigned char> rle_decompress(const std::vector<unsigned char> &in) {
    std::vector<unsigned char> out;
    size_t i = 0;
    size_t n = in.size();
    while (i + 1 < n) {
        unsigned char count = in[i];
        unsigned char val = in[i+1];
        out.insert(out.end(), count, val);
        i += 2;
    }
    return out;
}