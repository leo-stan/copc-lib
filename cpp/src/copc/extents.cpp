#include "copc-lib/copc/extents.hpp"

#include <sstream>

namespace copc
{

CopcExtent::CopcExtent(double minimum, double maximum, double mean, double var)
    : minimum(minimum), maximum(maximum), mean(mean), var(var)
{
    if (minimum > maximum)
        throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
    if (var < 0)
        throw std::runtime_error("CopcExtent: Variance must be >= 0.");
}

CopcExtent::CopcExtent(const std::vector<double> &vec)
{
    if (vec.size() != 2 && vec.size() != 4)
        throw std::runtime_error("CopcExtent: Vector size must be 2 or 4.");

    if (vec[0] > vec[1])
        throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
    minimum = vec[0];
    maximum = vec[1];

    if (vec.size() == 4)
    {
        mean = vec[2];
        var = vec[3];
    }
}

CopcExtent::CopcExtent(const CopcExtent &other)
    : minimum(other.minimum), maximum(other.maximum), mean(other.mean), var(other.var)
{
    if (other.minimum > other.maximum)
        throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
    if (var < 0)
        throw std::runtime_error("CopcExtent: Variance must be >= 0.");
}

std::string CopcExtent::ToString() const
{
    std::stringstream ss;
    ss << "(" << minimum << "/" << maximum << "/" << mean << "/" << var << ")";
    return ss.str();
}

// Empty constructor
CopcExtents::CopcExtents(int8_t point_format_id, uint16_t num_eb_items, bool has_extended_stats)
    : point_format_id_(point_format_id), has_extended_stats_(has_extended_stats)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("CopcExtents: Supported point formats are 6 to 8.");
    auto num_extents = NumberOfExtents(point_format_id, num_eb_items);
    extents_.reserve(num_extents);
    for (int i{0}; i < num_extents; i++)
        extents_.push_back(std::make_shared<CopcExtent>());
}

// Copy constructor
CopcExtents::CopcExtents(const CopcExtents &extents)
    : point_format_id_(extents.PointFormatId()), has_extended_stats_(extents.HasExtendedStats())
{
    extents_.reserve(extents.NumberOfExtents());
    for (int i{0}; i < extents.NumberOfExtents(); i++)
        extents_.push_back(std::make_shared<CopcExtent>(extents.Extents()[i]));
}

// VLR constructor
CopcExtents::CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t num_eb_items,
                         bool has_extended_stats)
    : point_format_id_(point_format_id), has_extended_stats_(has_extended_stats)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("CopcExtents: Supported point formats are 6 to 8.");

    if (vlr.items.size() - 3 !=
        NumberOfExtents(point_format_id, num_eb_items)) // -3 takes into account extra extents for x,y,z from LAS header
        throw std::runtime_error("CopcExtents: Number of extents incorrect.");
    extents_.reserve(NumberOfExtents(point_format_id, num_eb_items));
    for (int i = 3; i < vlr.items.size(); i++)
    {
        extents_.push_back(std::make_shared<CopcExtent>(vlr.items[i].minimum, vlr.items[i].maximum));
    }
}

las::CopcExtentsVlr CopcExtents::ToLazPerf(const CopcExtent &x, const CopcExtent &y, const CopcExtent &z) const
{
    las::CopcExtentsVlr vlr;
    vlr.items.reserve(extents_.size() + 3); // +3 takes into account extra extents for x,y,z from LAS header
    vlr.items.emplace_back(x.minimum, x.maximum);
    vlr.items.emplace_back(y.minimum, y.maximum);
    vlr.items.emplace_back(z.minimum, z.maximum);
    for (const auto &extent : extents_)
        vlr.items.emplace_back(extent->minimum, extent->maximum);

    return vlr;
}

las::CopcExtentsVlr CopcExtents::ToLazPerfExtended() const
{
    las::CopcExtentsVlr vlr;
    vlr.items.reserve(extents_.size() + 3); // +3 takes into account extra extents for x,y,z from LAS header
    vlr.items.emplace_back();               // TODO: Handle x,y,z later
    vlr.items.emplace_back();
    vlr.items.emplace_back();
    for (const auto &extent : extents_)
        vlr.items.emplace_back(extent->mean, extent->var); // Add mean/var instead of min/max
    return vlr;
}

void CopcExtents::SetExtendedStats(const las::CopcExtentsVlr &vlr)
{
    if (!has_extended_stats_)
        throw std::runtime_error("CopcExtents::SetExtendedStats: This instance does not have extended stats.");
    if (vlr.items.size() - 3 != extents_.size()) // -3 takes into account extra extents for x,y,z from LAS header
        throw std::runtime_error("CopcExtents::SetExtendedStats: Number of extended extents incorrect.");
    for (int i = 3; i < vlr.items.size(); i++)
    {
        extents_[i - 3]->mean = vlr.items[i].minimum;
        extents_[i - 3]->var = vlr.items[i].maximum;
    }
}

int CopcExtents::NumberOfExtents(int8_t point_format_id, uint16_t num_eb_items)
{
    return las::PointBaseNumberDimensions(point_format_id) - 3 +
           num_eb_items; // -3 disregards x,y,z since they are not handled in Extents
}

size_t CopcExtents::ByteSize(int8_t point_format_id, uint16_t num_eb_items)
{
    return CopcExtents(point_format_id, num_eb_items).ToLazPerf({}, {}, {}).size();
}

std::string CopcExtents::ToString() const
{
    std::stringstream ss;
    ss << "Copc Extents (Min/Max/Mean/Var):" << std::endl;
    ss << "\tIntensity: " << extents_[0]->ToString() << std::endl;
    ss << "\tReturn Number: " << extents_[1]->ToString() << std::endl;
    ss << "\tNumber Of Returns: " << extents_[2]->ToString() << std::endl;
    ss << "\tScanner Channel: " << extents_[3]->ToString() << std::endl;
    ss << "\tScan Direction Flag: " << extents_[4]->ToString() << std::endl;
    ss << "\tEdge Of Flight Line: " << extents_[5]->ToString() << std::endl;
    ss << "\tClassification: " << extents_[6]->ToString() << std::endl;
    ss << "\tUser Data: " << extents_[7]->ToString() << std::endl;
    ss << "\tScan Angle: " << extents_[8]->ToString() << std::endl;
    ss << "\tPoint Source ID: " << extents_[9]->ToString() << std::endl;
    ss << "\tGPS Time: " << extents_[10]->ToString() << std::endl;
    if (point_format_id_ > 6)
    {
        ss << "\tRed: " << extents_[11]->ToString() << std::endl;
        ss << "\tGreen: " << extents_[12]->ToString() << std::endl;
        ss << "\tBlue: " << extents_[13]->ToString() << std::endl;
    }
    if (point_format_id_ == 8)
        ss << "\tNIR: " << extents_[14]->ToString() << std::endl;
    ss << "\tExtra Bytes:" << std::endl;
    for (int i = las::PointBaseNumberDimensions(point_format_id_); i < extents_.size(); i++)
    {
        ss << "\t\t" << extents_[i]->ToString() << std::endl;
    }
    return ss.str();
}

} // namespace copc
