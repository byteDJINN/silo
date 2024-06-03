#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

namespace fs = std::filesystem;

const std::string DATA_FOLDER = "data";

std::unordered_map<std::string, std::vector<std::string>> eternalData;
std::unordered_map<std::string, std::vector<std::string>> transientData;

void ensureDataFolderExists()
{
    if (!fs::exists(DATA_FOLDER))
    {
        fs::create_directory(DATA_FOLDER);
    }
}

void loadData()
{
    ensureDataFolderExists();

    for (const auto &entry : fs::directory_iterator(DATA_FOLDER))
    {
        std::ifstream file(entry.path());
        if (!file.is_open())
            continue;

        std::string person = entry.path().stem().string();
        std::string line;
        std::vector<std::string> eternal;
        std::vector<std::string> transient;
        bool isTransient = false;

        while (std::getline(file, line))
        {
            if (line.empty() && file.peek() == '\n')
            {
                isTransient = true;
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (isTransient)
            {
                transient.push_back(line);
            }
            else
            {
                eternal.push_back(line);
            }
        }

        eternalData[person] = eternal;
        transientData[person] = transient;
        file.close();
    }
}

void saveData(const std::string &person)
{
    ensureDataFolderExists();
    std::ofstream file(DATA_FOLDER + "/" + person + ".txt");
    if (!file.is_open())
    {
        std::cerr << "Error saving data for " << person << std::endl;
        return;
    }

    for (const auto &info : eternalData[person])
    {
        file << info << std::endl;
    }
    file << std::endl
         << std::endl;
    for (const auto &info : transientData[person])
    {
        file << info << std::endl;
    }

    file.close();
}

void listPeople()
{
    if (eternalData.empty() && transientData.empty())
    {
        std::cout << "  :(" << std::endl;
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
    std::cout << "Eternal: ";
    if (eternalData.find(person) != eternalData.end() && !eternalData[person].empty())
    {
        std::cout << eternalData[person].front();
    }
    else
    {
        std::cout << ":(" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Transient: " << std::endl;
    if (transientData.find(person) != transientData.end())
    {
        for (const auto &info : transientData[person])
        {
            std::cout << "  * " << info << std::endl;
        }
    }
    else
    {
        std::cout << " :(" << std::endl;
    }
}

void editEternalInfo(const std::string &person, std::string &entry)
{
    if(entry.find_first_not_of(' \t\n\v\f\r') == std::string::npos) {
        std::cout << ":(" << std::endl;
        return;
    
    }
    eternalData[person].insert(eternalData[person].begin(), entry);
    saveData(person);
    std::cout << ":)" << std::endl;
}

void addTransientEntry(const std::string &person, std::string &entry)
{
    if(entry.find_first_not_of(" \t\n\v\f\r") == std::string::npos) {
        std::cout << ":(" << std::endl;
        return;
    }
    transientData[person].insert(transientData[person].begin(), entry);
    saveData(person);
    std::cout << ":)" << std::endl;
}

void printUsage()
{
    // Print usage instructions
    std::cout << "Usage: \n";
    std::cout << "  program_name\n";
    std::cout << "  program_name <person>\n";
    std::cout << "  program_name <person> <command> <entry>\n";
    std::cout << "Commands:\n";
    std::cout << "  eternal, e   Edit eternal info\n";
    std::cout << "  transient, t Add transient entry\n";
}

int main(int argc, char *argv[])
{
    loadData();

    if (argc == 1)
    {
        listPeople();
    }
    else if (argc == 2)
    {
        std::string person = argv[1];
        showPersonInfo(person);
    }
    else if (argc >= 4)
    {
        std::string person = argv[1];
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
