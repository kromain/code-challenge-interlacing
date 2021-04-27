#include "tveffect.h"

#include <cstring>
#include <iterator>

namespace hmmhmm {

void tv_effect(uint8_t const* const inBuffer, const uint32_t width, const uint32_t height, const uint8_t bpp, uint8_t* outBuffer)
{
    // sanity checks
    if (!inBuffer || !outBuffer || !width || !height || !bpp)
        return;

    const size_t lineStride = width * bpp;

    // could be further restricted to the buffers actually overlapping, but there's no huge penalty from iterating backwards anyway
    const bool reverseCopy = outBuffer > inBuffer;

    for (uint32_t i = 0; i < height; ++i) {
        const uint32_t line = reverseCopy ? height - 1 - i : i;
        const size_t bufferOffset = line * lineStride;

        if (line % 2) { // odd line
            if (reverseCopy)
                std::fill(std::make_reverse_iterator(outBuffer + bufferOffset + lineStride), std::make_reverse_iterator(outBuffer + bufferOffset), 0);
            else
                std::fill(outBuffer + bufferOffset, outBuffer + bufferOffset + lineStride, 0);
        }
        else if (outBuffer != inBuffer) { // optimize for in-place modification: nothing to move in that case
            // memmove already handles the reverse copy in case outBuffer is partially overlapping inBuffer
            std::memmove(outBuffer + bufferOffset, inBuffer + bufferOffset, lineStride);
        }
    }
}

} // namespace hmmhmm
