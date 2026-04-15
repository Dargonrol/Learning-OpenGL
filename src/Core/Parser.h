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

    explicit Parser(const std::filesystem::path& path);

    void Parse();

    [[nodiscard]] const std::string& GetRaw() const noexcept;
    [[nodiscard]] const std::vector<std::string_view>& GetLines() const noexcept;


private:
    std::string buffer;
    std::vector<std::string_view> lines;
    std::unordered_map<std::string_view, Value> tokens;
};