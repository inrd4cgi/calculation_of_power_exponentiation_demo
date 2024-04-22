#include "opencl_implementation.h"

#include "global_variables.h"

void opencl_multiply(std::vector<int>& vec1, std::vector<int>& vec2, std::fstream& file)
{
    uint base_length = count_vector_non_zero_elements(vec1);
    uint temp_length = count_vector_non_zero_elements(vec2);

    intermediate.resize(base_length + temp_length + 1); // Resize intermediate to be large enough to hold the result

    try {
        // Initialize OpenCL environment
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        auto platform = platforms.front();
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        auto device = devices.front();
        auto context = cl::Context(device);
        auto queue = cl::CommandQueue(context, device);

        // Create memory buffers
        auto buffer_vec1 = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * vec1.size(), vec1.data());
        auto buffer_vec2 = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * vec2.size(), vec2.data());
        auto buffer_intermediate = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY, sizeof(int) * intermediate.size());

        // Write the kernel code
        std::string kernel_code =
            "__kernel void multi(__global const int* vec1, __global const int* vec2, __global int* intermediate, uint base_length, uint temp_length) {"
            "   int i = get_global_id(0);"
            "   if (i < base_length) {"
            "       for (int j = 0; j < temp_length; ++j) {"
            "           atomic_add(&intermediate[i + j], vec1[i] * vec2[j]);"
            "       }"
            "   }"
            "}";

        // Compile the kernel code
        auto program = cl::Program(context, kernel_code);
        try {
            program.build("-cl-std=CL3.0");
        }
        catch (const cl::Error&) {
            for (const auto& device : context.getInfo<CL_CONTEXT_DEVICES>()) {
                std::cerr << "Build log for device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl
                          << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
            }
            throw;
        }

        // Set the kernel arguments
        auto kernel = cl::Kernel(program, "multi");
        kernel.setArg(0, buffer_vec1);
        kernel.setArg(1, buffer_vec2);
        kernel.setArg(2, buffer_intermediate);
        kernel.setArg(3, static_cast<cl_uint>(base_length));
        kernel.setArg(4, static_cast<cl_uint>(temp_length));

        // Enqueue the kernel for execution
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(base_length), cl::NullRange);

        // Read the result back from the device to the host
        queue.enqueueReadBuffer(buffer_intermediate, CL_TRUE, 0, sizeof(int) * intermediate.size(), intermediate.data());

        // Perform the carry operation
        for (size_t i = 0; i < intermediate.size() - 1; ++i) {
            intermediate[i + 1] += intermediate[i] / 10;
            intermediate[i] %= 10;
        }
    }
    catch (const cl::Error& e) {
        std::cerr << "OpenCL error: " << e.what() << "(" << e.err() << ")\n";
    }
}