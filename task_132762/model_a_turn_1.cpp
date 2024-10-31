#include <iostream>
#include <string>
#include <vector>
#include <zlib.h>

int compressData(const std::string& input, std::vector<unsigned char>& output) {
	int ret;
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (15 + 16), 8, Z_DEFAULT_STRATEGY);
	if (ret != Z_OK) {
		std::cerr << "deflateInit2 failed: " << ret << std::endl;
		return ret;
	}

	strm.avail_in = static_cast<uInt>(input.size());
	strm.next_in = (Bytef*)input.data();

	int flush = Z_FINISH;

	do {
		output.resize(output.size() + 1024);
		strm.avail_out = static_cast<uInt>(output.size() - strm.total_out);
		strm.next_out = (Bytef*)output.data() + strm.total_out;
		ret = deflate(&strm, flush);    

		if (ret == Z_STREAM_END) break;
		if (ret != Z_OK) {
			std::cerr << "deflate failed: " << ret << std::endl;
			deflateEnd(&strm);
			return ret;
		}
	} while (strm.avail_out == 0);

	output.resize(strm.total_out);
	deflateEnd(&strm);
	return Z_OK;
}

int decompressData(const std::vector<unsigned char>& input, std::string& output) {
	int ret;
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = static_cast<uInt>(input.size());
	strm.next_in = (Bytef*)input.data();

	output.clear();

	ret = inflateInit2(&strm, (15 + 32));
	if (ret != Z_OK) {
		std::cerr << "inflateInit2 failed: " << ret << std::endl;
		return ret;
	}

	do {
		output.resize(output.size() + 1024);
		strm.avail_out = static_cast<uInt>(output.size() - strm.total_out);
		strm.next_out = (Bytef*)output.data() + strm.total_out;
		ret = inflate(&strm, Z_NO_FLUSH);

		if (ret == Z_STREAM_END) break;
		if (ret != Z_OK) {
			std::cerr << "inflate failed: " << ret << std::endl;
			inflateEnd(&strm);
			return ret;
		}
	} while (strm.avail_out == 0);

	output.resize(strm.total_out);
	inflateEnd(&strm);

	return Z_OK;
}

int main() {
	std::string message = "This is a sample message to be compressed.";

	std::vector<unsigned char> compressedData;
	int ret = compressData(message, compressedData);
	if (ret != Z_OK) {
		std::cerr << "Compression failed" << std::endl;
		return 1;
	}

	std::string decompressedMessage;
	ret = decompressData(compressedData, decompressedMessage);
	if (ret != Z_OK) {
		std::cerr << "Decompression failed" << std::endl;
		return 1;
	}

	std::cout << "Original message: " << message << std::endl;
	std::cout << "Compressed size: " << compressedData.size() << std::endl;
	std::cout << "Decompressed message: " << decompressedMessage << std::endl;
	return 0;
}