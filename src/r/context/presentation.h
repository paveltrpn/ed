
#pragma once

namespace tire {

struct VKInstance;
struct VKDevice;

struct Presentation final {
    Presentation( const VKInstance *instance,const VKDevice *device );

    Presentation( const Presentation &other ) = delete;
    Presentation( Presentation &&other ) = delete;

    auto operator=( const Presentation &other ) -> Presentation & = delete;
    auto operator=( Presentation &&other ) -> Presentation & = delete;

    ~Presentation();

private:
    const VKInstance *_instance{};
    const VKDevice *_device{};

};

}
