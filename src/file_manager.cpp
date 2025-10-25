#include "file_manager.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

static void perror_log(const std::string &msg) {
    std::cerr << msg << ": " << strerror(errno) << "\n";
}

bool path_exists(const std::string &path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

bool path_is_dir(const std::string &path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

void ensure_dir_recursive(const std::string &path) {
    if (path.empty()) return;
    if (path_exists(path)) return;
    std::string cur;
    if (path[0] == '/') cur = "/";
    std::istringstream ss(path);
    std::string token;
    while (std::getline(ss, token, '/')) {
        if (token.empty()) continue;
        if (!cur.empty() && cur.back() != '/') cur += "/";
        cur += token;
        if (!path_exists(cur)) {
            if (mkdir(cur.c_str(), 0755) != 0) {
                if (errno != EEXIST) perror_log("mkdir");
            }
        }
    }
}

std::string path_join(const std::string &a, const std::string &b) {
    if (a.empty()) return b;
    if (a.back() == '/') return a + b;
    return a + "/" + b;
}

void collect_files_recursive(const std::string &base, std::vector<std::string> &out_files) {
    struct stat st;
    if (stat(base.c_str(), &st) != 0) {
        perror_log("stat");
        return;
    }
    if (S_ISREG(st.st_mode)) {
        out_files.push_back(base);
        return;
    } else if (S_ISDIR(st.st_mode)) {
        DIR *d = opendir(base.c_str());
        if (!d) { perror_log("opendir"); return; }
        struct dirent *ent;
        while ((ent = readdir(d)) != nullptr) {
            std::string name = ent->d_name;
            if (name == "." || name == "..") continue;
            std::string child = path_join(base, name);
            collect_files_recursive(child, out_files);
        }
        closedir(d);
    } else {
        // ignorar otros tipos
    }
}

std::vector<unsigned char> read_whole_file(const std::string &inpath) {
    std::vector<unsigned char> out;
    int fd = open(inpath.c_str(), O_RDONLY);
    if (fd < 0) { perror_log("open"); return out; }
    const size_t CHUNK = 64 * 1024;
    std::vector<unsigned char> buf(CHUNK);
    ssize_t r;
    while ((r = read(fd, buf.data(), CHUNK)) > 0) {
        out.insert(out.end(), buf.begin(), buf.begin() + r);
    }
    if (r < 0) perror_log("read");
    close(fd);
    return out;
}

bool write_whole_file(const std::string &outpath, const std::vector<unsigned char> &data) {
    // crear directorio si es necesario
    size_t pos = outpath.find_last_of('/');
    if (pos != std::string::npos) {
        std::string dir = outpath.substr(0, pos);
        ensure_dir_recursive(dir);
    }
    int fd = open(outpath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror_log("open"); return false; }
    size_t written = 0;
    while (written < data.size()) {
        ssize_t w = write(fd, data.data() + written, data.size() - written);
        if (w < 0) { perror_log("write"); close(fd); return false; }
        written += w;
    }
    close(fd);
    return true;
}