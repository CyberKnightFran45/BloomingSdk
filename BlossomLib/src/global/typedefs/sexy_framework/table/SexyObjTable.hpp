#pragma once

#include "../obj/SexyObj.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <nlohmann/json.hpp>

class SexyObjTable 
{
public:
    std::string comment;
    uint32_t version = 1;
    std::vector<SexyObj> objects;

    std::string filepath;

    SexyObjTable() = default;

    SexyObjTable(const std::vector<SexyObj>& objs)
        : objects(objs) {}

    SexyObjTable(const std::string& comment, uint32_t ver, const std::vector<SexyObj>& objs)
        : comment(comment), version(ver), objects(objs) {}

    void CheckForNullFields() {
        if (objects.empty())
            objects = {};
    }

    void CheckObjs() {
        CheckForNullFields();
    }

    SexyObjTable ReadObject(const std::string& sourcePath = "") {
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

        SexyObjTable target;
        target.filepath = path;
        target.comment = j.value("#comment", "");
        target.version = j.value("version", 1u);

        if (j.contains("objects") && j["objects"].is_array()) {
            for (const auto& entry : j["objects"]) {
                SexyObj obj;
                obj.comment = entry.value("#comment", "");
                obj.aliases = entry.value("aliases", std::vector<std::string>{});
                obj.objclass = entry.value("objclass", "MyClassTemplate");
                obj.objdata = entry.value("objdata", nlohmann::json::object());
                target.objects.push_back(obj);
            }
        }

        return target;
    }

    void WriteObject(const std::string& targetPath) const {
        std::filesystem::path dir = std::filesystem::path(targetPath).parent_path();
        if (!std::filesystem::exists(dir))
            std::filesystem::create_directories(dir);

        nlohmann::json j;
        j["#comment"] = comment;
        j["version"] = version;
        j["objects"] = nlohmann::json::array();

        for (const auto& obj : objects) {
            nlohmann::json jobj;
            jobj["#comment"] = obj.comment;
            jobj["aliases"] = obj.aliases;
            jobj["objclass"] = obj.objclass;
            jobj["objdata"] = obj.objdata;
            j["objects"].push_back(jobj);
        }

        std::ofstream outFile(targetPath);
        if (!outFile)
            throw std::runtime_error("Could not write to file: " + targetPath);

        outFile << j.dump(4);
    }
};