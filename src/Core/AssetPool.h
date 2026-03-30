#pragma once
#include <memory>
#include <string_view>
#include <vector>
#include <ostream>
#include <queue>
#include <unordered_map>

using Index = u_int32_t;

struct TransparentHash {
    using is_transparent = void;

    size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }

    size_t operator()(const std::string& s) const noexcept {
        return std::hash<std::string_view>{}(s);
    }
};

struct TransparentEqual {
    using is_transparent = void;

    bool operator()(std::string_view lhs, std::string_view rhs) const noexcept {
        return lhs == rhs;
    }
};

struct Handle
{
    u_int32_t id;
    u_int32_t gen;

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
 * not guaranteed contiguous memory.
 * @tparam T Type
 */
template<typename T>
class AssetPool
{
public:
    struct Slot
    {
        Slot(Handle handle, bool isFree, std::unique_ptr<T>&& ptr)
        : h(handle), free(isFree), data(std::move(ptr)) {}

        Handle h = {0, 0};
        bool free = false;
        std::unique_ptr<T> data;

        [[nodiscard]] bool operator==(const Slot &other) const { return h.id == other.h.id && h.gen == other.h.gen; }
        [[nodiscard]] bool operator!=(const Slot &other) const { return !(*this == other); }
        [[nodiscard]] bool operator<(const Slot& other) const { return this->h.id < other.h.id || (this->h.id == other.h.id && this->h.gen < other.h.gen); }
        Slot(const Slot&) = delete;
        Slot& operator=(const Slot&) = delete;
        Slot(Slot&&) = default;
        Slot& operator=(Slot&&) = default;
    };

    // [[nodiscard]] T* Get(std::string_view name) const; // I don't think I want this because it is slower than access by handle
    [[nodiscard]] T* Get(Handle h) noexcept;
    /**
    * A Handle with gen = 0 is always invalid!
    */
    [[nodiscard]] Handle GetHandle(std::string_view) const noexcept;
    [[nodiscard]] std::string_view GetName(Handle h) const noexcept;

    /**
     * @return a @link Handle with generation != 0 if successful.
     */
    Handle Register(std::string_view sv, std::unique_ptr<T> asset);

    /**
     * Replaces the specified element. If it doesn't exist, create one.
     * @return
     */
    Handle ReplaceData(std::string_view sv, std::unique_ptr<T> asset);
    /**
     * @return true if successful
     */
    bool Remove(Handle h);

    [[nodiscard]] size_t Size() const;
    [[nodiscard]] size_t Capacity() const;
    void Reserve(size_t count);

    [[nodiscard]] bool Exists(std::string_view name) const;
    [[nodiscard]] bool Exists(Handle h) const;

    [[nodiscard]] T& operator[](Handle h);

    [[nodiscard]] auto begin()  { return pool_.begin(); }
    [[nodiscard]] auto cbegin() const { return pool_.cbegin(); }
    [[nodiscard]] auto end() { return pool_.end(); }
    [[nodiscard]] auto cend() const { return pool_.cend(); }

    friend std::ostream& operator<<(std::ostream &os, const AssetPool& pool)
    {
        for (const auto& [h, name]: pool.map_handle_string_)
        {
            os << "Name: " << name << " | Handle: " << h << "\n";
        }
        return os;
    }


private:
    std::vector<Slot> pool_;
    std::queue<Index> freeSpaces_;
    std::unordered_map<Handle, std::string> map_handle_string_;
    std::unordered_map<std::string, Handle, TransparentHash, TransparentEqual> map_string_handle_;
};


template<typename T>
T * AssetPool<T>::Get(const Handle h) noexcept
{
    if (h.id >= pool_.size() || h.gen == 0)
        return nullptr;
    auto& slot = pool_[h.id];
    if (slot.h == h && !slot.free)
        return slot.data.get();
    return nullptr;
}

template<typename T>
Handle AssetPool<T>::GetHandle(const std::string_view name) const noexcept
{
    const auto iter = map_string_handle_.find(name);
    if (iter == map_string_handle_.end())
        return {0, 0};
    return iter->second;
}

template<typename T>
std::string_view AssetPool<T>::GetName(const Handle h) const noexcept
{
    auto iter = map_handle_string_.find(h);
    if (iter == map_handle_string_.end())
        return "";
    return iter->second;
}

template<typename T>
Handle AssetPool<T>::Register(const std::string_view sv, std::unique_ptr<T> asset)
{
    auto iter = map_string_handle_.find(sv);
    if (iter == map_string_handle_.end())
    {
        Index index = 0;
        if (freeSpaces_.empty())
        {
            index = pool_.size();
            pool_.push_back(Slot({index,1}, false, std::move(asset)));
            /*
            Slot newSlot;
            newSlot.h = {index, 1};
            newSlot.free = false;
            newSlot.data = std::move(asset);
            pool_.push_back(std::move(newSlot));
            */
        } else
        {
            index = freeSpaces_.front();
            Slot& slot = pool_[index];
            slot.free = false;
            slot.data = std::move(asset);
            ++slot.h.gen;
            slot.h.id = index;
            freeSpaces_.pop();
        }

        std::string name = static_cast<std::string>(sv);

        map_string_handle_.insert({std::string(name), pool_[index].h});
        map_handle_string_.insert({pool_[index].h, std::string(name)});
        return pool_[index].h;
    }
    // name already exists do nothing
    Index index = iter->second.id;
    return pool_[index].h;
}

template<typename T>
Handle AssetPool<T>::ReplaceData(const std::string_view sv, std::unique_ptr<T> asset)
{
    auto iter = map_string_handle_.find(sv);
    if (iter == map_string_handle_.end())
    {
        // doesn't exist
        return Register(sv, std::move(asset));
    }
    // already exists
    Index index = iter->second.id;
    Handle oldHandle = pool_[index].h;
    //++pool_[index].h.gen;
    pool_[index].data = std::move(asset);
    map_string_handle_[static_cast<std::string>(sv)] = pool_[index].h;
    auto node = map_handle_string_.extract(map_handle_string_.find(oldHandle));
    node.key() = pool_[index].h;
    map_handle_string_.insert(std::move(node));
    return pool_[index].h;
}

template<typename T>
bool AssetPool<T>::Remove(Handle h)
{
    // doesn't actually remove the element but marks it as free
    auto iter1 = map_handle_string_.find(h);
    if (iter1 == map_handle_string_.end())
        return false;

    Index index = h.id;
    ++pool_[index].h.gen;
    pool_[index].free = true;
    map_string_handle_.erase(iter1->second);
    map_handle_string_.erase(h);

    freeSpaces_.push(index);

    return true;
}

template<typename T>
size_t AssetPool<T>::Size() const
{
    return pool_.size();
}

template<typename T>
size_t AssetPool<T>::Capacity() const
{
    return pool_.capacity();
}

template<typename T>
void AssetPool<T>::Reserve(size_t count)
{
    pool_.reserve(count);
    map_string_handle_.reserve(count);
    map_handle_string_.reserve(count);
}

template<typename T>
bool AssetPool<T>::Exists(const std::string_view name) const
{
    const auto iter = map_string_handle_.find(name);
    if (iter == map_string_handle_.end())
        return false;
    return Exists(iter->second);
}

template<typename T>
bool AssetPool<T>::Exists(const Handle h) const
{
    if (pool_.empty())
        return false;
    if (h.id >= pool_.size() || h.gen == 0)
        return false;
    if (pool_[h.id].free)
        return false;
    return true;
}

template<typename T>
T & AssetPool<T>::operator[](const Handle h)
{
    T* ptr = Get(h);
    if (!ptr) throw std::out_of_range("Invalid handle");
    return *ptr;
}