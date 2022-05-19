#include "source/core/pchat_server.h"

int main(int argc, char *argv[]) {
  return pcs::PchatServer{}.run(argc, argv);
}
