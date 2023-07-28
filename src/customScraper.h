#pragma once

#include <filesystem>
#include <string>
#include <vector>

class CustomScraper {
public:

    CustomScraper(std::string url);
    ~CustomScraper() {};

    bool empty();
    bool save(std::string text, std::filesystem::path path);
    std::string toStr(const char delimiter, int index = -1);
    std::string toStr(int index = -1);
    std::vector<std::string> getTable(unsigned index);
    unsigned count();

private:
    std::vector<std::vector<std::string> > tables;

    std::string getRequest(std::string url);
    std::vector<std::vector<std::string> > scraper(std::string htmlDocument, const char *xpath);
};
