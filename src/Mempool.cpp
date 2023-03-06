#include "Mempool.hpp"

#include "Config.hpp"
#include "log.hpp"

#include <algorithm>
#include <iostream>
#include <new>
#include <numeric>

#include <fmt/ostream.h>
#include <fmt/ranges.h>

namespace mp {
Mempool::Mempool(MempoolConfig config, std::size_t alignment) :
    metadata_{std::move(config)},
    memory_{alignment} {
    if (alignment < smallest_alignment) {
        auto msg = fmt::format(
            "Provided alignment '{}' is smaller than the smallest possible one '{}'",
            alignment,
            smallest_alignment);
        LOG_AND_THROW(MempoolExcetption, msg);
    }
    std::sort(metadata_.config.begin(),
              metadata_.config.end(),
              [](const auto& lhs, const auto& rhs) { return lhs.chunk_size < rhs.chunk_size; });
    auto size = GetTotalSize(metadata_.config);

    metadata_.chunk_size_to_offset_map = GetChunkSizeToOffsetMap(metadata_.config);
    LOG(size.chunks);
    metadata_.allocated_chunks.resize(size.chunks);

    memory_.size = size;
    if (memory_.size.bytes > total_limit) {
        auto msg = fmt::format(
            "Sum of all chunks '{}' is bigger than total limit '{}'. Config: '{}'",
            memory_.size.bytes,
            total_limit,
            metadata_.config);
        LOG_AND_THROW(MempoolExcetption, msg);
    }
    memory_.data.reset(new (std::align_val_t{alignment}) std::byte[memory_.size.bytes]);
}

void* Mempool::alloc(std::size_t size) noexcept {
    auto max_size = metadata_.config.back().chunk_size;
    if (size > max_size) {
        return nullptr;
    }
    auto sub_pool = std::find_if(
        metadata_.config.cbegin(),
        metadata_.config.cend(),
        [size](const auto& sub_pool) { return size <= sub_pool.chunk_size; });
    auto offset = metadata_.chunk_size_to_offset_map[sub_pool->chunk_size];
    auto sub_pool_begin = metadata_.allocated_chunks.begin() + offset.index;
    auto sub_pool_end = sub_pool_begin + sub_pool->chunk_amount;

    std::lock_guard lock(mutex_);
    auto chunk = std::find(sub_pool_begin, sub_pool_end, false);
    if (chunk == metadata_.allocated_chunks.end()) {
        return nullptr;
    }
    *chunk = true;
    auto chunk_idx = static_cast<std::size_t>(std::distance(sub_pool_begin, chunk));
    LOG("chunk_idx: " + std::to_string(chunk_idx));
    auto ret = offset.address + (chunk_idx * sub_pool->chunk_size);
    LOG("ret: " + std::to_string(ret));
    return memory_.data.get() + ret;
}

void* Mempool::free(void* p) noexcept {
    if (p < memory_.data.get() or p > memory_.data.get() + memory_.size.bytes) {
        return nullptr;
    }
    auto chunk = static_cast<std::size_t>(static_cast<std::byte*>(p) - memory_.data.get());
    if (static_cast<std::size_t>(chunk) % memory_.alignment) {
        return nullptr;
    }
    std::lock_guard lock(mutex_);
    metadata_.allocated_chunks[chunk] = false;
    return memory_.data.get() + chunk;
}

} // namespace mp
