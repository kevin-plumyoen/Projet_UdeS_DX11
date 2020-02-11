#ifndef DEF_CHAINED_POST_FX_H
#define DEF_CHAINED_POST_FX_H

#include "PostFX.h"
#include <memory>

class ChainedPostFX : public PostFX {
    std::shared_ptr<PostFX> first;
    std::shared_ptr<PostFX> second;

public:
    ChainedPostFX(std::shared_ptr<PostFX> first, std::shared_ptr<PostFX> second);

};

#endif