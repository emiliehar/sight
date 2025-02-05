# sight::filter::imageItk

Library containing functions producing an output by 'filtering' an input. This covers registration, 
matrix optimization, image filtering, drawing functions, etc...

## Classes:

- **AutomaticRegistration**
  Performs automatic image registration using ITKv4 registration framework.

- **filters**
  Various image filters (threshold, median, ...).

- **Flipper**
  Flips an image according to a given axis.

- **ItkImageCaster**
  Casts from/to different image type.

- **Labeling**
  Filters image using labels

- **MatrixRegressor**
  Computes an 'average' matrix from a matrix list. Uses PowellOptimizer.

- **MinMaxPropagation**
  Flood fills an image as long as the neighboring voxels are greater than the smallest seed value.

- **MIPMatchingRegistration**
  Fast approximate registration, made of pure translation roughly matching two 3-dimensional images

- **Resampler**
  Transforms and resamples an image.

- **SpheroidExtraction**
  Extracts spheres centers in an image with a given radius and above a given intensity value.


## How to use it

### CMake

```cmake

target_link_libraries( myTarget <PUBLIC|PRIVATE> filter_image)

```

