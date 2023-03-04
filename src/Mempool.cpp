#include "Mempool.hpp"

#include "log.hpp"

#include <algorithm>
#include <iostream>
#include <new>
#include <numeric>

#include <fmt/ostream.h>
#include <fmt/ranges.h>

namespace mp {
Mempool::Mempool(mp::MempoolConfig config, std::size_t alignment) :
    config_{std::move(config)},
    memory_{alignment, 0} {
    if (alignment < smallest_alignment) {
        auto msg = fmt::format(
            "Provided alignment '{}' is smaller than the smallest possible one '{}'",
            alignment,
            smallest_alignment);
        LOG_AND_THROW(MempoolExcetption, msg);
    }
    auto size = std::accumulate(config_.cbegin(),
                                config_.cend(),
                                0U,
                                [](std::size_t sum, const auto& sub_pool) {
                                    return sum + (static_cast<std::size_t>(sub_pool.chunk_amount) *
                                                  sub_pool.chunk_size);
                                });
    if (size > total_limit) {
        auto msg = fmt::format(
            "Sum of all chunks '{}' is bigger than total limit '{}'. Config: '{}'",
            size,
            total_limit,
            config_);
        LOG_AND_THROW(MempoolExcetption, msg);
    }
    memory_.data.reset(new (std::align_val_t{alignment}) std::byte[size]);
    memory_.size = size;
}

void* Mempool::alloc(std::size_t size) noexcept {
    auto max_size = std::max_element(
        config_.cbegin(),
        config_.cend(),
        [](const auto& lhs, const auto& rhs) { return lhs.chunk_size < rhs.chunk_size; });
    if (size > max_size->chunk_size) {
        return nullptr;
    }
    std::lock_guard lock(mutex_);
    return memory_.data.get();
}

void* Mempool::free(void* p) noexcept {
    if (p < memory_.data.get() or p > memory_.data.get() + memory_.size) {
        return nullptr;
    }
    std::lock_guard lock(mutex_);
    return memory_.data.get();
}

} // namespace mp
