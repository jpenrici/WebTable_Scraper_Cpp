#include "customScraper.h"

#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

#include <fstream>

CustomScraper::CustomScraper(std::string url)
{
    tables = scraper(getRequest(url), "//table");
}

std::string CustomScraper::getRequest(std::string url)
{
    std::string htmlDocument{};

    auto writer = [](char *data, size_t size, size_t nmemb, std::string * writeData)->size_t {
        if (writeData == NULL)
        {
            return 0;
        }
        static_cast<std::string *>(writeData)->append(data, size * nmemb);
        return size * nmemb;
    };

    CURL *curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                         static_cast<size_t(*)(char *, size_t, size_t, std::string *)>(writer));
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlDocument);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return htmlDocument;
}

std::vector<std::vector<std::string> > CustomScraper::scraper(std::string htmlDocument,
                                                              const char *xpath)
{
    if (htmlDocument.empty() || xpath == NULL) {
        return {{}};
    }

    // Search keyword
    std::vector<std::vector<std::string> > contents;
    htmlDocPtr doc = htmlReadMemory(htmlDocument.c_str(), htmlDocument.length(),
                                    nullptr, nullptr, HTML_PARSE_NOERROR);
    if (doc != NULL) {
        xmlXPathContextPtr ctxt = xmlXPathNewContext(doc);
        if (ctxt != NULL) {
            xmlXPathObjectPtr xpObj = xmlXPathEvalExpression((xmlChar *)xpath, ctxt);
            if (xpObj != NULL) {
                xmlNodeSetPtr nodeset = xpObj->nodesetval;
                if (!xmlXPathNodeSetIsEmpty(nodeset)) {
                    std::vector<std::string> values;
                    for (int i = 0; i < xmlXPathNodeSetGetLength(nodeset); ++i) {
                        xmlNodePtr root = xmlXPathNodeSetItem(nodeset, i);
                        if (root != NULL) {
                            xmlNodePtr node = NULL;
                            for (node = root->children; node; node = node->next) {
                                values.push_back((const char *)xmlNodeGetContent(node));
                            }
                        }
                    }
                    contents.push_back(values);
                }
            }
            xmlXPathFreeObject(xpObj);
        }
        xmlXPathFreeContext(ctxt);
    }
    xmlFreeDoc(doc);

    // Process output
    std::vector<std::vector<std::string> > result;
    for (auto &content : contents) {

        for (auto &chars : content) {
            std::vector<std::string> values;
            std::string str{};
            for (unsigned i = 0; i < chars.size(); i++) {
                // Split data
                if (chars[i] == '\n' && !str.empty()) {
                    unsigned start = 0;
                    unsigned finish = str.size() - 1;
                    finish = finish < 0 ? 0 : finish;
                    // Remove spaces to the left
                    while (start < str.size()) {
                        if (str[++start] != char(32)) {
                            break;
                        }
                    }
                    // Remove spaces to the right
                    while (finish > 0) {
                        if (str[--finish] != char(32)) {
                            break;
                        }
                    }
                    // Substring
                    str = str.substr(start, finish);
                    values.push_back(str);
                    str.clear();
                }
                else {
                    str.push_back(chars[i]);
                }
            }
            // Store
            if (!values.empty()) {
                result.push_back(values);
            }
        }
    }

    return result;
}

bool CustomScraper::save(std::string text, std::filesystem::path path)
{
    if (text.empty() || path.empty()) {
        return false;
    }

    try {
        std::ofstream file(path, std::ios::out);
        file << text;
        file.close();
    }
    catch (...) {
        return false;
    }

    return true;
}

std::vector<std::string> CustomScraper::getTable(unsigned index)
{
    return index < 0 && index >= tables.size() ? std::vector<std::string> {} :
               tables[index];
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

std::string CustomScraper::toStr(int index)
{
    return toStr(',', index);
}
