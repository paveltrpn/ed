# tire (tiny-render) editor

A small scene editor with Qt-based UI and vulkan scenegraph.  

Built with:

- **VulkanSceneGraph (VSG)** as scenegraph
- **Qt6** for the user interface

## Dependencies Installation
  
### glslang tools
```bash
apt install glslang-dev glslang-tools
```

### Qt 6.10+  
Build yourself ore use binaries provided by your system:  
```bash
# Download from https://www.qt.io/download
# Or use package manager:
sudo apt install qt6-base-dev qt6-declarative-dev
```

### Vulkan SDK  
Download and install from https://vulkan.lunarg.com/sdk/home  
Or use package manager:  
```bash
sudo apt install vulkan-sdk
```

### VulkanSceneGraph (VSG)

```bash
git clone https://github.com/vsg-dev/VulkanSceneGraph
cd VulkanSceneGraph
mkdir build && cd build
cmake ..
ninja -j {n}
cmake --install . --prefix /path/to/vsg
export vsg_DIR=/path/to/vsg
```

## Build  
Set required environment variables, create build directory, configure and build  
```bash
export VULKAN_SDK=/path/to/vulkan/sdk
export vsg_DIR=/path/to/vsg
export LOCAL_LATEST_QT=/path/to/qt

mkdir build
cd build

cmake ..
cmake --build .
```

