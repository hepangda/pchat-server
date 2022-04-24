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

#include "net_worker.h"

#include <zmq.hpp>

#include "source/service/service_manager.h"

namespace pcs {

NetWorker::NetWorker(Context &context, Ptr<zmq::context_t> &zmq_context)
    : context_(context), zmq_context_(zmq_context) {}

[[noreturn]] void NetWorker::run(const std::string &dealer_address) {
  socket_ = MakePtr<zmq::socket_t>(*zmq_context_, zmq::socket_type::rep);
  socket_->connect(dealer_address);

  while (true) {
    auto recv_message = MakePtr<zmq::message_t>();
    if (auto recv_res = socket_->recv(*recv_message, zmq::recv_flags::none); recv_res) {
      auto req_package = UnwrapMessage(recv_message);
      Ptr<ProtoMessage> rsp = nullptr;
      UInt64 flags = 0;

      if (auto service = context_.service_manager().getService(req_package.service_id(), req_package.method_id()); service) {
        Ptr<ProtoMessage> req = service->castProtoMessage(req_package.package());
        rsp = service->serve(std::move(req));
      } else {
        rsp = MakePtr<PchatEmpty>();
        flags |= PCF_NoService;
      }
      auto rsp_message = WrapMessage(WrapPackage(rsp, req_package.service_id(), req_package.method_id(), flags));
      socket_->send(*rsp_message, zmq::send_flags::none);
    }
  }
}

}  // namespace pcs