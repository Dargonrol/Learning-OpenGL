#pragma once
#include <string_view>
#include <vector>
#include <ostream>
#include <unordered_map>

/**
 * An Handle with gen = 0 is always invalid!
 */
struct Handle
{
    unsigned int id = 0;
    unsigned int gen = 0;

    explicit operator bool() const { return gen != 0; }
    [[nodiscard]] bool operator==(const Handle &other) const { return this->id == other.id && this->gen == other.gen; }
    [[nodiscard]] bool operator!=(const Handle &other) const { return !(*this == other); }
    [[nodiscard]] bool operator<(const Handle& other) const { return this->id < other.id || (this->id == other.id && this->gen < other.gen); }

    friend std::ostream& operator<<(std::ostream &os, const Handle& h)
    {
        os << "Handle{id:" << h.id << ",gen:" << h.gen << "}";
        return os;
    }
};

// Specialize std::hash for Handle
namespace std {
    template<>
    struct hash<Handle>
    {
        std::size_t operator()(const Handle& h) const noexcept
        {
            return std::hash<unsigned int>{}(h.id) ^ (std::hash<unsigned int>{}(h.gen) << 1);
        }
    };
}

/**
 * In-house asset/resource management datastructure.
 * guarantees contiguous memory.
 * @tparam T Type
 */
template<typename T>
class AssetPool
{
public:
    [[nodiscard]] T* Get(std::string_view name) const;
    [[nodiscard]] T* Get(Handle h) const;
    /**
    * An Handle with gen = 0 is always invalid!
    */
    [[nodiscard]] Handle GetHandle(std::string_view) const;
    [[nodiscard]] std::string_view GetName(Handle h) const;

    /**
     * @return true if successful
     */
    bool Register(std::string_view);
    /**
     * @return true if successful
     */
    bool Remove(std::string_view);
    /**
     * @return true if successful
     */
    bool Remove(Handle h);

    [[nodiscard]] size_t Size() const;
    [[nodiscard]] size_t Capacity() const;
    void Reserve(size_t count);

    [[nodiscard]] bool Exists(std::string_view) const;
    [[nodiscard]] bool Exists(Handle h) const;

    [[nodiscard]] T& operator[](Handle h);
    [[nodiscard]] T& operator[](std::string_view name);

    [[nodiscard]] auto begin()  { return pool_.begin(); }
    [[nodiscard]] auto cbegin() const { return pool_.cbegin(); }
    [[nodiscard]] auto end() { return pool_.end(); }
    [[nodiscard]] auto cend() const { return pool_.cend(); }

    friend std::ostream& operator<<(std::ostream &os, const AssetPool& pool)
    {
        for (const auto& [h, i]: pool.map_handle_index_)
        {
            os << "Name: " << pool.GetName(h) << " | Handle: " << h << "\n";
        }
        return os;
    }

private:
    std::vector<T> pool_;
    std::unordered_map<Handle, std::string> map_handle_string_;
    std::unordered_map<std::string, Handle> map_string_handle_;
    std::unordered_map<Handle, size_t> map_handle_index_;
};
