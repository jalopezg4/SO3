#include <string>
#include <vector>


// Recolecta archivos regulares recursivamente desde `path`.
void collect_files_recursive(const std::string &path, std::vector<std::string> &out_files);


// Comprueba si la ruta es directorio
bool path_is_dir(const std::string &path);


// Comprueba existencia de ruta
bool path_exists(const std::string &path);


// Asegura que exista una jerarquía de directorios (crea recursivamente)
void ensure_dir_recursive(const std::string &path);


// Lee todo el fichero (posix) y devuelve bytes
std::vector<unsigned char> read_whole_file(const std::string &inpath);


// Escribe todo en fichero (posix)
bool write_whole_file(const std::string &outpath, const std::vector<unsigned char> &data);


// Une paths de forma robusta
std::string path_join(const std::string &a, const std::string &b);