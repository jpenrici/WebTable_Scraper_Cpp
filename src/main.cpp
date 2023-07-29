#include "customScraper.h"

#include <filesystem>
#include <iomanip>
#include <iostream>

int main(int argc, char *argv[])
{
    std::string url{};
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg{argv[i]};
            std::string option{"url="};
            if (arg.starts_with(option) && arg.size() > option.size()) {
                url = arg.substr(option.size());
                break;
            }
        }
    }
    else {
        std::string filename = std::filesystem::path(argv[0]).filename();
        std::cout << "Usage: " << filename << " url=<url or file>\n";
    }

    std::cout << "URL: " << std::quoted(url) << '\n';
    if (!url.empty()) {
        CustomScraper cs(url);
        if (cs.empty()) {
            std::cout << "Nothing found!\n";
        }
        else {
            auto text = cs.toStr(';');
            cs.save(text);

            std::cout << "Found: " << cs.count() << "\n";
            std::cout << "Data:\n" << text << "\n";
        }
    }
    else {
        std::cerr << "There was something wrong!\n";
    }

    return 0;
}
