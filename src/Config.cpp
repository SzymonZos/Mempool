#include "Config.hpp"

#include <numeric>

namespace mp {

MempoolSize GetTotalSize(const MempoolConfig& config) {
    return std::accumulate(config.cbegin(),
                           config.cend(),
                           MempoolSize{},
                           [](MempoolSize sum, const auto& sub_pool) -> MempoolSize {
                               return {.chunks = sum.chunks + sub_pool.chunk_amount,
                                       .bytes = sum.bytes +
                                                (static_cast<std::size_t>(sub_pool.chunk_amount) *
                                                 sub_pool.chunk_size)};
                           });
}

ChunkSizeToOffsetMap GetChunkSizeToOffsetMap(const MempoolConfig& config) {
    ChunkSizeToOffsetMap map{};
    Offset offset{};
    for (const auto& sub_pool : config) {
        map[sub_pool.chunk_size] = offset;
        offset.index += sub_pool.chunk_amount;
        offset.address += (static_cast<std::size_t>(sub_pool.chunk_amount) * sub_pool.chunk_size);
    }
    return map;
}

} // namespace mp
