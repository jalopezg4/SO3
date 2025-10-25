#pragma once
#include <string>
#include <vector>


struct Config {
    std::string ops; // e.g. "ce"
    std::string comp_alg = "rle";
    std::string enc_alg = "xor";
    std::vector<unsigned char> key;
};


// Procesa un archivo (lectura, operaciones, escritura)
void process_file(const std::string &input_path, const std::string &output_path, const Config &cfg);