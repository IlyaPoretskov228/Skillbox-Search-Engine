#include "converter_json.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>

using json = nlohmann::json;

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::ifstream config_file("config.json");
    if (!config_file) {
        throw std::runtime_error("config file is missing");
    }
    json config_json;
    config_file >> config_json;
    if (config_json.empty() || !config_json.contains("config")) {
        throw std::runtime_error("config file is empty");
    }
    auto config = config_json["config"];
    if (!config.contains("name") || !config.contains("version") || !config.contains("max_responses")) {
        throw std::runtime_error("config file missing required fields");
    }
    if (config["version"].get<std::string>() != "0.1") {
        throw std::runtime_error("config.json has incorrect file version");
    }

    std::cout << "Starting " << config["name"].get<std::string>() << std::endl;

    if (!config_json.contains("files") || !config_json["files"].is_array()) {
        throw std::runtime_error("config file missing files field");
    }

    std::vector<std::string> documents;
    for (auto &file_path : config_json["files"]) {
        std::string path = file_path.get<std::string>();
        std::ifstream in(path);
        if (!in) {
            std::cerr << "Error: file " << path << " not found." << std::endl;
            continue;
        }
        std::stringstream buffer;
        buffer << in.rdbuf();
        documents.push_back(buffer.str());
    }
    return documents;
}

int ConverterJSON::GetResponsesLimit() {
    std::ifstream config_file("config.json");
    if (!config_file) {
        throw std::runtime_error("config file is missing");
    }
    json config_json;
    config_file >> config_json;
    if (config_json.empty() || !config_json.contains("config")) {
        throw std::runtime_error("config file is empty");
    }
    auto config = config_json["config"];
    if (!config.contains("max_responses")) {
        return 5;
    }
    return config["max_responses"].get<int>();
}

std::vector<std::string> ConverterJSON::GetRequests() {
    std::ifstream req_file("requests.json");
    if (!req_file) {
        throw std::runtime_error("requests.json file is missing");
    }
    json req_json;
    req_file >> req_json;
    if (!req_json.contains("requests") || !req_json["requests"].is_array()) {
        throw std::runtime_error("requests.json is empty or missing 'requests'");
    }
    std::vector<std::string> requests;
    for (auto &req : req_json["requests"]) {
        requests.push_back(req.get<std::string>());
    }
    return requests;
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<int,float>>> &answers) {
    json ans_json;
    ans_json["answers"] = json::object();

    for (size_t i = 0; i < answers.size(); i++) {
        std::string request_id = "request" +
            std::string((i < 9) ? "00" : (i < 99 ? "0" : "")) +
            std::to_string(i + 1);

        if (answers[i].empty()) {
            ans_json["answers"][request_id] = {{"result", "false"}};
        } else {
            json relevance = json::array();
            for (auto &p : answers[i]) {
                json item;
                item["docid"] = p.first;
                item["rank"] = p.second;
                relevance.push_back(item);
            }
            ans_json["answers"][request_id] = {
                {"result", "true"},
                {"relevance", relevance}
            };
        }
    }

    std::ofstream out("answers.json");
    out << std::setw(4) << ans_json;
    out.close();
}
