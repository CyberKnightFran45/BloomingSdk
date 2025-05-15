#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <nlohmann/json.hpp>

class SexyObj 
{
public:

    std::string comment;
    std::vector<std::string> aliases;
    std::string objclass = "MyClassTemplate";
    nlohmann::json objdata = nlohmann::json::object();

    std::string filepath;

    SexyObj() = default;

    SexyObj(const std::string& comment,
            const std::vector<std::string>& aliases,
            const std::string& objclass)
        : comment(comment), aliases(aliases), objclass(objclass) {}

    SexyObj(const std::string& comment,
            const std::vector<std::string>& aliases,
            const std::string& objclass,
            const nlohmann::json& objdata)
        : comment(comment), aliases(aliases), objclass(objclass), objdata(objdata) {}

    SexyObj ReadObject(const std::string& sourcePath = "") {
        std::string path = !sourcePath.empty() ? sourcePath : filepath;

        if (path.empty())
            throw std::runtime_error("No source path provided for ReadObject().");

        std::filesystem::path dir = std::filesystem::path(path).parent_path();
        if (!std::filesystem::exists(dir))
            std::filesystem::create_directories(dir);

        if (!std::filesystem::exists(path) || std::filesystem::file_size(path) == 0) {
            WriteObject(path);
        }

        std::ifstream inFile(path);
        if (!inFile)
            throw std::runtime_error("Could not open file: " + path);

        nlohmann::json j;
        inFile >> j;

        SexyObj targetObj;
        targetObj.filepath = path;
        targetObj.comment = j.value("#comment", "");
        targetObj.aliases = j.value("aliases", std::vector<std::string>{});
        targetObj.objclass = j.value("objclass", "MyClassTemplate");

        if (j.contains("objdata") && j["objdata"].is_object())
            targetObj.objdata = j["objdata"];
        else
            targetObj.objdata = nlohmann::json::object();

        return targetObj;
    }

    void WriteObject(const std::string& targetPath) const {
        std::filesystem::path dir = std::filesystem::path(targetPath).parent_path();
        if (!std::filesystem::exists(dir))
            std::filesystem::create_directories(dir);

        nlohmann::json j;
        j["#comment"] = comment;
        j["aliases"] = aliases;
        j["objclass"] = objclass;
        j["objdata"] = objdata;

        std::ofstream outFile(targetPath);
        if (!outFile)
            throw std::runtime_error("Could not write to file: " + targetPath);

        outFile << j.dump(4);
    }
};