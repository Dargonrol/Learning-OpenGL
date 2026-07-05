#include "Parser.h"

#include <fstream>
#include <iostream>

inline std::string_view trim(const std::string_view sv) noexcept
{
    size_t start = 0;
    size_t end = sv.size();

    // leading whitespace
    while (start < end && std::isspace(static_cast<unsigned char>(sv[start])))
        ++start;

    // trailing whitespace
    while (end > start && std::isspace(static_cast<unsigned char>(sv[end - 1])))
        --end;

    return sv.substr(start, end - start);
}

Parser::Parser(const std::filesystem::path &path) : path(path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::cerr << "[Parser] Failed to open file: " << std::filesystem::absolute(path) << std::endl;
        return;
    }
    const std::streamsize size = file.tellg();
    file.seekg(0);

    if (size <= 0)
    {
        std::cerr << "[Parser] Invalid file size" << std::endl;
        return;
    }

    buffer.resize(size);
    lines.reserve(size / 32);
    file.read(buffer.data(), size);

    if (!file)
    {
        std::cerr << "[Parser] Failed to read file: " << std::filesystem::absolute(path) << std::endl;
        return;
    }

    if (file.gcount() != size)
    {
        std::cerr << "[Parser] Incomplete read in: " << std::filesystem::absolute(path) << "\n";
        return;
    }

    // line parsing, cleaning
    size_t start = 0;
    const char* data = buffer.data();
    const size_t size_buf = buffer.size();

    for (size_t i = 0; i < size_buf; ++i)
    {
        if (data[i] == '\n')
        {
            size_t line_start = start;
            size_t line_end = i;
            bool is_string = false;

            for (size_t j = line_start; j + 1 < line_end; ++j)
            {
                if (data[j] == '"' && j > line_start && data[j-1] != '\\') // this is lazy and breaks in certain edge cases, replace with FSM
                    is_string = !is_string;
                if (is_string)
                    continue;

                if (data[j] == '/' && data[j + 1] == '/')
                {
                    line_end = j;
                    break;
                }
            }

            if (line_end > line_start && data[line_end - 1] == '\r')
                --line_end;

            auto view = std::string_view(data + line_start, line_end - line_start);
            view = trim(view);

            if (!view.empty())
                lines.emplace_back(view);

            start = i + 1;
        }
    }

    if (start < size_buf)
    {
        size_t line_start = start;
        size_t line_end = size_buf;
        bool is_string = false;

        for (size_t j = line_start; j + 1 < line_end; ++j)
        {
            if (data[j] == '"' && j > line_start && data[j-1] != '\\') // this is lazy and breaks in certain edge cases
                is_string = !is_string;
            if (is_string)
                continue;

            if (data[j] == '/' && data[j + 1] == '/')
            {
                line_end = j;
                break;
            }
        }

        if (line_end > line_start && data[line_end - 1] == '\r')
            --line_end;

        auto view = std::string_view(data + line_start, line_end - line_start);
        view = trim(view);

        if (!view.empty())
            lines.emplace_back(view);
    }

    lines.shrink_to_fit();
}

inline bool isQuoted(const std::string_view& sv)
{
    return sv.size() >= 2 && sv.front() == '"' && sv.back() == '"';
}

inline bool isBraceList(const std::string_view& sv)
{
    return sv.front() == '{' && sv.back() == '}';
}

inline bool getKeyAndValue(const std::string_view& line, std::string_view& key, std::string_view& value)
{
    size_t start = 0;
    size_t end = line.find_first_of('=');
    if (end == std::string_view::npos)
        return false;
    key = trim(line.substr(0, end));
    value = trim(line.substr(end + 1));
    return true;
}

void Parser::Parse()
{
    for (const auto& line: lines)
    {
        std::string_view key, value;
        if (!getKeyAndValue(line, key, value))
            continue;

        if (isQuoted(value)) // must be a string
        {
            parseString(line, key, value);
            continue;
        }

        if (isBraceList(value)) // must (for now) be a vector
        {
            parseVector(line, key, value);
            continue;
        }
        if (true) // must be float or int
        {
            parseScalar(line, key, value);
            continue;
        }
    }
}

void Parser::parseString(const std::string_view& line, const std::string_view& key, const std::string_view& value)
{
    std::string_view inner = value;
    inner.remove_prefix(1);
    inner.remove_suffix(1);
    tokens[std::string(key)] = std::string(inner);
}

void Parser::parseVector(const std::string_view& line, const std::string_view& key, const std::string_view& value)
{
    std::string_view inner = trim(value);
    std::vector<std::string_view> strValues;

    inner.remove_prefix(1);
    inner.remove_suffix(1);

    if (inner.empty())
    {
        std::cerr << "[Parser] Error parsing file: " << path.filename() << "\n" <<
                "in line: " << line << " | Error: Vector cannot be empty" << std::endl;
    }

    size_t pos = 0;
    while (pos < inner.size())
    {
        const size_t comma = inner.find_first_of(',', pos);

        if (comma == std::string_view::npos)
        {
            strValues.emplace_back(trim(inner.substr(pos)));
            break;
        }

        strValues.emplace_back(trim(inner.substr(pos, comma - pos)));
        pos = comma + 1;
    }

    std::vector<float> floats;
    floats.reserve(strValues.size());

    for (auto strVal: strValues)
    {
        if (strVal.empty())
        {
            std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                "in line: " << line << " | Error: values cannot be empty" << std::endl;
            strVal = "0.0";
        }

        if (strVal.back() == 'f')
            strVal.remove_suffix(1);

        float floatVal;
        auto result = std::from_chars(strVal.data(), strVal.data() + strVal.size(), floatVal);
        if (result.ec != std::errc() || result.ptr != strVal.data() + strVal.size())
        {
            std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                "in line: " << line << " | Error: " << make_error_code(result.ec).message() << std::endl;
            continue;
        }
        floats.emplace_back(floatVal);
    }

    if (floats.size() != strValues.size())
    {
        std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                "in line: " << line << " | Error: Error parsing one of the values" << std::endl;
    }

    switch (floats.size())
    {
        case 2: tokens[std::string(key)] = glm::vec2{floats[0], floats[1]}; break;
        case 3: tokens[std::string(key)] = glm::vec3{floats[0], floats[1], floats[2]}; break;

        default:
            std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                "in line: " << line << " | Error: Vector size not supported" << std::endl;
    }
}

void Parser::parseScalar(const std::string_view& line, const std::string_view& key, const std::string_view& value)
{
    std::string_view inner = trim(value);

    if (inner.empty())
    {
        std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                "in line: " << line << " | Error: Value cannot be empty" << std::endl;
        return;
    }

    if (inner.back() == 'f')
        inner.remove_suffix(1);

    std::from_chars_result result{};

    // try float
    {
        float val;
        result = std::from_chars(inner.data(), inner.data() + inner.size(), val);
        if (result.ec == std::errc() && result.ptr == inner.data() + inner.size())
        {
            tokens[std::string(key)] = val;
            return;
        }
    }

    // try int
    {
        int val;
        result = std::from_chars(inner.data(), inner.data() + inner.size(), val);
        if (result.ec == std::errc() && result.ptr == inner.data() + inner.size())
        {
            tokens[std::string(key)] = val;
            return;
        }
    }

    std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                "in line: " << line << " | Error: " << make_error_code(result.ec).message() << std::endl;
}

const std::unordered_map<std::string, Parser::Value> & Parser::GetTokensAndValuesMap() const noexcept
{
    return tokens;
}

const std::string & Parser::GetRaw() const noexcept
{
    return buffer;
}

const std::vector<std::string_view> & Parser::GetLines() const noexcept
{
    return lines;
}

