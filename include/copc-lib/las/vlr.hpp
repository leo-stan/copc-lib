#ifndef COPCLIB_LAS_VLR_H_
#define COPCLIB_LAS_VLR_H_

#include <cstring>
#include <vector>

#include "lazperf/vlr.hpp"

namespace copc::las
{

// TODO[Leo] (EXTENTS) Update this once new COPC specs have been merged.
struct CopcExtent
{
    double min;
    double max;
};

class CopcExtentsVlr
{
  public:
    CopcExtentsVlr() = default;
    CopcExtentsVlr(const CopcExtentsVlr &copc_extents) { extents = copc_extents.extents; };

    std::vector<CopcExtent> extents;
};

using WktVlr = lazperf::wkt_vlr;
using CopcInfoVlr = lazperf::copc_vlr;
using EbVlr = lazperf::eb_vlr;
using VlrHeader = lazperf::vlr_header;
// TODO[Leo] (EXTENTS) Update this once new COPC specs have been merged.
// CopcExtentsVlr = std::vector<CopcExtent>;

} // namespace copc::las

namespace lazperf
{
// Equality operations
inline bool operator==(const eb_vlr::ebfield &a, const eb_vlr::ebfield &b)
{
    return std::memcmp(a.reserved, b.reserved, 2) == 0 && a.data_type == b.data_type && a.options == b.options &&
           a.name == b.name && std::memcmp(a.no_data, b.no_data, 3) == 0 && std::memcmp(a.minval, b.minval, 3) == 0 &&
           std::memcmp(a.maxval, b.maxval, 3) == 0 && std::memcmp(a.scale, b.scale, 3) == 0 &&
           std::memcmp(a.offset, b.offset, 3) == 0 && a.description == b.description;
}
} // namespace lazperf

#endif // COPCLIB_LAS_VLR_H_
