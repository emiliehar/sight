# sight::module::filter::image

Contains services for image processing. 
In other words it contains services to operate on a `sight::data::Image`.

## Services

- **SImageExtruder**
  Sets all voxels of an image that are inside meshes to the lowest value of the image type.
- **SThreshold**
  Applies a threshold on an image.

## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_filter_image
)

```
