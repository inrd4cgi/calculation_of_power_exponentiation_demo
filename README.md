# All INRDers Should Become Creators, Who Is Able To Create Values Through Harnessing The Power Of AI 

This repository is intended to demonstrate how to utilize the power of AI copilot to optimize the performance of quick pow algorithm.

the implementation was originally written in C by author, where you could view [here](https://www.bilibili.com/video/BV1Xb4y1R7hH/?share_source=copy_web&vd_source=f65fb0fc3ad364f8044ef383ede4fad2) 

### Brief Descriptions
All the codes should be able to compile and run in CLion on Rocky Linux 9.3

- I've rewritten the original implementation from C to C++20, which you can refer to `completed_original.cpp`, it consumed approximately 434 seconds to calculate $2^{1000000}$ and output results to terminal and a file named `data.txt`.
- then experimented to accelerate the calculation through multi-threading and the calculation consumed around 10 seconds.
- the entire calculation could be done in 3 seconds where OpenCL was introduced for accelerations.

## Known Issues
- the multi-threading mode might not work properly when I tried to integrate it to one program, it takes around 70+ seconds and something might be broken.
- currently it does not work properly on Windows, where OpenCL's compatibility might not as good as Linux do, and MacOS is untested, please feel free to experiment the project on Mac and update the codes accordingly.
- CUDA is not introduced yet.
