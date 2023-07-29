#include "customScraper.h"

#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

#include <fstream>

CustomScraper::CustomScraper(std::string url)
{
    const char* xpath = "//table";

    std::string str{};
    if (url.starts_with("http")) {
        str = getRequest(url);
    } else {
        str = fromFile(url);
    }
    if (!str.empty()) {
        data = scraper(str, xpath);
    }
}

std::string CustomScraper::getRequest(std::string url)
{
    std::string htmlDocument{};

    auto writer = [](char *data, size_t size, size_t nmemb, std::string * writeData)->size_t {
        if (writeData == NULL) {
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
                for (int i = 0; i < xpObj->nodesetval->nodeNr; ++i) {
                    std::vector<std::string> values;
                    xmlNodePtr root = xpObj->nodesetval->nodeTab[i];
                    if (root != NULL) {
                        xmlNodePtr node = NULL;
                        for (node = root->children; node; node = node->next) {
                            values.push_back((const char *)xmlNodeGetContent(node));
                        }
                    }
                    if (!values.empty()) {
                        contents.push_back(values);
                    }
                }
            }
            xmlXPathFreeObject(xpObj);
        }
        xmlXPathFreeContext(ctxt);
    }
    xmlFreeDoc(doc);

    return contents;
}

std::string CustomScraper::fromFile(std::string path)
{
    if (path.empty()) {
        return {};
    }

    std::string str{};
    try {
        std::ifstream fileIn(path, std::ios::in);
        if (fileIn.is_open()) {
            std::string line{};
            while(getline(fileIn, line)) {
                str += line + "\n";
            }
            fileIn.close();
        }
    } catch (...) {
        // pass
    }

    return str;
}

bool CustomScraper::save(std::string text, std::string path)
{
    if (text.empty() || path.empty()) {
        return false;
    }

    try {
        std::ofstream fileOut(path, std::ios::out);
        fileOut << text;
        fileOut.close();
    }
    catch (...) {
        return false;
    }

    return true;
}

std::vector<std::string> CustomScraper::getTable(unsigned index)
{
    return index < 0 && index >= data.size() ? std::vector<std::string> {} : data[index];
}

bool CustomScraper::empty()
{
    return data.empty();
}

unsigned CustomScraper::count()
{
    return data.size();
}

// index < 0 : return all tables,
// index > vector size : return empty.
std::string CustomScraper::toStr(const char delimiter, int index)
{
    std::string result{}, str{};
    for (unsigned i = 0; i < data.size(); ++i) {
        if (i == index || index < 0) {
            for (unsigned j = 0; j < data[i].size(); ++j) {
                str = trim(trim(data[i][j], char(32)), '\n');
                if (!str.empty()) {
                    result += join(trim(split(str, '\n'), char(32)), delimiter) + "\n";
                }
            }
            result += (i < data.size() - 1) ? "\n" : "";
        }
    }

    return result;
}

std::string CustomScraper::toStr(int index)
{
    return toStr(',', index);
}

std::string CustomScraper::join(std::vector<std::string> vec, const char delimiter)
{
    std::string str{};
    for(unsigned i = 0; i < vec.size(); i++) {
        str += vec[i] + (i < vec.size() - 1 ? std::string{delimiter} : "");
    }

    return str;
}

std::string CustomScraper::trim(std::string str, const char trimmer)
{
    int left = 0;
    int right = str.size() - 1;
    right = right < 0 ? 0 : right;
    while (left < str.size()) {
        if (str[left] != trimmer) {
            break;
        }
        left++;
    }
    while (right >= 0) {
        if (str[right] != trimmer) {
            break;
        }
        right--;
    }

    return str.substr(left, 1 + right - left);
}

std::vector<std::string> CustomScraper::trim(std::vector<std::string> vec,
                                             const char trimmer)
{
    for(unsigned i = 0; i < vec.size(); ++i) {
        vec[i] = trim(vec[i], trimmer);
    }

    return vec;
}

std::vector<std::string> CustomScraper::split(std::string str, const char delimiter)
{
    std::vector<std::string> result;
    std::string s{};
    for(unsigned i = 0; i < str.size(); ++i) {
        if (str[i] == delimiter) {
            result.push_back(s);
            s = "";
        } else {
            s += str[i];
        }
    }
    if (!s.empty()) {
        result.push_back(s);
    }

    return result;
}
