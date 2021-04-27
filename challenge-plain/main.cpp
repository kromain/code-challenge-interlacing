#include "imagebuffer.h"
#include "tveffect.h"

#include <iostream>
#include <filesystem>

using namespace hmmhmm;
namespace fs = std::filesystem;

int wmain(int argc, wchar_t* argv[])
{
    // Simplest arg handling, more elaborate parsing would use boost::program_options, QCommandLineParser, etc.
    if (argc < 2) {
        std::wcerr << "Usage: " << fs::path(argv[0]).filename() << " file_to_process.jpeg [other_file_to_process.jpg...]" << std::endl;
        return 1;
    }

    for (int argIndex = 1; argIndex < argc; ++argIndex) {
        const fs::path imagePath = argv[argIndex];
        if (!fs::is_regular_file(imagePath)) {
            std::wcerr << imagePath << " is not a valid file! Skipping." << std::endl;
            continue;
        }

        std::wcout << "Processing " << imagePath.filename() << "..." << std::endl;

        ImageBuffer imageBuffer;
        if (!imageBuffer.load(imagePath)) {
            std::wcerr << imagePath << " is not a valid image file! Skipping." << std::endl;
            continue;
        }

        // it's a trivial forward-only transformation, so we can skip the separate output buffer and modify the pixels in-place
        tv_effect(imageBuffer.pixels(), imageBuffer.imageWidth(), imageBuffer.imageHeight(), imageBuffer.bytesPerPixel(), imageBuffer.pixels());

        fs::path outputPath = imagePath;
        outputPath.replace_extension("");
        outputPath += "-interlaced";
        outputPath.replace_extension(imagePath.extension());

        if (imageBuffer.save(outputPath))
            std::wcout << "Successfully saved " << outputPath << std::endl;
        else
            std::wcerr << "Failed to save " << outputPath << "!" << std::endl;
    }

    return 0;
}
