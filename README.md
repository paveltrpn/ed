# tire (tiny-render) editor

A small scene editor with Qt-based UI and vulkan scenegraph.  

Editor built with:

- **Vulkan** via VulkanSceneGraph (VSG) for modern GPU rendering
- **Qt 6.10** for the user interface

## Dependencies Installation
  
### GLFW3, nlohmann-json and glslang tools
```bash
apt install nlohmann-json3-dev libglfw3 libglfw3-dev glslang-dev glslang-tools
```

### Qt 6.10+  
Build yourself ore use binaries provided by your system:  
```bash
# Download from https://www.qt.io/download
# Or use package manager:
sudo apt install qt6-base-dev qt6-declarative-dev
```

### Vulkan SDK

```bash
# Download and install from https://vulkan.lunarg.com/sdk/home
# Or use package manager:
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

```bash
# Set required environment variables
export VULKAN_SDK=/path/to/vulkan/sdk
export vsg_DIR=/path/to/vsg
export LOCAL_LATEST_QT=/path/to/qt

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .
```

