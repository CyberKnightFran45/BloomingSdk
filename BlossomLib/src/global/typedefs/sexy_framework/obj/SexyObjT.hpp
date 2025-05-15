#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

template <class T>
class SexyObjT {
public:
    std::string comment;                   // JSON key: "#comment"
    std::vector<std::string> aliases;     // JSON key: "aliases"
    std::string objclass;                 // JSON key: "objclass"
    T objdata;                            // JSON key: "objdata"

    std::string filepath;                 // Optional source tracking

    // Constructors
    SexyObjT()
        : objclass("MyClassTemplate") {}

    SexyObjT(const std::string& comment,
             const std::vector<std::string>& aliases,
             const std::string& objclass)
        : comment(comment), aliases(aliases), objclass(objclass) {}

    SexyObjT(const std::string& comment,
             const std::vector<std::string>& aliases,
             const std::string& objclass,
             const T& objdata)
        : comment(comment), aliases(aliases), objclass(objclass), objdata(objdata) {}

    // Reads a JSON file and returns an instance of SexyObjT<T>
    SexyObjT<T> ReadObject(const std::string& sourcePath = "") {
        std::string path = !sourcePath.empty() ? sourcePath : filepath;
        if (path.empty()) {
            throw std::runtime_error("No source path provided to ReadObject().");
        }

        std::filesystem::path dir = std::filesystem::path(path).parent_path();
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        if (!std::filesystem::exists(path) || std::filesystem::file_size(path) == 0) {
            WriteObject(path);  // create file with default data
        }

        std::ifstream inFile(path);
        if (!inFile) {
            throw std::runtime_error("Could not open file: " + path);
        }

        nlohmann::json j;
        inFile >> j;

        SexyObjT<T> target;
        target.filepath = path;
        target.comment = j.value("#comment", "");
        target.aliases = j.value("aliases", std::vector<std::string>{});
        target.objclass = j.value("objclass", "MyClassTemplate");
        if (j.contains("objdata"))
            target.objdata = j["objdata"].get<T>();

        return target;
    }

    // Writes the current object to a file as JSON
    void WriteObject(const std::string& targetPath = "") const {
        std::string path = !targetPath.empty() ? targetPath : filepath;
        if (path.empty()) {
            throw std::runtime_error("No target path provided to WriteObject().");
        }

        std::filesystem::path dir = std::filesystem::path(path).parent_path();
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        nlohmann::json j;
        j["#comment"] = comment;
        j["aliases"] = aliases;
        j["objclass"] = objclass;
        j["objdata"] = objdata;

        std::ofstream outFile(path);
        if (!outFile) {
            throw std::runtime_error("Could not write to file: " + path);
        }

        outFile << j.dump(4); // pretty print with 4 spaces
    }
};
