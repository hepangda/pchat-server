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

#include "file.h"

namespace pcs {

File::File(const std::string& file_path, const char* mode) : fp(fopen(file_path.c_str(), mode)) {}

File::~File() { fclose(fp); }

std::string File::readToEnd() {
  std::fseek(fp, 0, SEEK_END);
  std::size_t file_size = std::ftell(fp);

  std::string result;
  result.resize(file_size);
  std::fseek(fp, 0, SEEK_SET);

  std::fread(result.data(), sizeof(uint8_t), file_size, fp);
  return result;
}

}  // namespace pcs