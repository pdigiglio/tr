`tr` -- Tuple ranges

#To compile:

```
cmake . -DLIBTR_COMPILE_TESTS=ON -B build -G Ninja
cmake --build build
```


// TODO:
// tr::tuple
// tr::compressed_tuple
// tr::span (with unpack capabilities)
// try and add a tag type to ebo and see if compressing capabilities increase.
// TEST include guards (there surely is an issue with alghoritms not having guards)
// TODO:
//  1. Do I need tup_elem ? Probably not
//  2. Change tr::get to another name to avoid ADL issues with std::get.
//  3. Some kind of cx iota to generate compile time indices.