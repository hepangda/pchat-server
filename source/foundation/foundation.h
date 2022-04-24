/**
 *  Pchat-server: a simple chatroom server
 *  Copyright (C) 2017- Pangda

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3 of the License

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <optional>
#include <cstddef>
#include <string>
#include "source/protocol/common.pb.h"

using ProtoMessage = google::protobuf::Message;

namespace zmq {

class context_t;
class socket_t;
class message_t;

} // namespace zmq

namespace soci {

class connection_pool;
class session;
class statement;

} // namespace soci

namespace pcs {

template<typename T>
using Opt = std::optional<T>;

template<typename T>
using Ptr = std::unique_ptr<T>;

template<typename T, typename... Args>
inline decltype(auto) MakePtr(Args &&... args) {
  return std::make_unique<T>(std::forward<Args &&>(args)...);
}

template<typename T>
using Rc = std::shared_ptr<T>;

template<typename T, typename... Args>
inline decltype(auto) MakeRc(Args &&... args) {
  return std::make_shared<T>(std::forward<Args &&>(args)...);
}

using Byte = std::byte;

using Int16 = std::int16_t;
using UInt16 = std::uint16_t;

using Int32 = std::int32_t;
using UInt32 = std::uint32_t;

using Int64 = std::int64_t;
using UInt64 = std::uint64_t;

using Float32 = float;
using Float64 = double;

}

#ifndef PCS_RELEASE_VERSION_
#define PCS_RELEASE_VERSION_

#include <iostream>

#define pcs_debug_print(x) std::cout << #x << ": " << (x) << std::endl

#endif // PCS_RELEASE_VERSION_