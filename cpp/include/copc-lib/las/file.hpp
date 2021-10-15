#ifndef COPCLIB_LAS_FILE_H_
#define COPCLIB_LAS_FILE_H_

#include <iostream>
#include <map>

#include "copc-lib/geometry/vector3.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc::las
{
class LasFile
{
  public:
    LasFile(const LasHeader &header, const las::EbVlr &eb) : header_(header), eb_vlr_(eb){};

    std::map<uint64_t, las::VlrHeader> vlrs; // maps from absolute offsets to VLR entries
    LasHeader GetLasHeader() const { return header_; }

    // Extra bytes
    las::EbVlr GetExtraBytes() const { return eb_vlr_; }

    // Update header
    void SetMin(const Vector3 &min) { header_.min = min; }
    void SetMax(const Vector3 &max) { header_.max = max; }
    void SetPointsByReturn(const std::array<uint64_t, 15> &points_by_return_14)
    {
        header_.points_by_return = points_by_return_14;
    }

  protected:
    LasHeader header_;
    las::EbVlr eb_vlr_;
};
} // namespace copc::las
#endif // COPCLIB_LAS_FILE_H_
