#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <zlib.h>
#include <openssl/sha.h>
#include "hash_object.h"
#include "utils.h"

// this is the sha1 hash of the file
std::string hash_object(const std::string &filepath)
{
    // Read file contents
    std::ifstream t(filepath, std::ios::binary);
    if (!t.is_open())
    {
        std::cerr << "Could not open file " << filepath << std::endl;
        return "";
    }
    std::stringstream data;
    data << t.rdbuf();
    if (data.str().empty())
    {
        std::cerr << "fatal: Unable to hash, File is empty: " << filepath << std::endl;
        return "";
    }
    // if the filepath points to a directory
    if (std::filesystem::is_directory(filepath))
    {
        std::cerr << "fatal: Not a valid file: " << filepath << std::endl;
        return "";
    }
    // Create blob content string
    std::string content = "blob " + std::to_string(data.str().length()) + '\0' + data.str();

    // Calculate SHA1 hash
    std::string buffer = sha_file(content);
    if (buffer.length() != 40)
    {
        std::cerr << "Invalid SHA1 hash length: " << buffer.length() << std::endl;
        return "";
    }

    // Compress blob content
    uLong bound = compressBound(content.size());
    unsigned char compressedData[bound];
    compressFile(content, &bound, compressedData);

    // Write compressed data to .git/objects
    std::string dir = ".git/objects/" + buffer.substr(0, 2);
    if (!std::filesystem::exists(dir))
        std::filesystem::create_directory(dir);
    std::string objectPath = dir + "/" + buffer.substr(2);
    std::ofstream objectFile(objectPath, std::ios::binary);
    if (!objectFile.is_open())
    {
        std::cerr << "Could not create file " << objectPath << std::endl;
        return "";
    }
    objectFile.write((char *)compressedData, bound);
    objectFile.close();
    return buffer;
}
