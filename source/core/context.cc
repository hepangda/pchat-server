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

#include "context.h"

#include "source/core/concurrency_manager.h"
#include "source/core/network_manager.h"
#include "source/core/persistence_manager.h"
#include "source/service/service_manager.h"
#include "source/utils/arguments_manager.h"

namespace pcs {

Context::Context()
    : network_manager_(MakePtr<NetworkManager>(*this)),
      persistence_manager_(MakePtr<PersistenceManager>(*this)),
      concurrency_manager_(MakePtr<ConcurrencyManager>(*this)),
      service_manager_(MakePtr<ServiceManager>(*this)),
      arguments_manager_(MakePtr<ArgumentsManager>(*this)) {}

Context::~Context() = default;

}  // namespace pcs