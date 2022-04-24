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

class NetworkManager {
 public:
  explicit NetworkManager(Context &context);
  ~NetworkManager();

  void bind();
  void run();

  [[nodiscard]] const std::string &serverRouterEndpoint() const { return server_router_endpoint_; }
  [[nodiscard]] const std::string &serverDealerEndpoint() const { return server_dealer_endpoint_; }

 private:
  Context &context_;
  Ptr<zmq::context_t> zmq_context_;
  Ptr<zmq::socket_t> server_router_socket_;
  Ptr<zmq::socket_t> server_dealer_socket_;

  std::string server_router_endpoint_;
  std::string server_dealer_endpoint_;
};

}  // namespace pcs
