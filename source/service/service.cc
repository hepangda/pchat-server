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

#include "service.h"

#include <zmq.hpp>

namespace pcs {

PchatPackage WrapPackage(Ptr<ProtoMessage> &proto_class, int service_id, int method_id, UInt64 flags) {
  PchatPackage pchat_package;
  pchat_package.set_service_id(service_id);
  pchat_package.set_method_id(method_id);
  pchat_package.set_flags(flags);
  *pchat_package.mutable_package() = proto_class->SerializeAsString();
  return pchat_package;
}

Ptr<zmq::message_t> WrapMessage(const PchatPackage &pchat_package) {
  auto result = MakePtr<zmq::message_t>();
  auto package_str = pchat_package.SerializeAsString();
  result->rebuild(package_str.data(), package_str.size());
  return result;
}

Ptr<zmq::message_t> WrapMessage(Ptr<ProtoMessage> &proto_class, int service_id, int method_id, UInt64 flags) {
  return WrapMessage(WrapPackage(proto_class, service_id, method_id, flags));
}

PchatPackage UnwrapMessage(Ptr<zmq::message_t> &message) {
  PchatPackage package;
  package.ParseFromArray(message->data(), message->size());
  return package;
}

}  // namespace pcs