#include <iostream>
#include <vector>
#include <thread>
#include <getopt.h>
#include "file_manager.h"
#include "worker.h"
#include "thread_limiter.h"

// main.cpp (corregido)
// Compilar con: g++ -std=c++17 -O2 -pthread -Iinclude src/*.cpp -o gsea

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Uso mínimo: ./gsea -c|-d|-e|-u -i <input> -o <output> [-k key] [--comp-alg name] [--enc-alg name] [--max-threads N]\n";
        return 1;
    }

    Config cfg;
    std::string input_path, output_path;
    size_t max_threads = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 4;

    // Capturar argumentos en orden simple (left-to-right)
    for (int i = 1; i < argc; ++i) {
        std::string s = argv[i];
        if (s == "-c") cfg.ops.push_back('c');
        else if (s == "-d") cfg.ops.push_back('d');
        else if (s == "-e") cfg.ops.push_back('e');
        else if (s == "-u") cfg.ops.push_back('u');
        else if (s == "-i" && i + 1 < argc) { input_path = argv[++i]; }
        else if (s == "-o" && i + 1 < argc) { output_path = argv[++i]; }
        else if (s == "--comp-alg" && i + 1 < argc) { cfg.comp_alg = argv[++i]; }
        else if (s == "--enc-alg" && i + 1 < argc) { cfg.enc_alg = argv[++i]; }
        else if (s == "-k" && i + 1 < argc) {
            std::string ks = argv[++i];
            cfg.key.assign(ks.begin(), ks.end());
        }
        else if (s == "--max-threads" && i + 1 < argc) {
            max_threads = std::stoul(argv[++i]);
        }
        // else: ignore unknown options for now
    }

    if (cfg.ops.empty()) {
        std::cerr << "No se indicaron operaciones (-c,-d,-e,-u).\n";
        return 1;
    }
    if (input_path.empty() || output_path.empty()) {
        std::cerr << "Debe especificar -i <input> y -o <output>\n";
        return 1;
    }

    // Recolectar archivos
    std::vector<std::string> files;
    collect_files_recursive(input_path, files);
    if (files.empty()) {
        std::cerr << "No se encontraron archivos en: " << input_path << "\n";
        return 1;
    }

    bool input_is_dir = path_is_dir(input_path);
    bool output_is_dir = path_is_dir(output_path) || (!output_path.empty() && output_path.back() == '/');
    if (input_is_dir && !output_is_dir) {
        if (path_exists(output_path) && !path_is_dir(output_path)) {
            std::cerr << "Si la entrada es un directorio, -o debe ser un directorio o no existir.\n";
            return 1;
        }
        ensure_dir_recursive(output_path);
        output_is_dir = true;
    }

    ThreadLimiter limiter(max_threads);
    std::vector<std::thread> pool;
    pool.reserve(files.size());

    for (const auto &infile : files) {
        // calcular ruta relativa si entrada es directorio
        std::string rel;
        if (input_is_dir) {
            std::string base = input_path;
            if (!base.empty() && base.back() == '/') base.pop_back();
            if (infile.size() >= base.size() && infile.compare(0, base.size(), base) == 0) {
                rel = infile.substr(base.size());
                if (!rel.empty() && rel[0] == '/') rel.erase(0,1);
            } else {
                rel = infile;
            }
        }

        std::string outpath;
        if (output_is_dir) {
            if (rel.empty()) {
                std::string fname = infile.substr(infile.find_last_of('/') + 1);
                outpath = path_join(output_path, fname);
            } else {
                outpath = path_join(output_path, rel);
            }
        } else {
            if (files.size() == 1) outpath = output_path;
            else {
                std::string fname = infile.substr(infile.find_last_of('/') + 1);
                outpath = output_path + "_" + fname;
            }
        }

        // lanzar thread limitado
        limiter.enter();
        pool.emplace_back([infile, outpath, cfg, &limiter]() mutable {
            process_file(infile, outpath, cfg);
            limiter.leave();
        });
    }

    for (auto &t : pool) if (t.joinable()) t.join();

    std::cout << "Procesados: " << files.size() << " archivos\n";
    return 0;
}
