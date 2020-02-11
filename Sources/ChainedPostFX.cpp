#include "ChainedPostFX.h"

ChainedPostFX::ChainedPostFX(std::shared_ptr<PostFX> first, std::shared_ptr<PostFX> second)
: first{first}
, second{second} {

}