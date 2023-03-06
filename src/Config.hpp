#ifndef MEMPOOL_CONFIG_HPP
#define MEMPOOL_CONFIG_HPP

#include <cstdint>
#include <ostream>
#include <unordered_map>
#include <vector>

namespace mp {

struct SubPoolDescriptor {
    std::int32_t chunk_amount;
    std::size_t chunk_size;

    friend std::ostream& operator<<(std::ostream& ostream, const SubPoolDescriptor& sub_pool) {
        ostream << sub_pool.chunk_amount << "x " << sub_pool.chunk_size;
        return ostream;
    }
};

struct MempoolSize {
    std::int32_t chunks;
    std::size_t bytes;
};
using MempoolConfig = std::vector<SubPoolDescriptor>;
MempoolSize GetTotalSize(const MempoolConfig& config);

struct Offset {
    std::int32_t index;
    std::size_t address;
};
using ChunkSizeToOffsetMap = std::unordered_map<std::size_t, Offset>;
ChunkSizeToOffsetMap GetChunkSizeToOffsetMap(const MempoolConfig& config);

} // namespace mp

#endif // MEMPOOL_CONFIG_HPP
