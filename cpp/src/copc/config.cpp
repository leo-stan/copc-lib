#include "copc-lib/copc/config.hpp"

#include <stdexcept>

namespace copc
{

CopcConfigWriter::CopcConfigWriter(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
                                   const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
    : CopcConfig(point_format_id, scale, offset, wkt, extra_bytes_vlr)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");
}

CopcConfig::CopcConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
                       const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
    : header_(std::make_shared<las::LasHeader>(
          point_format_id, las::PointBaseByteSize(point_format_id) + las::NumBytesFromExtraBytes(extra_bytes_vlr.items),
          scale, offset)),
      copc_info_(std::make_shared<copc::CopcInfo>()),
      copc_extents_(std::make_shared<copc::CopcExtents>(point_format_id, extra_bytes_vlr.items.size())), wkt_(wkt),
      eb_vlr_(std::make_shared<las::EbVlr>(extra_bytes_vlr))
{
}
} // namespace copc