#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <regex>
#include "deps/json.hpp"
// Define the Entry struct
struct Entry
{
    std::string text;
    std::chrono::system_clock::time_point timestamp;

    // Default constructor 
    Entry() {}

    // Parameterized constructor
    Entry(const std::string &text, const std::chrono::system_clock::time_point &timestamp) : text(text), timestamp(timestamp) {}
};

// Define the EternalEntry struct, inheriting from Entry
struct EternalEntry : public Entry
{
    // Default constructor 
    EternalEntry() {}

    // Inherit Entry's parameterized constructor
    using Entry::Entry;
};

// Define the TransientEntry struct, inheriting from Entry
struct TransientEntry : public Entry
{
    // Default constructor
    TransientEntry() {}

    // Inherit Entry's parameterized constructor
    using Entry::Entry;
};

// All these json functions are for nlohmann to know how to serialize and deserialize the structs

// JSON deserialization for EternalEntry
void from_json(const nlohmann::json &j, EternalEntry &e)
{
    j.at("text").get_to(e.text);
    e.timestamp = std::chrono::system_clock::from_time_t(j.at("timestamp").get<std::time_t>());
}

// JSON deserialization for TransientEntry
void from_json(const nlohmann::json &j, TransientEntry &e)
{
    j.at("text").get_to(e.text);
    e.timestamp = std::chrono::system_clock::from_time_t(j.at("timestamp").get<std::time_t>());
}

// JSON serialization for EternalEntry
void to_json(nlohmann::json &j, const EternalEntry &e)
{
    j = nlohmann::json{{"text", e.text}, {"timestamp", std::chrono::system_clock::to_time_t(e.timestamp)}};
}

// JSON serialization for TransientEntry
void to_json(nlohmann::json &j, const TransientEntry &e)
{
    j = nlohmann::json{{"text", e.text}, {"timestamp", std::chrono::system_clock::to_time_t(e.timestamp)}};
}

class DataManager
{
public:
    // singleton pattern
    static DataManager &getInstance()
    {
        static DataManager instance;
        return instance;
    }

    DataManager(const DataManager &) = delete;
    DataManager &operator=(const DataManager &) = delete;

    void loadData()
    {
        ensureDataFolderExists();

        for (const auto &entry : std::filesystem::directory_iterator(DATA_FOLDER))
        {
            if (entry.path().filename() == BACKUP_FOLDER.substr(BACKUP_FOLDER.find_last_of("/") + 1))
                continue;

            std::ifstream file(entry.path());
            if (!file.is_open())
                continue;

            std::string person = entry.path().stem().string();
            nlohmann::json j;
            file >> j;

            eternalData[person] = j["eternal"].get<std::vector<EternalEntry>>();
            transientData[person] = j["transient"].get<std::vector<TransientEntry>>();

            file.close();
        }
    }

    void saveData(const std::string &person)
    {
        ensureDataFolderExists();
        backupData(person);

        nlohmann::json j;
        j["eternal"] = eternalData[person];
        j["transient"] = transientData[person];

        std::ofstream file(DATA_FOLDER + "/" + person + ".json");
        if (!file.is_open())
        {
            std::cerr << "Error saving data for " << person << std::endl;
            return;
        }

        file << j.dump(4);
        file.close();
    }

    const std::unordered_map<std::string, std::vector<EternalEntry>> &getEternalData() const
    {
        return eternalData;
    }

    const std::unordered_map<std::string, std::vector<TransientEntry>> &getTransientData() const
    {
        return transientData;
    }

    void addEternalData(const std::string &person, const EternalEntry &data)
    {
        eternalData[person].push_back(data);
        saveData(person);
    }

    void addTransientData(const std::string &person, const TransientEntry &data)
    {
        transientData[person].push_back(data);
        saveData(person);
    }

private:
    const std::string DATA_FOLDER = "data";
    const std::string BACKUP_FOLDER = DATA_FOLDER + "/backups";
    const int MAX_BACKUPS = 5;
    std::unordered_map<std::string, std::vector<EternalEntry>> eternalData;
    std::unordered_map<std::string, std::vector<TransientEntry>> transientData;

    DataManager()
    {
        ensureDataFolderExists();
        loadData();
    }

    void ensureDataFolderExists()
    {
        if (!std::filesystem::exists(DATA_FOLDER))
        {
            std::filesystem::create_directory(DATA_FOLDER);
        }
        if (!std::filesystem::exists(BACKUP_FOLDER))
        {
            std::filesystem::create_directory(BACKUP_FOLDER);
        }
    }

    void backupData(const std::string &person)
    {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now), "%Y%m%d%H%M%S");
        std::string timestamp = ss.str();

        std::string backupFilePath = BACKUP_FOLDER + "/" + person + "_" + timestamp + ".json";
        std::ifstream src(DATA_FOLDER + "/" + person + ".json", std::ios::binary);
        std::ofstream dst(backupFilePath, std::ios::binary);
        dst << src.rdbuf();

        // Keep only the 5 most recent backups
        std::vector<std::filesystem::directory_entry> backups;
        for (const auto &entry : std::filesystem::directory_iterator(BACKUP_FOLDER))
        {
            if (entry.path().stem().string().find(person) == 0)
            {
                backups.push_back(entry);
            }
        }
        std::sort(backups.begin(), backups.end(), [](const auto &a, const auto &b)
                  { return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b); });

        for (size_t i = MAX_BACKUPS; i < backups.size(); ++i)
        {
            std::filesystem::remove(backups[i]);
        }
    }
};

void listPeople()
{
    const auto &dataManager = DataManager::getInstance();
    const auto &eternalData = dataManager.getEternalData();
    const auto &transientData = dataManager.getTransientData();

    if (eternalData.empty() && transientData.empty())
    {
        return;
    }

    for (const auto &entry : eternalData)
    {
        std::cout << "  - " << entry.first << std::endl;
    }
    for (const auto &entry : transientData)
    {
        if (eternalData.find(entry.first) == eternalData.end())
        {
            std::cout << "  - " << entry.first << std::endl;
        }
    }
}

void showPersonInfo(const std::string &person)
{
    const auto &dataManager = DataManager::getInstance();
    const auto &eternalData = dataManager.getEternalData();
    const auto &transientData = dataManager.getTransientData();

    std::cout << "Eternal: ";
    auto it1 = eternalData.find(person);
    if (it1 != eternalData.end() && !it1->second.empty())
    {
        std::cout << it1->second.back().text << std::endl;
    }

    std::cout << std::endl;

    std::cout << "Transient: " << std::endl;
    auto it2 = transientData.find(person);
    if (it2 != transientData.end())
    {
        for (auto it = it2->second.rbegin(); it != it2->second.rend(); ++it)
        {
            std::time_t time = std::chrono::system_clock::to_time_t(it->timestamp);
            std::cout << "  " << std::put_time(std::localtime(&time), "%Y-%m-%d") << ": " << it->text << std::endl;
        }
    }

    std::cout << std::endl;
}

void editEternalInfo(const std::string &person, const std::string &entry)
{
    if (entry.find_first_not_of(" \t\n\v\f\r") == std::string::npos)
    {
        return;
    }
    auto &dataManager = DataManager::getInstance();
    EternalEntry formattedEntry = {entry, std::chrono::system_clock::now()};
    dataManager.addEternalData(person, formattedEntry);
    showPersonInfo(person);
}

void addTransientEntry(const std::string &person, const std::string &entry)
{
    if (entry.find_first_not_of(" \t\n\v\f\r") == std::string::npos)
    {
        return;
    }
    auto &dataManager = DataManager::getInstance();
    TransientEntry formattedEntry = {entry, std::chrono::system_clock::now()};
    dataManager.addTransientData(person, formattedEntry);
    showPersonInfo(person);
}

void printUsage()
{
    // Print usage instructions
    std::cout << "\nUsage: \n";
    std::cout << "  program_name\n";
    std::cout << "  program_name  <person>\n";
    std::cout << "  program_name  <person> <command> <entry>\n";
    std::cout << "Commands:\n";
    std::cout << "  eternal, e    Edit eternal info\n";
    std::cout << "  transient, t  Add transient entry\n\n";
}

int main(int argc, char *argv[])
{
    DataManager &dataManager = DataManager::getInstance();
    std::regex nameRegex("^[a-zA-Z]+$");

    if (argc == 1)
    {
        listPeople();
    }
    else if (argc == 2)
    {
        std::string person = argv[1];
        if (!std::regex_match(person, nameRegex))
        {
            std::cout << "Invalid person" << std::endl;
            printUsage();
            return 1;
        }
        showPersonInfo(person);
    }
    else if (argc >= 4)
    {
        std::string person = argv[1];
        if (!std::regex_match(person, nameRegex))
        {
            std::cout << "Invalid person" << std::endl;
            printUsage();
            return 1;
        }
        std::string command = argv[2];
        std::string entry;

        for (int i = 3; i < argc; ++i)
        {
            if (i > 3)
            {
                entry += " ";
            }
            entry += argv[i];
        }

        if (command == "eternal" || command == "e")
        {
            editEternalInfo(person, entry);
        }
        else if (command == "transient" || command == "t")
        {
            addTransientEntry(person, entry);
        }
        else
        {
            std::cout << "Invalid command." << std::endl;
            printUsage();
        }
    }
    else
    {
        printUsage();
    }

    return 0;
}
