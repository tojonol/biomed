# Build Procedure
First make sue that your `ITK_DIR` environment variable points to the directory
where you built ITK. Then run these commands:

```
mkdir build
cd build
cmake ..
make
```

After the first time you run cmake you can just `make` and run within the build
directory without re-doing the cmake step as long as the CMakeLists.txt file
doesn't change.

# Usage

The project produces several executables which generally take a DICOM image
stack and output either another stack or text to stdout. Functions and usage
for each is recorded below:

## Segmentation
### Seed Picker
Starts a GUI to select a set of seed points for segmentation.

```
./SeedPicker image_stack
```

### SRGMain
Performs seeded region growing segmentation.

```
./SRGMain seed_file stack_dir output_dir
```

output_dir should be structured like so:

```
+- output_dir
   +- regoin_1
   |  +- mask
   |  +- segment
   |  \- pngs
   |
   +-- region_2
   |  +- mask
   |  +- segment
   |  \- pngs
   |
   +-- region_3
    ...
```

with a directory for each region in the input seed file.

### Registration
Registers two images and output a transformed moving image to output_dir

```
./Registration fixed_image_stack moving_image_stack output_dir sample_points
``` 

### Quantify
Does volume calculationon segmented images.

No usage instructions because everything is hardcoded.
