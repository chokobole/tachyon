#include "binfile_utils.hpp"

#include "tachyon/base/files/file_path.h"

namespace tachyon::circom {

void ProveRapidsnark(const base::FilePath& wtns_file) {
  auto wtns = BinFileUtils::openExisting(wtns_file.value(), "wtns", 2);
  auto wtnsHeader = WtnsUtils::loadHeader(wtns.get());
}

}  // namespace tachyon::circom
