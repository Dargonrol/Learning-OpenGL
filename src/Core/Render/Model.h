#pragma once
#include <vector>

struct Handle;

class Model
{

private:
    std::vector<Handle> meshes_;
    std::vector<Handle> materials_;

};
