#ifndef COPCLIB_COPC_FILE_H_
#define COPCLIB_COPC_FILE_H_

#include <string>

#include "copc-lib/copc/extents.hpp"
#include "copc-lib/copc/info.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/utils.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{
const int COPC_OFFSET = 429;

class CopcConfig
{
  public:
    CopcConfig(const las::LasHeader &header, const CopcInfo &copc_info, const CopcExtents &copc_extents,
               const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
        : header_(std::make_shared<las::LasHeader>(header)), eb_vlr_(std::make_shared<las::EbVlr>(extra_bytes_vlr)),
          copc_info_(std::make_shared<copc::CopcInfo>(copc_info)),
          copc_extents_(std::make_shared<copc::CopcExtents>(copc_extents)), wkt_(wkt){};

    las::LasHeader LasHeader() const { return *header_; }

    copc::CopcInfo CopcInfo() const { return *copc_info_; }

    copc::CopcExtents CopcExtents() const { return *copc_extents_; }

    std::string Wkt() const { return wkt_; }

    las::EbVlr ExtraBytesVlr() const { return *eb_vlr_; }

  protected:
    CopcConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset, const std::string &wkt,
               const las::EbVlr &extra_bytes_vlr);

    std::shared_ptr<las::LasHeader> header_;
    std::shared_ptr<copc::CopcInfo> copc_info_;
    std::shared_ptr<copc::CopcExtents> copc_extents_;
    std::string wkt_;
    std::shared_ptr<las::EbVlr> eb_vlr_;
};

class CopcConfigWriter : public CopcConfig
{
  public:
    CopcConfigWriter(const int8_t &point_format_id, const Vector3 &scale = Vector3::DefaultScale(),
                     const Vector3 &offset = Vector3::DefaultOffset(), const std::string &wkt = "",
                     const las::EbVlr &extra_bytes_vlr = {0});

    // Allow copy from CopcFile
    CopcConfigWriter(const CopcConfig &file)
        : CopcConfig(file.LasHeader(), file.CopcInfo(), file.CopcExtents(), file.Wkt(), file.ExtraBytesVlr()){};

    std::shared_ptr<las::LasHeader> LasHeader() { return header_; }

    std::shared_ptr<copc::CopcInfo> CopcInfo() { return copc_info_; }

    std::shared_ptr<copc::CopcExtents> CopcExtents() { return copc_extents_; }
};

} // namespace copc
#endif // COPCLIB_COPC_FILE_H_
