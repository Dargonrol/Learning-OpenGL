#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>
#include <variant>

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
        std::string_view,
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

    [[nodiscard]] const std::unordered_map<std::string_view, Value>& GetTokensAndValuesMap() const noexcept;

    [[nodiscard]] const std::string& GetRaw() const noexcept;
    [[nodiscard]] const std::vector<std::string_view>& GetLines() const noexcept;

    template<typename T>
    requires(std::is_enum_v<T>)
    [[nodiscard]] std::unordered_map<T, Parser::Value, Parser::EnumHash> getTokensAsEnum(const std::unordered_map<T, std::string>& enumMap) const noexcept
    {
        std::unordered_map<T, Parser::Value, Parser::EnumHash> map;
        map.reserve(enumMap.size());

        for (const auto& [key, value] : enumMap)
        {
            const auto iter = tokens.find(value);
            if (iter == tokens.end())
                continue;
            map.emplace(key, iter->second);
        }
        return map;
    }


private:
    std::string buffer;
    std::vector<std::string_view> lines;
    std::unordered_map<std::string_view, Value> tokens;

    std::filesystem::path path;
};