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

#include "source/core/context.h"
#include "source/foundation/foundation.h"

namespace pcs {

class Service {
 public:
  explicit Service(Context &context) : context_(context) {}
  virtual ~Service() = default;

  virtual Ptr<ProtoMessage> serve(Ptr<ProtoMessage> proto) = 0;
  virtual Ptr<ProtoMessage> castProtoMessage(const std::string &buffer) = 0;

 protected:
  Context &context() { return context_; }

 private:
  Context &context_;
};

class ServiceMapper {
 public:
  virtual ~ServiceMapper() = default;

  [[nodiscard]] virtual int service_id() = 0;
  virtual Ptr<Service> fromMethodId(Context &context, int method_id) = 0;
  virtual Ptr<Service> fromMethodName(Context &context, const std::string &method_name) = 0;
};

template <typename T, typename U>
inline decltype(auto) CastProtoMessage(U &u) {
  auto t = MakePtr<T>();
  if constexpr (std::is_same_v<typename std::remove_cvref<U>::type, std::string>) {
    t->ParseFromString(u);
  } else {
    t->CopyFrom(u);
  }
  return t;
}

PchatPackage WrapPackage(Ptr<ProtoMessage> &proto_class, int service_id, int method_id, UInt64 flags = pcs::PCF_None);

Ptr<zmq::message_t> WrapMessage(const PchatPackage &pchat_package);

Ptr<zmq::message_t> WrapMessage(Ptr<ProtoMessage> &proto_class, int service_id, int method_id, UInt64 flags = pcs::PCF_None);

PchatPackage UnwrapMessage(Ptr<zmq::message_t> &message);

}  // namespace pcs