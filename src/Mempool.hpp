#ifndef MEMPOOL_MEMPOOL_HPP
#define MEMPOOL_MEMPOOL_HPP

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace mp {

struct SubPoolDescriptor {
    int32_t chunk_amount;
    std::size_t chunk_size;

    friend std::ostream& operator<<(std::ostream& ostream, const SubPoolDescriptor& sub_pool) {
        ostream << sub_pool.chunk_amount << "x " << sub_pool.chunk_size;
        return ostream;
    }
};

using MempoolConfig = std::vector<SubPoolDescriptor>;

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
    MempoolConfig config_;
    struct Memory {
        std::size_t alignment;
        std::size_t size;
        Deleter deleter{std::align_val_t{alignment}};
        std::unique_ptr<std::byte[], Deleter> data{nullptr, deleter};
    } memory_{};
    mutable std::mutex mutex_;
};
} // namespace mp

#endif // MEMPOOL_MEMPOOL_HPP
