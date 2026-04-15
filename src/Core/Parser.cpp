#include "Parser.h"

#include <fstream>
#include <iostream>

Parser::Parser(const std::filesystem::path &path)
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
        std::cerr << "Incomplete read\n";
        return;
    }

    // line parsing
    size_t start = 0;
    const char* data = buffer.data();
    const size_t size_buf = buffer.size();

    for (size_t i = 0; i < size_buf; ++i)
    {
        if (data[i] == '\n')
        {
            size_t line_start = start;
            size_t line_end = i;

            for (size_t j = line_start; j + 1 < line_end; ++j)
            {
                if (data[j] == '/' && data[j + 1] == '/')
                {
                    line_end = j;
                    break;
                }
            }

            if (line_end > line_start && data[line_end - 1] == '\r')
                --line_end;

            if (line_end > line_start)
                lines.emplace_back(data + line_start, line_end - line_start);

            start = i + 1;
        }
    }

    if (start < size_buf)
    {
        size_t line_start = start;
        size_t line_end = size_buf;

        for (size_t j = line_start; j + 1 < line_end; ++j)
        {
            if (data[j] == '/' && data[j + 1] == '/')
            {
                line_end = j;
                break;
            }
        }

        if (line_end > line_start && data[line_end - 1] == '\r')
            --line_end;

        if (line_end > line_start)
            lines.emplace_back(data + line_start, line_end - line_start);
    }

    lines.shrink_to_fit();
}

void Parser::Parse()
{

}

const std::string & Parser::GetRaw() const noexcept
{
    return buffer;
}

const std::vector<std::string_view> & Parser::GetLines() const noexcept
{
    return lines;
}
