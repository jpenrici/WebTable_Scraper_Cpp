#include "customScraper.h"

#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

CustomScraper::CustomScraper(std::string url)
{
    tables = scrapeText(getRequest(url));
}

std::string CustomScraper::getRequest(std::string url)
{
    std::string htmlDocument{};

    auto writer = [](char *data, size_t size, size_t nmemb,
                     std::string *writeData)->size_t {
        if (writeData == NULL) {
            return 0;
        }
        static_cast<std::string*>(writeData)->append(data, size * nmemb);
        return size * nmemb;
    };

    CURL *curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                         static_cast<size_t(*)(char*, size_t, size_t, std::string*)>(writer));
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlDocument);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return htmlDocument;
}

std::vector<std::vector<std::string> > CustomScraper::scrapeText(std::string htmlDocument)
{
    if (htmlDocument.empty()) {
        return {{}};
    }

    htmlDocPtr doc = htmlReadMemory(htmlDocument.c_str(), htmlDocument.length(),
                                    nullptr, nullptr, HTML_PARSE_NOERROR);
    xmlXPathContextPtr ctx = xmlXPathNewContext(doc);

    const char* str = "TO DO";
    xmlXPathObjectPtr htmlElement = xmlXPathEvalExpression((xmlChar *)str, ctx);

    // TO DO

    xmlXPathFreeContext(ctx);
    xmlFreeDoc(doc);

    return {{}};
}

bool CustomScraper::save(std::string text, std::filesystem::path path)
{
    // TO DO.

    return true;
}

std::vector<std::string> CustomScraper::getTable(unsigned index)
{
    return index < 0 && index >= tables.size() ? std::vector<std::string>{} : tables[index];
}

bool CustomScraper::empty()
{
    return tables.empty();
}

unsigned CustomScraper::count()
{
    return tables.size();
}

// index < 0 : return all tables,
// index > vector size : return empty.
std::string CustomScraper::toStr(const char delimiter, int index)
{
    std::string result{};
    for (unsigned i = 0; i < tables.size(); ++i) {
        if (i == index || index < 0) {
            for (unsigned j = 0; j < tables[i].size(); ++j) {
                result += tables[i][j] + (j < tables[i].size() - 1 ? std::string{delimiter} : "");
            }
            result += (i < tables.size() - 1) ? "\n" : "";
        }
    }

    return result;
}

// index < 0 : return all tables,
// index > vector size : return empty.
std::string CustomScraper::toStr(int index)
{
    return toStr(',', index);
}
