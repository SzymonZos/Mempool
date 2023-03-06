#ifndef MEMPOOL_MEMPOOL_HPP
#define MEMPOOL_MEMPOOL_HPP

#include "Config.hpp"

#include <memory>
#include <mutex>
#include <stdexcept>

namespace mp {

class MempoolExcetption : std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct Deleter {
    std::align_val_t alignment;

    void operator()(std::byte* ptr) const {
        ::operator delete[](ptr, alignment);
    };
};

class Mempool {
public:
    static constexpr std::size_t total_limit = 2048 * 1024;
    static constexpr auto smallest_alignment = alignof(char);

    Mempool(MempoolConfig subpool, std::size_t alignment);

    Mempool(const Mempool& other) = delete;
    Mempool& operator=(const Mempool& other) = delete;

    Mempool(Mempool&& other) noexcept = delete;
    Mempool& operator=(Mempool&& other) noexcept = delete;

    ~Mempool() = default;

    void* alloc(std::size_t size) noexcept;
    void* free(void* p) noexcept;

private:
    struct Metadata {
        MempoolConfig config;
        ChunkSizeToOffsetMap chunk_size_to_offset_map{};
        std::vector<bool> allocated_chunks{};
    } metadata_{};
    struct Memory {
        std::size_t alignment;
        MempoolSize size{};
        Deleter deleter{std::align_val_t{alignment}};
        std::unique_ptr<std::byte[], Deleter> data{nullptr, deleter};
    } memory_{};
    mutable std::mutex mutex_;
};
} // namespace mp

#endif // MEMPOOL_MEMPOOL_HPP
