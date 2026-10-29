#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H
#include <vector>
struct FilterDescriptor
{
    const char* filterName;
    std::vector<const char*> params;
    // clang-format off
    FilterDescriptor(): filterName(nullptr) {};
    // clang-format on
};
#endif  // #ifndef DESCRIPTOR_H
