#pragma once

#include <vector>

#include "Runtime/Streams/IOStreams.hpp"
#include "Runtime/Character/CSegId.hpp"

namespace metaforce {
class CPoseAsTransforms;

class CSkinBank {
  std::vector<CSegId> x0_segments;

public:
  explicit CSkinBank(CInputStream& in);
  void GetBankTransforms(std::vector<const zeus::CTransform*>& out, const CPoseAsTransforms& pose) const;
};

} // namespace metaforce
