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
        std::cerr << "Failed to open file: " << std::filesystem::absolute(path) << std::endl;
        return;
    }
    const std::streamsize size = file.tellg();
    file.seekg(0);

    if (size <= 0)
    {
        std::cerr << "Invalid file size" << std::endl;
        return;
    }

    buffer.resize(size);
    lines.reserve(size / 32);
    file.read(buffer.data(), size);

    if (!file)
    {
        std::cerr << "Failed to read file: " << std::filesystem::absolute(path) << std::endl;
        return;
    }

    if (file.gcount() != size)
    {
        std::cerr << "Incomplete read in: " << std::filesystem::absolute(path) << "\n";
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

void Parser::Parse()
{
    for (const auto& line: lines)
    {
        size_t start = 0;
        size_t end = line.size();
        end = line.find_first_of('=');
        if (end == std::string_view::npos)
            continue;
        std::string_view key = trim(line.substr(0, end));
        std::string_view value = trim(line.substr(end + 1));

        // do the parsing
        // value is a string
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
        {
            value.remove_prefix(1);
            value.remove_suffix(1);
            tokens[key] = value;
            continue;
        }

        // value is a vec-n
        if (value.front() == '{' && value.back() == '}')
        {
            std::vector<std::string_view> values;

            value.remove_prefix(1);
            value.remove_suffix(1);

            size_t pos = 0;
            while (pos < value.size())
            {
                const size_t comma = value.find_first_of(',', pos);

                if (comma == std::string_view::npos)
                {
                    values.emplace_back(trim(value.substr(pos)));
                    break;
                }

                values.emplace_back(trim(value.substr(pos, comma - pos)));
                pos = comma + 1;
            }
            std::vector<float> floats;
            floats.reserve(values.size());
            for (auto str_val: values)
            {
                if (str_val.empty())
                {
                    std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                        "in line: " << line << " | Error: values cannot be empty" << std::endl;
                    continue;
                }
                float float_val;
                auto result = std::from_chars(str_val.data(), str_val.data() + str_val.size(), float_val);
                if (result.ec != std::errc() || result.ptr != str_val.data() + str_val.size())
                {
                    std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                        "in line: " << line << " | Error: " << make_error_code(result.ec).message() << std::endl;
                    continue;
                }
                floats.emplace_back(float_val);
            }

            if (floats.size() != values.size())
            {
                std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                        "in line: " << line << " | Error: Error parsing one of the values" << std::endl;
                continue;
            }

            switch (floats.size())
            {
                case 2: tokens[key] = glm::vec2{floats[0], floats[1]}; break;
                case 3: tokens[key] = glm::vec3{floats[0], floats[1], floats[2]}; break;

                default:
                    std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                        "in line: " << line << " | Error: Vector size not supported" << std::endl;
                    continue;

            }

        } else
        {
            if (value.front() == '{' && value.back() != '}')
            {
                std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                    "in line: " << line << " | Error: \"" << value.back() << "\" is not a valid ending" << std::endl;
                continue;
            }

            if (value.back() == '}' && value.front() != '{')
            {
                std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                    "in line: " << line << " | Error: \"" << value.front() << "\" is not a valid beginning" << std::endl;
                continue;
            }
        }


        // value is a float
        if (value.back() == 'f' || value.find('.') != std::string_view::npos)
        {
            float val;
            auto result = std::from_chars(value.data(), value.data() + value.size(), val);
            if (result.ec != std::errc() || result.ptr != value.data() + value.size())
            {
                std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                    "in line: " << line << " | Error: " << make_error_code(result.ec).message() << std::endl;
                continue;
            }
            tokens[key] = val;
            continue;
        }

        // value is an int
        {
            int val;
            auto result = std::from_chars(value.data(), value.data() + value.size(), val);
            if (result.ec != std::errc() || result.ptr != value.data() + value.size())
            {
                std::cerr << "Error parsing file: " << std::filesystem::absolute(path) << "\n" <<
                    "in line: " << line << " | Error: " << make_error_code(result.ec).message() << std::endl;
                continue;
            }
            tokens[key] = val;
            continue;
        }
    }
}

const std::unordered_map<std::string_view, Parser::Value> & Parser::GetTokensAndValuesMap() const noexcept
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
