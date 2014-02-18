#pragma once
#include <functional>

#include "mpack/msgpack.h"
#include "mpack/json.h"
#include "mpack/typestruct.h"

namespace mpack{ 
namespace msgpack {
namespace rpc {
typedef std::function<void(packer&, unpacker&)> packedmethod;
}}}

#include "mpack/packedmethod.h"
#include "mpack/rpc.h"
#include "mpack/dispatcher.h"

