#include "imagebuffer.h"

#include <jpeglib.h>

#include <functional>


namespace {

/** Simple lambda-based RAII resource deleter */
struct ScopeGuard {
    ScopeGuard(std::function<void()>&& d) : deleter(std::move(d)) {}
    ~ScopeGuard() { deleter(); }

    std::function<void()> deleter;
};

} // anonymous namespace

namespace hmmhmm {

ImageBuffer::ImageBuffer()
    : m_imageWidth(0)
    , m_imageHeight(0)
    , m_bytesPerPixel(0)
    , m_pixels(nullptr)
{
}

bool ImageBuffer::load(const std::filesystem::path& imageFile)
{
    // TODO file format detection would be added here if multiple formats were supported
    return load_libjpeg(imageFile);
}

bool ImageBuffer::save(const std::filesystem::path& imageFile)
{
    if (!m_pixels || !m_imageWidth || !m_imageHeight || m_bytesPerPixel != 3)
        return false;

    // TODO file extension parsing would be added here if multiple formats were supported
    return save_libjpeg(imageFile);
}

unsigned int ImageBuffer::imageWidth() const
{
        return m_imageWidth;
}

unsigned int ImageBuffer::imageHeight() const
{
        return m_imageHeight;
}

unsigned int ImageBuffer::bytesPerPixel() const
{
    return m_bytesPerPixel;
}

uint8_t* ImageBuffer::pixels()
{
        return m_pixels.get();
}

const uint8_t* ImageBuffer::pixels() const
{
    return m_pixels.get();
}

bool ImageBuffer::load_libjpeg(const std::filesystem::path& imageFile)
{
    FILE* fileHandle = fopen(imageFile.u8string().c_str(), "rb");
    if (!fileHandle)
        return false;

    jpeg_error_mgr errorMgr;
    jpeg_decompress_struct libjpegHandle;
    // no fancy error handling here, we'll just use libjpeg's default behavior
    libjpegHandle.err = jpeg_std_error(&errorMgr);
    jpeg_create_decompress(&libjpegHandle);

    // automatically release the resources when exiting the method
    ScopeGuard sg([&fileHandle, &libjpegHandle] {
        jpeg_destroy_decompress(&libjpegHandle);
        fclose(fileHandle);
        });

    jpeg_stdio_src(&libjpegHandle, fileHandle);

    if (jpeg_read_header(&libjpegHandle, true /*require_image*/) != JPEG_HEADER_OK)
        return false;

    // we want a 24bpp RGB pixel format
    libjpegHandle.out_color_space = JCS_RGB;

    jpeg_start_decompress(&libjpegHandle);

    m_imageWidth = libjpegHandle.output_width;
    m_imageHeight = libjpegHandle.output_height;
    m_bytesPerPixel = libjpegHandle.out_color_components;

    if (!m_imageWidth || !m_imageHeight || m_bytesPerPixel != 3) {
        jpeg_abort_decompress(&libjpegHandle);
        return false;
    }

    const size_t lineStride = m_imageWidth * m_bytesPerPixel;

    // automatically releases the previous array if already allocated
    m_pixels = std::make_unique<uint8_t[]>(lineStride * m_imageHeight);

    for (uint8_t* pixelBuffer = m_pixels.get(); libjpegHandle.output_scanline < m_imageHeight;) {
        const auto linesRead = jpeg_read_scanlines(&libjpegHandle, &pixelBuffer, libjpegHandle.rec_outbuf_height);
        if (!linesRead)
            break;

        pixelBuffer += linesRead * lineStride;
    }

    jpeg_finish_decompress(&libjpegHandle);

    return true;
}

bool ImageBuffer::save_libjpeg(const std::filesystem::path& imageFile)
{
    FILE* fileHandle = fopen(imageFile.u8string().c_str(), "wb");
    if (!fileHandle)
        return false;

    jpeg_error_mgr errorMgr;
    jpeg_compress_struct libjpegHandle;
    // no fancy error handling here, we'll just use libjpeg's default behavior
    libjpegHandle.err = jpeg_std_error(&errorMgr);
    jpeg_create_compress(&libjpegHandle);

    // automatically release the resources when exiting the method
    ScopeGuard sg([&fileHandle, &libjpegHandle] {
        jpeg_destroy_compress(&libjpegHandle);
        fclose(fileHandle);
        });

    jpeg_stdio_dest(&libjpegHandle, fileHandle);

    libjpegHandle.image_width = m_imageWidth;
    libjpegHandle.image_height = m_imageHeight;
    libjpegHandle.input_components = m_bytesPerPixel;
    libjpegHandle.in_color_space = JCS_RGB;
    jpeg_set_defaults(&libjpegHandle);
    jpeg_set_quality(&libjpegHandle, 100, true /*force_baseline*/);
    jpeg_start_compress(&libjpegHandle, true /*write_all_tables*/);

    const size_t lineStride = m_imageWidth * m_bytesPerPixel;

    for (uint8_t* pixelBuffer = m_pixels.get(); libjpegHandle.next_scanline < m_imageHeight;) {
        const auto linesWritten = jpeg_write_scanlines(&libjpegHandle, &pixelBuffer, 1);
        if (!linesWritten)
            break;

        pixelBuffer += linesWritten * lineStride;
    }
    jpeg_finish_compress(&libjpegHandle);

    return true;
}

} // namespace hmmhmm
