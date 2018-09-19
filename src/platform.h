#ifndef PLATFORM_H
#define PLATFORM_H

#include "log.h"

#include <cstdint>
#include <string>

class Runtime;
enum DeviceId   : uint32_t {};
enum PlatformId : uint32_t {};

enum class KernelArgType : uint8_t { Val = 0, Ptr, Struct };

enum class BorderMode {
    Clamp = 0,
    Repeat,
    Mirror
};

enum class TextureFormat {
    Int8 = 0,
    Int16,
    Int32,
    Uint8,
    Uint16,
    Uint32,
    Float32
};

enum class FilterMode {
    Nearest = 0,
    Linear
};

struct TextureDesc {
    TextureFormat format;
    BorderMode border_x;
    BorderMode border_y;
    FilterMode filter;
    size_t width;
    size_t height;
    size_t pitch;
    size_t num_channels;
};

/// A runtime platform. Exposes a set of devices, a copy function,
/// and functions to allocate and release memory.
class Platform {
public:
    Platform(Runtime* runtime)
        : runtime_(runtime)
    {}

    virtual ~Platform() {}

    /// Allocates memory for a device on this platform.
    virtual void* alloc(DeviceId dev, int64_t size) = 0;
    /// Allocates page-locked host memory for a platform (and a device).
    virtual void* alloc_host(DeviceId dev, int64_t size) = 0;
    /// Allocates unified memory for a platform (and a device).
    virtual void* alloc_unified(DeviceId dev, int64_t size) = 0;
    /// Returns the device memory associated with the page-locked memory.
    virtual void* get_device_ptr(DeviceId dev, void* ptr) = 0;
    /// Releases memory for a device on this platform.
    virtual void release(DeviceId dev, void* ptr) = 0;
    /// Releases page-locked host memory for a device on this platform.
    virtual void release_host(DeviceId dev, void* ptr) = 0;

    /// Allocates a texture on a device, using a device memory pointer
    virtual void* alloc_tex(DeviceId dev, void* data, const TextureDesc&) = 0;
    /// Release a texture on the device
    virtual void release_tex(DeviceId dev, void* tex) = 0;

    /// Associate a program string to a given filename.
    virtual void register_file(const std::string& filename, const std::string& program_string) = 0;

    /// Launches a kernel with the given block/grid size and arguments.
    virtual void launch_kernel(DeviceId dev,
                               const char* file, const char* kernel,
                               const uint32_t* grid, const uint32_t* block,
                               void** args, const uint32_t* size, const KernelArgType* types,
                               uint32_t num_args) = 0;
    /// Waits for the completion of all the launched kernels on the given device.
    virtual void synchronize(DeviceId dev) = 0;

    /// Copies memory. Copy can only be performed devices in the same platform.
    virtual void copy(DeviceId dev_src, const void* src, int64_t offset_src, DeviceId dev_dst, void* dst, int64_t offset_dst, int64_t size) = 0;
    /// Copies memory from the host (CPU).
    virtual void copy_from_host(const void* src, int64_t offset_src, DeviceId dev_dst, void* dst, int64_t offset_dst, int64_t size) = 0;
    /// Copies memory to the host (CPU).
    virtual void copy_to_host(DeviceId dev_src, const void* src, int64_t offset_src, void* dst, int64_t offset_dst, int64_t size) = 0;

    /// Returns the number of devices in this platform.
    virtual size_t dev_count() const = 0;
    /// Returns the platform name.
    virtual std::string name() const = 0;

protected:
    [[noreturn]] void platform_error() {
        error("The selected '%' platform is not available", name());
    }

    [[noreturn]] void command_unavailable(const std::string& command) {
        error("The command '%' is unavailable on platform '%'", command, name());
    }

    Runtime* runtime_;
};

#endif
