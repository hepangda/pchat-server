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

#include "source/foundation/foundation.h"

namespace pcs {

class NetworkManager;
class PersistenceManager;
class ConcurrencyManager;
class ServiceManager;
class ArgumentsManager;

class Context {
 public:
  Context();
  ~Context();

  [[nodiscard]] NetworkManager &network_manager() { return *network_manager_; }
  [[nodiscard]] PersistenceManager &persistence_manager() { return *persistence_manager_; }
  [[nodiscard]] ConcurrencyManager &concurrency_manager() { return *concurrency_manager_; }
  [[nodiscard]] ServiceManager &service_manager() { return *service_manager_; }
  [[nodiscard]] ArgumentsManager &arguments_manager() { return *arguments_manager_; }

 private:
  Ptr<NetworkManager> network_manager_;
  Ptr<PersistenceManager> persistence_manager_;
  Ptr<ConcurrencyManager> concurrency_manager_;
  Ptr<ServiceManager> service_manager_;
  Ptr<ArgumentsManager> arguments_manager_;
};

}  // namespace pcs
