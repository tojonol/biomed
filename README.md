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
