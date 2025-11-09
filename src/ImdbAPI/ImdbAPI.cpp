#include "ImdbAPI.h"
#include <stdexcept>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

TmdbAPI::TmdbAPI(std::string apiKey)
    : apiKey_(std::move(apiKey)),
      baseUrl_("https://api.themoviedb.org/3") {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

TmdbAPI::~TmdbAPI() {
    curl_global_cleanup();
}

Movie TmdbAPI::fetchMovieById(const int tmdbId) const {
    const std::string url = baseUrl_ +
        "/movie/" + std::to_string(tmdbId) +
        "?api_key=" + apiKey_ +
        "&language=en-US";

    nlohmann::json j = getJson(url);

    Movie m;
    m.tmdbId = j.value("id", tmdbId);
    m.name   = j.value("title", "");

    if (j.contains("genres") && j["genres"].is_array()) {
        for (const auto& g : j["genres"]) {
            if (const auto& name = g["name"]; name.is_string()) {
                m.genres.push_back(name.get<std::string>());
            }
        }
    }

    if (j.contains("vote_average") && j["vote_average"].is_number()) {
        m.rating = j["vote_average"].get<double>();
    }

    if (j.contains("release_date") && j["release_date"].is_string()) {
        if (const std::string date = j["release_date"].get<std::string>(); date.size() >= 4) {
            m.year = std::stoi(date.substr(0, 4));
        }
    }

    return m;
}

std::vector<Movie> TmdbAPI::fetchPopularMovies(const int poolSize) const {
    std::vector<Movie> result;
    if (poolSize <= 0) return result;

    constexpr int perPage = 20;
    const int maxPages = (poolSize + perPage - 1) / perPage;

    const auto genreMap = fetchGenreMap();


    for (int page = 1; page <= maxPages; ++page) {
        const std::string url = baseUrl_ +
            "/movie/popular" +
            "?api_key=" + apiKey_ +
            "&language=en-US" +
            "&page=" + std::to_string(page);

        nlohmann::json j = getJson(url);
        if (!j.contains("results") || !j["results"].is_array()) break;

        for (const auto& r : j["results"]) {
            constexpr double MIN_POPULARITY = 10.0;
            constexpr int MIN_VOTE_COUNT = 300;
            if (static_cast<int>(result.size()) >= poolSize) break;

            const int    voteCount  = r.value("vote_count", 0);
            if (const double popularity = r.value("popularity", 0.0); voteCount < MIN_VOTE_COUNT || popularity < MIN_POPULARITY) {
                continue;
            }

            Movie m;
            m.tmdbId = r.value("id", 0);
            m.name   = r.value("title", "");
            if (m.tmdbId == 0 || m.name.empty()) {
                continue;
            }

            if (r.contains("vote_average") && r["vote_average"].is_number()) {
                m.rating = r["vote_average"].get<double>();
            }

            if (r.contains("release_date") && r["release_date"].is_string()) {
                if (const std::string date = r["release_date"].get<std::string>(); date.size() >= 4) {
                    m.year = std::stoi(date.substr(0, 4));
                }
            }

            if (r.contains("genre_ids") && r["genre_ids"].is_array()) {
                for (const auto& gidJson : r["genre_ids"]) {
                    const int gid = gidJson.get<int>();
                    if (auto it = genreMap.find(gid); it != genreMap.end()) {
                        m.genres.push_back(it->second);
                    }
                }
            }

            result.push_back(std::move(m));
        }

        if (static_cast<int>(result.size()) >= poolSize) break;
    }

    return result;
}

std::vector<Movie> TmdbAPI::searchMoviesByTitle(const std::string& query,
                                                const int limit) const {
    std::vector<Movie> out;
    if (query.empty() || limit <= 0) return out;

    const std::string url = baseUrl_ +
        "/search/movie" +
        "?api_key=" + apiKey_ +
        "&language=en-US" +
        "&include_adult=false" +
        "&page=1" +
        "&query=" + urlEncode(query);

    nlohmann::json j = getJson(url);
    if (!j.contains("results") || !j["results"].is_array()) return out;

    for (const auto& r : j["results"]) {
        if (static_cast<int>(out.size()) >= limit) break;

        Movie m;
        m.tmdbId = r.value("id", 0);
        m.name   = r.value("title", "");
        if (m.tmdbId == 0 || m.name.empty()) continue;

        if (r.contains("vote_average") && r["vote_average"].is_number()) {
            m.rating = r["vote_average"].get<double>();
        }

        if (r.contains("release_date") && r["release_date"].is_string()) {
            const std::string date = r["release_date"].get<std::string>();
            if (date.size() >= 4) {
                m.year = std::stoi(date.substr(0, 4));
            }
        } else {
            continue;
        }

        out.push_back(std::move(m));
    }

    return out;
}

size_t TmdbAPI::writeCallback(const char* ptr,
                              const size_t size,
                              const size_t nmemb,
                              void* userdata) {
    auto* stream = static_cast<std::string*>(userdata);
    const size_t count = size * nmemb;
    stream->append(ptr, count);
    return count;
}

nlohmann::json TmdbAPI::getJson(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("curl_easy_init failed");
    }

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &TmdbAPI::writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    const CURLcode res = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error(
            std::string("curl_easy_perform failed: ") +
            curl_easy_strerror(res));
    }

    if (httpCode < 200 || httpCode >= 300) {
        throw std::runtime_error(
            "TMDB HTTP error " + std::to_string(httpCode) +
            " | body: " + response);
    }

    try {
        return nlohmann::json::parse(response);
    } catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("JSON parse failed: ") + e.what() +
            " | body: " + response);
    }
}

std::string TmdbAPI::urlEncode(const std::string& s) {
    static auto hex = "0123456789ABCDEF";
    std::string out;
    for (const unsigned char c : s) {
        if (('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            out += c;
        } else if (c == ' ') {
            out += "%20";
        } else {
            out += '%';
            out += hex[c >> 4];
            out += hex[c & 0xF];
        }
    }
    return out;
}

std::unordered_map<int, std::string> TmdbAPI::fetchGenreMap() const {
    const std::string url = baseUrl_ +
        "/genre/movie/list" +
        "?api_key=" + apiKey_ +
        "&language=en-US";

    nlohmann::json j = getJson(url);

    std::unordered_map<int, std::string> map;
    if (j.contains("genres") && j["genres"].is_array()) {
        for (const auto& g : j["genres"]) {
            const int id = g.value("id", 0);
            if (const std::string name = g.value("name", ""); id != 0 && !name.empty()) {
                map[id] = name;
            }
        }
    }
    return map;
}
