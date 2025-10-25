#pragma once
#include <vector>


// Run-Length Encoding (RLE) básico, operaciones sobre bytes.
std::vector<unsigned char> rle_compress(const std::vector<unsigned char> &in);
std::vector<unsigned char> rle_decompress(const std::vector<unsigned char> &in);