# sight::filter::image

Library containing functions producing an output by 'filtering' an input. This covers registration, 
matrix optimization, image filtering, drawing functions, etc...

## Classes:

- **BresenhamLine**
  Draws a Bresenham line.

- **Image**
  Applies a mask to an image, zeroing data outside the mask.

- **ImageDiff**
  Computes difference between two images.

- **ImageExtruder**
  Extrudes voxels from an image that are inside a given mesh.

- **LineDrawer**
  Draws line.

## How to use it

### CMake

```cmake

target_link_libraries( myTarget <PUBLIC|PRIVATE> filter_image)

```

