#include "xor_cipher.h"
#include <vector>

// Implementación del cifrado XOR (repetición de clave).
// Nota: encriptar y desencriptar es la misma operación.
std::vector<unsigned char> xor_crypt(const std::vector<unsigned char> &data, const std::vector<unsigned char> &key) {
    if (key.empty()) return data; // si la clave está vacía, no hacemos nada
    std::vector<unsigned char> out(data.size());
    size_t k = key.size();
    for (size_t i = 0; i < data.size(); ++i) {
        out[i] = data[i] ^ key[i % k];
    }
    return out;
}