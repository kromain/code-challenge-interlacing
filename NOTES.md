# Implementation notes

## General infrastructure

I've tried to approach this challenge the same way I approach my regular tasks at work.
My focus wasn't so much on the implementation of the `tv_effect()` method, which is a pretty trivial problem,
but more about how I was going to integrate it in a self-contained program.

My first goal was to define what language and tools I was going to use. Using Qt's image handling classes would have been my go-to solution,
but I wanted the program to be quickly built and testable without having to install external toolkits, so I chose to stick to plain C++17,
and make the program a simple console tool which accepts files as command line arguments, no GUI.
I also wanted it to be as portable as possible, so it can easily be built primarily on Windows, but also Linux and MacOS.
Since CMake is fully supported inside Visual Studio, that was my choice for the project infrastructure and build.

Next, I still had to find a parser for image files, which is the one thing the stdlib doesn't provide.
There's plenty of options like Magick++, CImg, Boost::GIL, but they came with way more than what I needed,
which meant more complex APIs, and several also required additional sub-dependencies.
Since I really didn't need more than load/save support and chose to only consider JPEG images,
I just went for the plain libjpeg and wrote a simple ImageBuffer wrapper around it.

## Pros and cons of using libjpeg

One major downside of using libjpeg directly is that it *only* handles JPEG file formats.
This means I would have to repeat the process with libpng for PNG images, libtiff for TIFF, etc.
However the limited number of image formats and similarities in their APIS makes it a less tedious task than it seems.
But in a real project, it would of course make more sense to invest in a more fully featured library like Magick++ or Qt's QImage.

One advantage of using libjpeg directly is how easy it is manipulate the pixel data line by line, which offers optimization opportunities (see below).

## tv_effect() implementation

The logic behind the method is trivial: replace every second line with a solid black color, ie. set the pixel values to zero,
while the other lines are copied unmodified.

The signature got me confused to a little bit: I am so used to working with RGBA pixel formats that I was suprised the buffers were specified as uint8_t
instead of uint32_t. But after hooking up libjpeg and loading image files I understood why: jpeg images have no alpha layer, so the pixel data is RGB 24bpp only.
Therefore it made sense not to require uint32_t as that would add an unnecessary alpha component per pixel.

That said, it raised a different issue: the method has arguments for the image width and height, but it does not have one for the bytes-per-pixel value.
I could have just hardcoded it to 3 in the implementation, but it felt more appropriate to add it as another parameter to the method, especially as handling
the different values doesn't add complexity to the implementation, it just modifies the line stride.

Calculating the line stride, ie the number of bytes per image line, gives us the information we need to process the pixel data line by line.
The rest of the method is then a simple loop of checking if we're on an odd or even line, applying the transformation for one line stride,
then incrementing the start of the buffer by one line stride, and repeating for as many lines as the image contains.

For the transformation itself, since all the line pixel data is contiguous in memory we can process it in one go:
- for the black lines, we use std::fill() to set all the bytes on the line to zero, since black in RGB color space is (0,0,0).
- for the unmodified lines, we use std::memmove() to copy the bytes from source to destination (see below on why memmove() instead of memcpy()).

### Overlapping input/output pixel buffers

One thing the prototype documentation didn't mention was the behavior in case the input and output buffers overlap, or even if they're the same area in memory.
Being able to transform the image in-place is highly desirable as it removes the need for a second image buffer, so I've made the assumption that `tv_effect()`
needed to support overlapping and equal buffers.
Supporting this slightly complicates the loop, as we need to loop forward (resp. backward) depending on if the output buffer is before (resp. after) the input buffer,
just like std::memmove() does (but we can't just use it since we need to handle the black lines in a similar same way).

Also, we can further optimize the in-place case (ie. when the input and output buffers are the same) by simply skipping the memmove step in that case.

# Ideas for further improving memory/cpu usage

## Using line-by-line processing

As mentioned above, one benefit of using libjpeg or any other API that support a streaming API is the possibility to read-transform-write the image pixels line-by-line
instead of loading all the pixels ahead of time. This greatly reduces the memory pressure and allows handling extremely large images that couldn't be loaded at once otherwise.

## Using parallel line processing

One way to maximize the processing performance on large images is to convert lines in parallel by distributing the image into vertical segments and having each segment
converted on a separate thread based on the number of cores available.

However, I'm unsure that the savings would be substantial in this case, since the fill()/memmove() operations are so simple that the compiler and OS are likely
to optimize them pretty well already, for example using SIMD instructions and caching tricks.
In fact, we're more likely to be hindered by thread switching overhead and memory contention, negating the benefits of parallel looping.

## Using the GPU

Also overkill for this particular transform, but for more advanced effects, using the GPU with a fragment shader or a language like CUDA would further improve performance
while relieving the CPU to perform other tasks.
