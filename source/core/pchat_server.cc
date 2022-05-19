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

#include "pchat_server.h"

#include "source/core/concurrency_manager.h"
#include "source/core/context.h"
#include "source/core/network_manager.h"
#include "source/core/persistence_manager.h"
#include "source/service/service_manager.h"
#include "source/utils/arguments_manager.h"

namespace pcs {

PchatServer::PchatServer() : context_(MakePtr<Context>()) {}

PchatServer::~PchatServer() = default;

int PchatServer::run(int argc, char **argv) {
  context_->arguments_manager().parse(argc, argv);
  context_->persistence_manager().build();
  context_->service_manager().build();
  context_->network_manager().bind();
  context_->network_manager().run();
  return 0;
}

}  // namespace pcs