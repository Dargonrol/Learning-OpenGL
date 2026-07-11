#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>
#include <variant>
#include <iostream>
#include <unordered_set>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"


/**
 * Reads a File and saves it contents into a buffer.
 * Note: The lines vector is only valid aslong the object is unchanged.
 */
class Parser
{
public:
    using Value = std::variant<
        std::string,
        int,
        float,
        glm::vec2,
        glm::vec3
    >;

    struct EnumHash {
        template<typename U>
        std::size_t operator()(U e) const noexcept {
            return std::hash<std::underlying_type_t<U>>{}(
                static_cast<std::underlying_type_t<U>>(e)
            );
        }
    };

    explicit Parser(const std::filesystem::path& path);

    void Parse();

    [[nodiscard]] const std::unordered_map<std::string, Value>& GetTokensAndValuesMap() const noexcept;

    [[nodiscard]] const std::string& GetRaw() const noexcept;
    [[nodiscard]] const std::vector<std::string_view>& GetLines() const noexcept;

    template<typename T>
    requires(std::is_enum_v<T>)
    [[nodiscard]] std::unordered_map<T, Parser::Value, Parser::EnumHash> getTokensAsEnum(const std::unordered_map<T, std::string>& enumMap) const noexcept
    {
        std::unordered_map<T, Value, EnumHash> map;
        map.reserve(enumMap.size());
        std::unordered_set<std::string> used;

        for (const auto& [key, value] : enumMap)
        {
            const auto iter = tokens.find(value);
            if (iter == tokens.end())
            {
                continue;
            }
            map.emplace(key, iter->second);
            used.insert(iter->first);
        }

        for (const auto& [k, v] : tokens)
        {
            if (!used.contains(k))
                std::cerr << "[Parser] Unknown token: " << k << "in file: " << path.filename() << "." << path.extension() << "\n";
        }

        return map;
    }

private:
    void parseString(const std::string_view& line, const std::string_view& key, const std::string_view& value);
    void parseVector(const std::string_view& line, const std::string_view& key, const std::string_view& inner);
    void parseScalar(const std::string_view& line, const std::string_view& key, const std::string_view& value);

private:
    std::string buffer;
    std::vector<std::string_view> lines;
    std::unordered_map<std::string, Value> tokens;

    std::filesystem::path path;
};