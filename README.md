# use without IDE
## Step 1 configure project:
```sh
cmake --preset x64
```
or
```sh
cmake --preset x86
```

## Step 2 Build the project
### Debug build
```sh
cmake --build build/x86 --config Debug
```
or
```sh
cmake --build build/x64 --config Debug
```
### Release build
```sh
cmake --build build/x86 --config Release
```
or
```sh
cmake --build build/x64 --config Release
```

## Step 3 Locate the binary and run it.