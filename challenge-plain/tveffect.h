#pragma once

#include <cstdint>

namespace hmmhmm {

    /** Creates an TV interlacing effect using the image buffers passed to the method.
     * @param inBuffer - the image to work with as an array of pixels.
     * @param outBuffer - the returned, interlaced image as an array of pixels.
     * @param width - the number of pixels wide of the incoming image.
     * @param height - the number of pixels tall of the incoming image.
     * @param bpp - the number of bytes per pixel of the incoming image.
     *
     * @note the buffers can be overlapping and even equal for in-place pixel transformation.
     */
    void tv_effect(uint8_t const* const inBuffer, const uint32_t width, const uint32_t height, const uint8_t bpp, uint8_t* outBuffer);

} // namespace hmmhmm
