#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <nlohmann/json.hpp>

template <class T>
class SexyObjTableT 
{
public:
    std::string comment;
    uint32_t version = 1;
    std::vector<T> objects;

    std::string filepath;

    SexyObjTableT() = default;

    SexyObjTableT(const std::vector<T>& objs)
        : objects(objs) {}

    SexyObjTableT(const std::string& comment, uint32_t ver, const std::vector<T>& objs)
        : comment(comment), version(ver), objects(objs) {}

    virtual void CheckObjs() {
        if (objects.empty())
            objects = {};
    }

    SexyObjTableT<T> ReadObject(const std::string& sourcePath = "") {
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

        SexyObjTableT<T> target;
        target.filepath = path;
        target.comment = j.value("#comment", "");
        target.version = j.value("version", 1u);

        if (j.contains("objects") && j["objects"].is_array()) {
            for (const auto& entry : j["objects"]) {
                T obj;

                // Requiere que T pueda cargarse desde json
                if constexpr (std::is_same_v<decltype(obj.ReadObject), SexyObj (T::*)(const std::string&)>)
                    obj = obj.ReadObject(entry.dump());  // si usa ReadObject desde JSON string
                else if constexpr (std::is_constructible_v<T, const nlohmann::json&>)
                    obj = T(entry);                      // si tiene ctor(json)
                else
                    obj = entry.get<T>();                // si tiene from_json

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
            if constexpr (std::is_same_v<decltype(obj.objclass), std::string>) {
                nlohmann::json jobj;
                jobj["#comment"] = obj.comment;
                jobj["aliases"] = obj.aliases;
                jobj["objclass"] = obj.objclass;
                jobj["objdata"] = obj.objdata;
                j["objects"].push_back(jobj);
            } else {
                j["objects"].push_back(obj);  // requiere que obj pueda convertirse a json
            }
        }

        std::ofstream outFile(targetPath);
        if (!outFile)
            throw std::runtime_error("Could not write to file: " + targetPath);

        outFile << j.dump(4);
    }
};
