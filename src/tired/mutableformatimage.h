
#pragma once

#include <vsg/all.h>

namespace vsg {

class VSG_DECLSPEC MutableFormatImage : public Inherit<Image, MutableFormatImage> {
public:
    MutableFormatImage( ref_ptr<Data> in_data = {} )
        : Inherit<Image, MutableFormatImage>( std::move( in_data ) ) {}

public:
    VkResult compile( Device* device ) override;
};

}  // namespace vsg