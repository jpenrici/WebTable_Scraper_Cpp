#pragma once

#include <filesystem>
#include <string>
#include <vector>

class CustomScraper {
public:

    CustomScraper(std::string url);
    ~CustomScraper() {};

    bool empty();
    bool save(std::string text, std::string path = "output.txt");
    std::string toStr(const char delimiter, int index = -1);
    std::string toStr(int index = -1);
    std::vector<std::string> getTable(unsigned index);
    unsigned count();

private:
    // Data found
    std::vector<std::vector<std::string> > data;

    // CURL
    std::string getRequest(std::string url);

    // Libxml2
    std::vector<std::vector<std::string> > scraper(std::string htmlDocument, const char *xpath);

    // Tools
    std::string fromFile(std::string path);
    std::string join(std::vector<std::string> vec, const char delimiter);
    std::string trim(std::string str, const char trimmer);
    std::vector<std::string> trim(std::vector<std::string> vec, const char trimmer);
    std::vector<std::string> split(std::string str, const char delimiter);
};
