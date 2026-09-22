# tired (tiny-render scene) editor

A small scene editor with Qt-based UI and vulkan sceneg graph.  

## Dependencies  
  
### Vulkan SDK and glslang tools  
```bash
apt install glslang-dev glslang-tools vulkan-sdk
```  
  
Or you can download and install Vulkan SDK from https://vulkan.lunarg.com/sdk/home.  
Vulkan SDK minimal version is 1.1  

### Qt6  
Build yourself or use binaries provided by your system:  
```bash
sudo apt install qt6-base-dev qt6-declarative-dev
```

### Another libs  
Vulkan scene graph sources is embed into this project and itself depends on some libraries: 
```bash
sudo apt install libxcb-dev libthreads-dev
```

## Build  
Build checked only on Linux with X11 window server.  
This project uses -std=c++26 build flag and it implies demand of newer c++ compiler (gcc 16.1 works nice).  
Create build directory, configure and build  
```bash

mkdir build
cd build

cmake ..
cmake --build .
```

## Libraries used  
This project embed couple of very nice, low dependency, very useful libraries:  
- "Vulkan Scene Graph" - https://github.com/vsg-dev/VulkanSceneGraph
- "generator" - https://github.com/ilmola/generator
- "OpenMesh" - https://www.graphics.rwth-aachen.de/software/openmesh/

