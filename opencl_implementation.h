#pragma once

#include "helper_functions.h"
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

void opencl_multiply(std::vector<int>& vec1, std::vector<int>& vec2, std::fstream& file);
