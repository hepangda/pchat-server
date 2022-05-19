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

#include "network_manager.h"

#include <zmq.hpp>

#include "source/core/concurrency_manager.h"
#include "source/network/net_worker.h"
#include "source/utils/arguments_manager.h"

namespace pcs {

NetworkManager::NetworkManager(Context &context)
    : context_(context),
      zmq_context_(MakePtr<zmq::context_t>()),
      server_router_socket_(MakePtr<zmq::socket_t>(*zmq_context_, zmq::socket_type::router)),
      server_dealer_socket_(MakePtr<zmq::socket_t>(*zmq_context_, zmq::socket_type::dealer)) {}

NetworkManager::~NetworkManager() = default;

void NetworkManager::bind() {
  server_router_endpoint_ = context_.arguments_manager().config().network().router_bind_address();
  server_router_socket_->bind(server_router_endpoint_);

  server_dealer_endpoint_ = context_.arguments_manager().config().network().dealer_bind_address();
  server_dealer_socket_->bind(server_dealer_endpoint_);
}

void NetworkManager::run() {
  UInt32 concurrency_count = context_.arguments_manager().config().network().rep_concurrency_count();
  for (UInt32 i = 0; i < concurrency_count; i++) {
    context_.concurrency_manager().async([this] {
      NetWorker net_worker{context_, zmq_context_};
      net_worker.run(serverDealerEndpoint());
    });
  }
  zmq::proxy(*server_router_socket_, *server_dealer_socket_);
}

}  // namespace pcs