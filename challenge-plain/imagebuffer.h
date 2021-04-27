#pragma once

#include <cstdio>
#include <filesystem>
#include <memory>

namespace hmmhmm {

class ImageBuffer
{
public:
    /** Constructs an empty image buffer instance. */
    ImageBuffer();
    /** Default destructor. */
    ~ImageBuffer() = default;

    /** Loads the pixels from an image file.
     * Currently supported formats: JPEG
     * @param imageFile - the path to the image file on disk
     * @return true if the image can be read and is a supported format, false otherwise
     */
    bool load(const std::filesystem::path& imageFile);
    /** Saves the current pixels to a image file.
     * Currently supported formats: JPEG
     * @param imageFile - the path to the image file on disk
     * @return true if there are valid pixels and the image can be written to, false otherwise
     */
    bool save(const std::filesystem::path& imageFile);

    /** Returns the number of pixel columns in the image (0 if empty). */
    unsigned int imageWidth() const;
    /** Returns the number of pixel rows in the image (0 if empty). */
    unsigned int imageHeight() const;
    /** Returns the number of bytes per pixel in the image, e.g. 3 for an RGB image or 0 if empty. */
    unsigned int bytesPerPixel() const;

    /** Returns the modifiable pixel data for the image (nullptr if empty).
     * The data layout depends on imageWidth(), imageHeight() and bytesPerPixel().
     */
    uint8_t* pixels();
    /** Returns the read-only pixel data for the image (nullptr if empty).
     * The data layout depends on imageWidth(), imageHeight() and bytesPerPixel().
     */
    const uint8_t* pixels() const;

private:
    // per-format routines, more could be added for PNG, TIFF, etc.
    bool load_libjpeg(const std::filesystem::path& imageFile);
    bool save_libjpeg(const std::filesystem::path& imageFile);

    unsigned int m_imageWidth;
    unsigned int m_imageHeight;
    unsigned int m_bytesPerPixel;

    std::unique_ptr<uint8_t[]> m_pixels;
};

} // namespace hmmhmm
