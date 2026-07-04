# use without IDE
## Step 1 configure project:
### Linux:
``cmake --preset x64``
or
``cmake --preset x86``

### Windows:
``cmake --preset x64-win``
or
``cmake --preset x86-win``

## Step 2 Build the project
``cmake --build build/x86 --config Debug``
or
``cmake --build build/x64 --config Debug``
or
``cmake --build build/x86 --config Release``
or
``cmake --build build/x64 --config Release``

## Step 3 Locate the binary and run it.