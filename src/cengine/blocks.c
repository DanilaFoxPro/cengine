#include <cengine\blocks.h>

#include <cengine/debug/recovery.h>

struct BlockInfo blocks[256];

uint8_t block_is_transparent( const BlockIdentifier block_identifier )
{
        return blockflag_get_value( blocks[block_identifier].flags, blockflag_opaque );
}


const struct BlockSides blocksides_filled ( BlockSide fill )
{
        struct BlockSides output;
        
        output.left     = fill;
        output.right    = fill;
        output.top      = fill;
        output.bottom   = fill;
        output.front    = fill;
        output.back     = fill;
        
        return output;
        
}

const struct BlockSides blocksides_common_fill ( BlockSide sides, BlockSide top, BlockSide bottom )
{
        struct BlockSides output;
        
        output.left     = sides;
        output.right    = sides;
        output.front    = sides;
        output.back     = sides;
        
        output.top      = top;
        output.bottom   = bottom;
        
        return output;
        
}

void blockflag_enable(BlockInfoFlags* flags, const BlockInfoFlags flag)
{
        *flags |= flag;
}

void blockflag_disable(BlockInfoFlags* flags, const BlockInfoFlags flag)
{
        *flags &= ~flag;
}

void blockflag_toggle(BlockInfoFlags* flags, const BlockInfoFlags flag)
{
        *flags ^= flag;
}

uint8_t blockflag_get_value(const BlockInfoFlags flags, const BlockInfoFlags flag)
{
        return flags & flag;
}


/** Populate 'blocks' array with valid block sides. */
void blocks_fill()
{
        
        // Set default flags for all used block IDs.
        
        BlockInfoFlags default_flags = 0;
        default_flags ^= blockflag_opaque;
        
        for( int i = 0; i < blockid_LIMIT; i++ ) {
                blocks[i].flags = default_flags;
        }
        
        // Set blocksides.
        
        blocks[blockid_air].sides     = blocksides_filled( tile_dirt );
        blocks[blockid_grass].sides   = blocksides_common_fill( tile_grass_side, tile_grass_top, tile_dirt );
        blocks[blockid_stone].sides   = blocksides_filled( tile_stone );
        blocks[blockid_dirt].sides    = blocksides_filled( tile_dirt );
        blocks[blockid_bedrock].sides = blocksides_filled( tile_bedrock );
        blocks[blockid_sand].sides    = blocksides_filled( tile_sand );
        blocks[blockid_glass].sides   = blocksides_filled( tile_glass );
        
        // Special block flags.
        
        blockflag_enable( &blocks[blockid_air].flags,   blockflag_opaque );
        blockflag_enable( &blocks[blockid_glass].flags, blockflag_opaque );
        
}
