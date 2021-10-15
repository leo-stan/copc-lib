import copclib as copc


def reader_example():
    # Create a reader object
    reader = copc.FileReader("autzen-classified.copc.laz")

    # We can get the CopcData struct
    copc_vlr = reader.copc_info
    print("CopcData: ")
    print("\tSpacing: %d" % copc_vlr.spacing)
    print("\tRoot Offset: %d" % copc_vlr.root_hier_offset)
    print("\tRoot Size: %d" % copc_vlr.root_hier_size)

    # Get the Las Header
    las_header = reader.las_header
    print()
    print("Las Header:")
    print("\tPoint Format: %d" % las_header.point_format_id)
    print("\tPoint Count: %d" % las_header.point_count)

    # Get the WKT string
    print("WKT: %s" % reader.wkt)

    load_key = (4, 11, 9, 0)

    # FindNode will automatically load the minimum pages needed
    # to find the key you request
    node = reader.FindNode(load_key)
    # If FindNode can't find the node, it will return an "invalid" node:
    if not node.IsValid():
        return
    # GetPoints returns a Points object, which provides helper functions
    # as well as a Get() function to access the underlying point vector
    node_points = reader.GetPoints(node)

    print(node_points)

    print("First 5 points:")
    # Points object supports slicing
    for point in node_points[:5]:
        print(point)

    # We can also get the raw compressed data if we want to decompress it ourselves:

    loadKey = (4, 11, 9, 0)

    node = reader.FindNode(loadKey)
    if not node.IsValid():
        return
    compressed_data = reader.GetPointDataCompressed(node)

    # # We can decompress `n` number of points, or we can decompress the entire node
    # # by setting n=node.point_count
    num_points_to_decompress = 5
    uncompressed_data = copc.DecompressBytes(
        compressed_data, las_header, num_points_to_decompress
    )

    print(
        "Successfully decompressed %d points!"
        % (len(uncompressed_data) / las_header.point_record_length)
    )

    reader.Close()


if __name__ == "__main__":
    reader_example()
