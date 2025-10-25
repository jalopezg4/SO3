#include "worker.h"
#include "file_manager.h"
#include "rle.h"
#include "xor_cipher.h"
#include <iostream>


void process_file(const std::string &input_path, const std::string &output_path, const Config &cfg) {
    auto data = read_whole_file(input_path);
    if (data.empty() && !path_exists(input_path)) {
        std::cerr << "No se pudo leer: " << input_path << "\n";
        return;
    }
    auto cur = data;
    for (char op : cfg.ops) {
        if (op == 'c') {
            if (cfg.comp_alg == "rle") cur = rle_compress(cur);
            else std::cerr << "Comp alg no implementada: " << cfg.comp_alg << "\n";
        } else if (op == 'd') {
            if (cfg.comp_alg == "rle") cur = rle_decompress(cur);
            else std::cerr << "Comp alg no implementada: " << cfg.comp_alg << "\n";
        } else if (op == 'e') {
            if (cfg.enc_alg == "xor") cur = xor_crypt(cur, cfg.key);
            else std::cerr << "Enc alg no implementada: " << cfg.enc_alg << "\n";
        } else if (op == 'u') {
            if (cfg.enc_alg == "xor") cur = xor_crypt(cur, cfg.key);
            else std::cerr << "Enc alg no implementada: " << cfg.enc_alg << "\n";
        } else {
            std::cerr << "Operación desconocida: " << op << "\n";
        }
    }
    if (!write_whole_file(output_path, cur)) std::cerr << "Fallo al escribir: " << output_path << "\n";
}