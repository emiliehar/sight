# sight::module::filter::mesh

Contains services for mesh filtering. 

## Services

- **SVTKMesher**: generates a mesh from a mask in an image using the VTK library.
- **SMeshCreation**: action that generates a mesh from a mask in an image using the VTK library.
- **SUltrasoundMesh**: generates a mesh used to display an ultrasound image.
- **SNeedle**: generates a cylinder shape of `data::Mesh` type, using VTK.

## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_filter_mesh
)
```
