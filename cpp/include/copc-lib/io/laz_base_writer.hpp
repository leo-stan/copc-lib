#ifndef COPCLIB_IO_LAZ_BASE_WRITER_H_
#define COPCLIB_IO_LAZ_BASE_WRITER_H_

#include <array>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>

#include "copc-lib/copc/copc_config.hpp"
#include "copc-lib/geometry/vector3.hpp"
#include "copc-lib/io/laz_base_writer.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/laz_config.hpp"
#include "copc-lib/las/points.hpp"
#include "copc-lib/las/utils.hpp"

namespace copc::laz
{

class BaseWriter
{

  public:
    BaseWriter(std::ostream &out_stream, const las::LazConfig &las_config);
    void InitWriter(std::ostream &out_stream, const las::LazConfig &las_config);

    void Close();
};
} // namespace copc::laz

#endif // COPCLIB_IO_LAZ_BASE_WRITER_H_
