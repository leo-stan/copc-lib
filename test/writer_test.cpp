#include <cstring>
#include <sstream>
#include <string>

#include <catch2/catch.hpp>
#include <copc-lib/copc/extents.hpp>
#include <copc-lib/geometry/vector3.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/io/writer.hpp>
#include <lazperf/readers.hpp>

using namespace copc;
using namespace std;

TEST_CASE("Writer Config Tests", "[Writer]")
{
    GIVEN("A valid file_path")
    {
        SECTION("Default Config")
        {
            string file_path = "writer_test.copc.laz";

            CopcConfig cfg(6);
            FileWriter writer(file_path, cfg);

            auto las_header = writer.GetLasHeader();
            REQUIRE(las_header.scale.z == 0.01);
            REQUIRE(las_header.offset.z == 0);
            REQUIRE(las_header.point_format_id == 6);

            cfg.ToString();

            writer.Close();

            REQUIRE_THROWS(CopcConfig(5));
            REQUIRE_THROWS(CopcConfig(9));
        }

        SECTION("Custom Config")
        {
            string file_path = "writer_test.copc.laz";

            CopcConfig cfg(8, {2, 3, 4}, {-0.02, -0.03, -40.8});
            cfg.las_header_base.file_source_id = 200;

            // Test checks on string attributes
            cfg.las_header_base.SystemIdentifier("test_string");
            REQUIRE(cfg.las_header_base.SystemIdentifier() == "test_string");
            REQUIRE_THROWS(cfg.las_header_base.SystemIdentifier(std::string(33, 'a')));
            cfg.las_header_base.GeneratingSoftware("test_string");
            REQUIRE(cfg.las_header_base.GeneratingSoftware() == "test_string");
            REQUIRE_THROWS(cfg.las_header_base.GeneratingSoftware(std::string(33, 'a')));

            FileWriter writer(file_path, cfg);

            auto las_header = writer.GetLasHeader();
            REQUIRE(las_header.file_source_id == 200);
            REQUIRE(las_header.point_format_id == 8);
            REQUIRE(las_header.scale.x == 2);
            REQUIRE(las_header.offset.x == -0.02);
            REQUIRE(las_header.scale.y == 3);
            REQUIRE(las_header.offset.y == -0.03);
            REQUIRE(las_header.scale.z == 4);
            REQUIRE(las_header.offset.z == -40.8);

            writer.Close();
        }

        SECTION("COPC Spacing")
        {
            string file_path = "writer_test.copc.laz";

            {
                CopcConfig cfg(6);
                cfg.copc_info.spacing = 10;
                FileWriter writer(file_path, cfg);

                // todo: use Reader to check all of these
                auto spacing = writer.GetCopcInfo().spacing;
                REQUIRE(spacing == 10);

                writer.Close();
            }
            FileReader reader(file_path);
            REQUIRE(reader.GetCopcInfo().spacing == 10);
        }

        SECTION("Extents")
        {
            string file_path = "writer_test.copc.laz";

            CopcConfig cfg(6);
            FileWriter writer(file_path, cfg);

            auto extents = writer.GetCopcExtents();
            extents.Intensity()->minimum = -1.0;
            extents.Intensity()->maximum = 1;

            extents.Classification()->minimum = -std::numeric_limits<double>::max();
            extents.Classification()->maximum = std::numeric_limits<double>::max();

            writer.SetCopcExtents(extents);

            REQUIRE(writer.GetCopcExtents().Intensity()->minimum == extents.Intensity()->minimum);
            REQUIRE(writer.GetCopcExtents().Intensity()->maximum == extents.Intensity()->maximum);
            REQUIRE(writer.GetCopcExtents().Classification()->minimum == extents.Classification()->minimum);
            REQUIRE(writer.GetCopcExtents().Classification()->maximum == extents.Classification()->maximum);

            writer.Close();

            // Test reading of extents
            FileReader reader(file_path);
            REQUIRE(reader.GetCopcExtents().Intensity()->minimum == extents.Intensity()->minimum);
            REQUIRE(reader.GetCopcExtents().Intensity()->maximum == extents.Intensity()->maximum);
            REQUIRE(reader.GetCopcExtents().Classification()->minimum == extents.Classification()->minimum);
            REQUIRE(reader.GetCopcExtents().Classification()->maximum == extents.Classification()->maximum);
        }

        SECTION("WKT")
        {
            string file_path = "writer_test.copc.laz";

            CopcConfig cfg(6);
            cfg.wkt = "TEST_WKT";
            FileWriter writer(file_path, cfg);

            // todo: use Reader to check all of these
            REQUIRE(writer.GetWkt() == "TEST_WKT");

            writer.Close();

            FileReader reader(file_path);
            REQUIRE(reader.GetWkt() == "TEST_WKT");
        }
        SECTION("Copy")
        {
            FileReader orig("autzen-classified.copc.laz");

            string file_path = "writer_test.copc.laz";
            auto cfg = orig.GetCopcConfig();
            FileWriter writer(file_path, cfg);
            writer.Close();

            FileReader reader(file_path);
            REQUIRE(reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id);
            REQUIRE(reader.GetLasHeader().global_encoding == orig.GetLasHeader().global_encoding);
            REQUIRE(reader.GetLasHeader().creation_day == orig.GetLasHeader().creation_day);
            REQUIRE(reader.GetLasHeader().creation_year == orig.GetLasHeader().creation_year);
            REQUIRE(reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id);
            REQUIRE(reader.GetLasHeader().point_format_id == orig.GetLasHeader().point_format_id);
            REQUIRE(reader.GetLasHeader().point_record_length == orig.GetLasHeader().point_record_length);
            REQUIRE(reader.GetLasHeader().point_count == 0);
            REQUIRE(reader.GetLasHeader().scale == reader.GetLasHeader().scale);
            REQUIRE(reader.GetLasHeader().offset == reader.GetLasHeader().offset);
        }
    }
    GIVEN("A valid output stream")
    {
        SECTION("Default Config")
        {
            stringstream out_stream;

            CopcConfig cfg(6);
            Writer writer(out_stream, cfg);

            auto las_header = writer.GetLasHeader();
            REQUIRE(las_header.scale.z == 0.01);
            REQUIRE(las_header.offset.z == 0);
            REQUIRE(las_header.point_format_id == 6);

            writer.Close();

            lazperf::reader::generic_file f(out_stream);
            REQUIRE(f.pointCount() == 0);
            REQUIRE(f.header().scale.z == 0.01);
            REQUIRE(f.header().offset.z == 0);
            REQUIRE(f.header().point_format_id == 6);
        }

        SECTION("Custom Config")
        {
            stringstream out_stream;

            CopcConfig cfg(8, {2, 3, 4}, {-0.02, -0.03, -40.8});
            cfg.las_header_base.file_source_id = 200;
            Writer writer(out_stream, cfg);

            auto las_header = writer.GetLasHeader();
            REQUIRE(las_header.file_source_id == 200);
            REQUIRE(las_header.point_format_id == 8);
            REQUIRE(las_header.scale.x == 2);
            REQUIRE(las_header.offset.x == -0.02);
            REQUIRE(las_header.scale.y == 3);
            REQUIRE(las_header.offset.y == -0.03);
            REQUIRE(las_header.scale.z == 4);
            REQUIRE(las_header.offset.z == -40.8);

            writer.Close();

            lazperf::reader::generic_file f(out_stream);
            REQUIRE(f.pointCount() == 0);
            REQUIRE(f.header().file_source_id == 200);
            REQUIRE(f.header().point_format_id == 8);
            REQUIRE(f.header().scale.x == 2);
            REQUIRE(f.header().offset.x == -0.02);
            REQUIRE(f.header().scale.y == 3);
            REQUIRE(f.header().offset.y == -0.03);
            REQUIRE(f.header().scale.z == 4);
            REQUIRE(f.header().offset.z == -40.8);
        }

        SECTION("COPC Spacing")
        {
            stringstream out_stream;

            CopcConfig cfg(6);
            cfg.copc_info.spacing = 10;
            Writer writer(out_stream, cfg);

            // todo: use Reader to check all of these
            auto spacing = writer.GetCopcInfo().spacing;
            REQUIRE(spacing == 10);

            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.GetCopcInfo().spacing == 10);
        }

        SECTION("WKT")
        {
            stringstream out_stream;

            CopcConfig cfg(6);
            cfg.wkt = "TEST_WKT";
            Writer writer(out_stream, cfg);

            // todo: use Reader to check all of these
            REQUIRE(writer.GetWkt() == "TEST_WKT");

            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.GetWkt() == "TEST_WKT");
        }

        SECTION("Copy")
        {
            fstream in_stream;
            in_stream.open("autzen-classified.copc.laz", ios::in | ios::binary);
            Reader orig(&in_stream);

            stringstream out_stream;
            auto cfg = orig.GetCopcConfig();

            Writer writer(out_stream, cfg);
            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id);
            REQUIRE(reader.GetLasHeader().global_encoding == orig.GetLasHeader().global_encoding);
            REQUIRE(reader.GetLasHeader().creation_day == orig.GetLasHeader().creation_day);
            REQUIRE(reader.GetLasHeader().creation_year == orig.GetLasHeader().creation_year);
            REQUIRE(reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id);
            REQUIRE(reader.GetLasHeader().point_format_id == orig.GetLasHeader().point_format_id);
            REQUIRE(reader.GetLasHeader().point_record_length == orig.GetLasHeader().point_record_length);
            REQUIRE(reader.GetLasHeader().point_count == 0);
            REQUIRE(reader.GetLasHeader().scale == reader.GetLasHeader().scale);
            REQUIRE(reader.GetLasHeader().offset == reader.GetLasHeader().offset);
        }

        SECTION("Update")
        {
            stringstream out_stream;
            const Vector3 min1 = {-800, 300, 800};
            const Vector3 max1 = {5000, 8444, 3333};
            const Vector3 min2 = {-20, -30, -40};
            const Vector3 max2 = {20, 30, 40};
            std::array<uint64_t, 15> points_by_return = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

            CopcConfig cfg(6);
            cfg.las_header_base.min = min1;
            cfg.las_header_base.max = max1;
            cfg.copc_info.spacing = 10;
            cfg.wkt = "TEST_WKT";
            Writer writer(out_stream, cfg);

            REQUIRE(writer.GetLasHeader().min == min1);
            REQUIRE(writer.GetLasHeader().max == max1);
            REQUIRE(writer.GetLasHeader().points_by_return == std::array<uint64_t, 15>{0});

            writer.SetMin(min2);
            writer.SetMax(max2);
            writer.SetPointsByReturn(points_by_return);

            REQUIRE(writer.GetLasHeader().min == min2);
            REQUIRE(writer.GetLasHeader().max == max2);
            REQUIRE(writer.GetLasHeader().points_by_return == points_by_return);

            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.GetLasHeader().min == min2);
            REQUIRE(reader.GetLasHeader().max == max2);
            REQUIRE(reader.GetLasHeader().points_by_return == points_by_return);
        }
    }
}

TEST_CASE("Writer Pages", "[Writer]")
{
    SECTION("Root Page")
    {
        stringstream out_stream;

        Writer writer(out_stream, CopcConfig(6));

        REQUIRE(!writer.FindNode(VoxelKey::BaseKey()).IsValid());
        REQUIRE(!writer.FindNode(VoxelKey::InvalidKey()).IsValid());
        REQUIRE(!writer.FindNode(VoxelKey(5, 4, 3, 2)).IsValid());

        REQUIRE_NOTHROW(writer.GetRootPage());
        Page root_page = writer.GetRootPage();
        REQUIRE(root_page.IsValid());
        REQUIRE(root_page.IsPage());
        REQUIRE(root_page.loaded == true);

        REQUIRE_THROWS(writer.AddSubPage(root_page, VoxelKey::InvalidKey()));

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.GetCopcInfo().root_hier_offset > 0);
        REQUIRE(reader.GetCopcInfo().root_hier_size == 0);
        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());
    }

    SECTION("Nested Page")
    {
        stringstream out_stream;

        Writer writer(out_stream, CopcConfig(6));

        Page root_page = writer.GetRootPage();

        auto sub_page = writer.AddSubPage(root_page, VoxelKey(1, 1, 1, 1));
        REQUIRE(sub_page.IsPage());
        REQUIRE(sub_page.IsValid());
        REQUIRE(sub_page.loaded == true);

        REQUIRE_THROWS(writer.AddSubPage(sub_page, VoxelKey(1, 1, 1, 0)));
        REQUIRE_THROWS(writer.AddSubPage(sub_page, VoxelKey(2, 4, 5, 0)));

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.GetCopcInfo().root_hier_offset > 0);
        REQUIRE(reader.GetCopcInfo().root_hier_size == 32);
        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());
    }
}

TEST_CASE("Writer EBs", "[Writer]")
{
    SECTION("Data type 0")
    {
        stringstream out_stream;
        las::EbVlr eb_vlr(1); // Always initialize with the ebCount constructor
        // don't make ebfields yourself unless you set their names correctly
        eb_vlr.items[0].data_type = 0;
        eb_vlr.items[0].options = 4;
        CopcConfig config(7, {}, {}, eb_vlr);
        Writer writer(out_stream, config);

        REQUIRE(writer.GetLasHeader().point_record_length == 40); // 36 + 4

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.GetExtraByteVlr().items.size() == 1);
        REQUIRE(reader.GetExtraByteVlr().items[0].data_type == 0);
        REQUIRE(reader.GetExtraByteVlr().items[0].options == 4);
        REQUIRE(reader.GetExtraByteVlr().items[0].name == "FIELD_0");
        REQUIRE(reader.GetExtraByteVlr().items[0].maxval[2] == 0);
        REQUIRE(reader.GetExtraByteVlr().items[0].minval[2] == 0);
        REQUIRE(reader.GetExtraByteVlr().items[0].offset[2] == 0);
        REQUIRE(reader.GetExtraByteVlr().items[0].scale[2] == 0);
        REQUIRE(reader.GetLasHeader().point_record_length == 40);
    }

    SECTION("Data type 29")
    {
        stringstream out_stream;
        las::EbVlr eb_vlr(1);
        eb_vlr.items[0].data_type = 29;
        CopcConfig config(7, {}, {}, eb_vlr);
        Writer writer(out_stream, config);

        REQUIRE(writer.GetLasHeader().point_record_length == 48); // 36 + 12

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.GetExtraByteVlr().items.size() == 1);
        REQUIRE(reader.GetLasHeader().point_record_length == 48);
    }
}
TEST_CASE("Writer Copy", "[Writer]")
{

    SECTION("Copy Copc Config")
    {
        FileReader reader("autzen-classified.copc.laz");

        stringstream out_stream;
        auto cfg = reader.GetCopcConfig();
        cfg.las_header_base.scale = {1, 1, 1}; // Update some values in the process
        cfg.las_header_base.offset = {50, 50, 50};

        Writer writer(out_stream, cfg);

        REQUIRE(writer.GetLasHeader().point_record_length == reader.GetLasHeader().point_record_length);
        REQUIRE(writer.GetLasHeader().scale == Vector3(1, 1, 1));
        REQUIRE(writer.GetLasHeader().offset == Vector3(50, 50, 50));
        REQUIRE(writer.GetLasHeader().min == reader.GetLasHeader().min);
        REQUIRE(writer.GetLasHeader().max == reader.GetLasHeader().max);

        writer.Close();

        Reader new_reader(&out_stream);
        REQUIRE(new_reader.GetLasHeader().point_record_length == reader.GetLasHeader().point_record_length);
        REQUIRE(new_reader.GetLasHeader().scale == Vector3(1, 1, 1));
        REQUIRE(new_reader.GetLasHeader().offset == Vector3(50, 50, 50));
        REQUIRE(new_reader.GetLasHeader().min == reader.GetLasHeader().min);
        REQUIRE(new_reader.GetLasHeader().max == reader.GetLasHeader().max);
        REQUIRE(new_reader.GetCopcInfo().spacing == reader.GetCopcInfo().spacing);
        REQUIRE(new_reader.GetCopcExtents().Intensity()->minimum == reader.GetCopcExtents().Intensity()->minimum);
        REQUIRE(new_reader.GetWkt() == reader.GetWkt());
        REQUIRE(new_reader.GetExtraByteVlr().items == reader.GetExtraByteVlr().items);
    }

    SECTION("Autzen")
    {
        FileReader reader("autzen-classified.copc.laz");

        stringstream out_stream;

        auto cfg = reader.GetCopcConfig();

        Writer writer(out_stream, cfg);

        Page root_page = writer.GetRootPage();

        for (const auto &node : reader.GetAllChildren())
        {
            // only write/compare compressed data or otherwise tests take too long
            writer.AddNodeCompressed(root_page, node.key, reader.GetPointDataCompressed(node), node.point_count);
        }

        writer.Close();

        Reader new_reader(&out_stream);

        for (const auto &node : reader.GetAllChildren())
        {
            REQUIRE(node.IsValid());
            auto new_node = new_reader.FindNode(node.key);
            REQUIRE(new_node.IsValid());
            REQUIRE(new_node.key == node.key);
            REQUIRE(new_node.point_count == node.point_count);
            REQUIRE(new_node.byte_size == node.byte_size);
            REQUIRE(new_reader.GetPointDataCompressed(new_node) == reader.GetPointDataCompressed(node));
        }

        // we can do one uncompressed comparison here
        REQUIRE(new_reader.GetPointData(new_reader.FindNode(VoxelKey(5, 9, 7, 0))) ==
                reader.GetPointData(reader.FindNode(VoxelKey(5, 9, 7, 0))));
    }
}

TEST_CASE("Check Spatial Bounds", "[Writer]")
{
    string file_path = "writer_test.copc.laz";
    CopcConfig cfg(7, {0.1, 0.1, 0.1}, {50, 50, 50});
    cfg.las_header_base.min = {-10, -10, -5};
    cfg.las_header_base.max = {10, 10, 5};
    bool verbose = false;

    SECTION("Las Header Bounds check (pass)")
    {
        FileWriter writer(file_path, cfg);

        auto header = writer.GetLasHeader();
        Page root_page = writer.GetRootPage();

        las::Points points(header.point_format_id, header.scale, header.offset);

        auto point = points.CreatePoint();
        point->X(10);
        point->Y(10);
        point->Z(5);

        points.AddPoint(point);
        writer.AddNode(root_page, {1, 1, 1, 1}, points);
        writer.Close();

        FileReader reader(file_path);

        REQUIRE(reader.ValidateSpatialBounds(verbose) == true);
    }

    SECTION("Las Header Bounds check (node outside)")
    {
        FileWriter writer(file_path, cfg);

        auto header = writer.GetLasHeader();
        Page root_page = writer.GetRootPage();

        las::Points points(header.point_format_id, header.scale, header.offset);

        auto point = points.CreatePoint();
        point->X(10);
        point->Y(10);
        point->Z(5.1);

        points.AddPoint(point);
        writer.AddNode(root_page, {2, 3, 3, 3}, points);
        writer.Close();

        FileReader reader(file_path);

        REQUIRE(reader.ValidateSpatialBounds(verbose) == false);
    }

    SECTION("Las Header Bounds check (node intersects)")
    {
        FileWriter writer(file_path, cfg);

        auto header = writer.GetLasHeader();
        Page root_page = writer.GetRootPage();

        las::Points points(header.point_format_id, header.scale, header.offset);

        auto point = points.CreatePoint();
        point->X(10);
        point->Y(10);
        point->Z(5.1);

        points.AddPoint(point);
        writer.AddNode(root_page, {1, 1, 1, 1}, points);
        writer.Close();

        FileReader reader(file_path);

        REQUIRE(reader.ValidateSpatialBounds(verbose) == false);
    }

    SECTION("Node Bounds check")
    {
        FileWriter writer(file_path, cfg);

        auto header = writer.GetLasHeader();
        Page root_page = writer.GetRootPage();

        las::Points points(header.point_format_id, header.scale, header.offset);

        auto point = points.CreatePoint();
        point->X(0.1);
        point->Y(0.1);
        point->Z(0.1);

        points.AddPoint(point);
        writer.AddNode(root_page, {1, 0, 0, 0}, points);
        writer.Close();

        FileReader reader(file_path);
        REQUIRE(reader.ValidateSpatialBounds(verbose) == false);
    }
}
