#include "cat_file.h"

// extern string git_path;

int git_cat_file(int argc, char *argv[])
{
    if (argc <= 3)
    {
        std::cerr << "Invalid arguments, usage: cat-file -p/-t/-s <object_sha>\n";
        return EXIT_FAILURE;
    }
    std::string flag = argv[2];
    if (flag != "-p" && flag != "-t" && flag != "-s")
    {
        std::cerr << "Invalid flag, usage: cat-file -p/-t/-s <object_sha>\n";
        return EXIT_FAILURE;
    }
    // now i am sure with the arg count and the flag
    // the third argument contains the actual file name
    const std::string value = argv[3];
    // the first two characters denote the subfolder name inside the objects folder
    const std::string dir_name = value.substr(0, 2);
    // the rest of the sha1 hash value is the actual file name inside the folder
    const std::string blob_sha = value.substr(2);
    string subdir = "objects";
    const fs::path blob_path = git_dir / subdir / dir_name / blob_sha;
    cout << MAGENTA << __LINE__ << " cat-file.cpp " << git_path << RESET << endl;
    cout << MAGENTA << __LINE__ << " cat-file.cpp " << blob_path << RESET << endl;

    std::ifstream input = std::ifstream(blob_path);
    if (!input.is_open())
    {
        std::cerr << "Failed to open " << blob_path << " file.\n";
        return EXIT_FAILURE;
    }
    const std::string blob_data = std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    // std::cout << blob_data;
    // i have the compressed data of the file which needs to be decompressed using zlib
    string buf = string();
    cout<<RED<<buf<<RESET<<endl;
    buf.resize(blob_data.size());
    while (true)
    {
        auto len = buf.size();
        if (auto res = uncompress((uint8_t *)buf.data(), &len, (const uint8_t *)blob_data.data(), blob_data.size()); res == Z_BUF_ERROR)
        {
            buf.resize(buf.size() * 2);
        }
        else if (res != Z_OK)
        {
            std::cerr << "Failed to uncompress Zlib. (code: " << res << ")\n";
            return EXIT_FAILURE;
        }
        else
        {
            buf.resize(len);
            break;
        }
    }
    // if the object type is tree then call ls-tree

    if (flag == "-p")
    {
        if (buf.find("commit") == 0)
        {
            // std::cout << "Commit object\n";
            // call commit-tree
            show_commit((dir_name + blob_sha).c_str());
            return EXIT_SUCCESS;
        }
        if (buf.find("tree") == 0)
        {
            // std::cout << "Tree object\n";
            // call ls-tree
            ls_tree((dir_name + blob_sha).c_str());
            return EXIT_SUCCESS;
        }
        std::cout << std::string_view(buf).substr(buf.find('\0') + 1) << endl;
    }
    else if (flag == "-t")
    {
        const auto space = buf.find(' ');
        std::cout << std::string_view(buf).substr(0, space) << endl;
    }
    else if (flag == "-s")
        std::cout << buf.size() << '\n';
    return EXIT_SUCCESS;
}
