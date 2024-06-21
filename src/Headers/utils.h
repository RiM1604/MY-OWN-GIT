#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <set>
#include <cstdio>
#include <zlib.h>

// example of a tree object:
// 100644 blob 3cf56579491f151d82b384c211cf1971c300fbf8 .dockerignore
// 040000 tree 918756f1a4e5d648ae273801359c440c951555f9    build
struct TreeEntry
{
    std::string mode;
    std::string type;
    std::string hash;
    std::string name;
};

#define CHUNK 16384

std::string sha1_hex(const std::string &input);
std::string hash_digest(const std::string &input);
void compressFile(const std::string &data, uLong *bound, unsigned char *dest);
int decompress(FILE *source, FILE *dest);
std::set<std::string> parse_tree_object(FILE *file);

#endif
