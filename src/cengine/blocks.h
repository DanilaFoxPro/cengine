#ifndef BLOCKS_H
#define BLOCKS_H

#include <stdint.h> // uint8_t

typedef uint8_t BlockIdentifier;
typedef int BlockSide;

struct BlockSides {
        BlockSide left;
        BlockSide right;
        BlockSide top;
        BlockSide bottom;
        BlockSide front;
        BlockSide back;
};

typedef uint8_t BlockInfoFlags;

enum {
        blockflag_opaque = 1,
};

struct BlockInfo {
        struct BlockSides sides;
        BlockInfoFlags flags;
};

enum block_tile {
        tile_dirt,
        tile_stone,
        tile_bedrock,
        tile_sand,
        tile_grass_side,
        tile_glass,
        tile_grass_top = 8,
};

enum block_index {
        blockid_air,
        blockid_grass,
        blockid_stone,
        blockid_dirt,
        blockid_bedrock,
        blockid_sand,
        blockid_glass,
        blockid_LIMIT
};

uint8_t block_is_transparent( const BlockIdentifier block_identifier );

const struct BlockSides blocksides_filled( BlockSide fill );
const struct BlockSides blocksides_common_fill( BlockSide sides, BlockSide top, BlockSide bottom );

void    blockflag_enable        ( BlockInfoFlags* flags, const BlockInfoFlags flag );
void    blockflag_disable       ( BlockInfoFlags* flags, const BlockInfoFlags flag );
void    blockflag_toggle        ( BlockInfoFlags* flags, const BlockInfoFlags flag );
uint8_t blockflag_get_value( const BlockInfoFlags flags, const BlockInfoFlags flag );

void blocks_fill();

extern struct BlockInfo blocks[256];

#endif
