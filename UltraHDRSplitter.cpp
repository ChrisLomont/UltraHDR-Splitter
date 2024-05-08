// code to split Adobe/Android UltraHDR JPEG format into three files
// 1) main image, 2) gain map image, 3) gain map text info
// Chris Lomont, 2024, www.lomont.org
// C++ 20
// 
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <format>
#include <filesystem>
#include <regex>

using namespace std;
namespace fs = std::filesystem;


// simple UltraHdr parsing
// handles Google's UltraHdr format details
// https://developer.android.com/media/platform/hdr-image-format

class UltraHdr
{
public:
	double headerVersion{0}; // should be 1.0
	bool baseRenditionIsHdr{false};

	// these can be single values for one channel gain maps, some can be 3 values for RGB gain maps
	std::vector<double> gainMapMin, gainMapMax;
	std::vector<double> gamma;
	std::vector<double> offsetSdr, offsetHdr;
	std::vector<double> capacityMin, capacityMax;

	// feed xmp strings here
	bool ParseXmp(const string& text)
	{
		// nice C++ regex cheatsheet https://cpprocks.com/files/c++11-regex-cheatsheet.pdf
		std::vector<double> headerVersionVec;
		std::smatch hdrMatch, gainMinMatch, gainMaxMatch, gammaMatch, offsetSDRMatch, offsetHDRMatch, capacityMinMatch,
		            capacityMaxMatch, baseMatch;
		if (
			ParseValues(text, "Version", headerVersionVec, true) &&
			ParseValues(text, "GainMapMin", gainMapMin, false, 0.0) &&
			ParseValues(text, "GainMapMax", gainMapMax, true) &&
			ParseValues(text, "Gamma", gamma, false, 1.0) &&
			ParseValues(text, "OffsetSDR", offsetSdr, false, 1.0 / 64.0) &&
			ParseValues(text, "OffsetHDR", offsetHdr, false, 1.0 / 64.0) &&
			ParseValues(text, "HDRCapacityMin", capacityMin, false, 0.0) &&
			ParseValues(text, "HDRCapacityMax", capacityMax, true)
		)
		{
			headerVersion = headerVersionVec[0];
			// baseRendition, optional, default false
			std::smatch baseMatch;
			baseRenditionIsHdr =
				std::regex_search(text, baseMatch, std::regex{"hdrgm:BaseRenditionIsHDR=\"(True|False)\""})
				&& baseMatch.str(1) == "True";

			// validate parameters: todo;
			return true;
		}
		return false;
	}

	void Dump(std::ostream& os, std::string prefix = "") const
	{
		os << std::format("{}Version       : {}\n", prefix, headerVersion);
		os << std::format("{}BaseRenditionIsHDR: {}\n", prefix, baseRenditionIsHdr);
		os << std::format("{}GainMapMin    : {}\n", prefix, FormatV(gainMapMin));
		os << std::format("{}GainMapMax    : {}\n", prefix, FormatV(gainMapMax));
		os << std::format("{}Gamma         : {}\n", prefix, FormatV(gamma));
		os << std::format("{}OffsetSDR     : {}\n", prefix, FormatV(offsetSdr));
		os << std::format("{}OffsetHDR     : {}\n", prefix, FormatV(offsetHdr));
		os << std::format("{}HDRCapacityMin: {}\n", prefix, FormatV(capacityMin));
		os << std::format("{}HDRCapacityMax: {}\n", prefix, FormatV(capacityMax));
	}

private:
	static std::string FormatV(const std::vector<double>& vals)
	{
		std::stringstream ss;
		for (size_t i = 0; i < vals.size(); ++i)
		{
			if (i > 0) ss << ", ";
			ss << vals[i];
		}
		return ss.str();
	}


	static bool Same(double v1, double v2)
	{
		return fabs(v1 - v2) < 1e-5;
	}

	static bool ParseValues(const std::string& text, const std::string& item, std::vector<double>& values,
	                        bool required, double defaultValue = 0.0)
	{
		std::smatch match;

		// seems google HDR always has '.', Adobe Lightroom skips it for .0
		// google single gain map form: hdrgm:GainMapMin="0.000000"
		// Lightroom 3 channel form   :
		/*
		    <hdrgm:GainMapMin>
		        <rdf:Seq>
		            <rdf:li>-0.07811</rdf:li>
		            <rdf:li>-0.049089</rdf:li>
		            <rdf:li>-0.028652</rdf:li>
		        </rdf:Seq>
		    </hdrgm:GainMapMin>
		 */

		const std::string num = R"xx((\d+(\.\d+)?))xx"; // grouped number
		const std::string singleRegex = R"xx(hdrgm:)xx" + item + "=\"" + num + "\"";
		const auto singleMatch = std::regex_search(text, match, std::regex{singleRegex});
		auto vectorMatch = false;
		if (!singleMatch)
		{
			// number in 'li' tag
			const auto linum = Tag("rdf", "li") + num + Tag("rdf", "li", false);

			// open tags
			const std::string tripleRegex =
				// open tags
				Tag("hdrgm", item) + Tag("rdf", "Seq") +
				// add 3 numbers
				linum + linum + linum +
				// close tags
				Tag("rdf", "Seq", false) + Tag("hdrgm", item, false);

			vectorMatch = std::regex_search(text, match, std::regex{tripleRegex});
		}
		if (singleMatch && match.size() > 1)
		{
			// todo - error checking
			values.push_back(std::stod(match.str(1)));
			return true;
		}
		if (vectorMatch && match.size() == 7)
		{
			// todo - error checking
			values.push_back(std::stod(match.str(1)));
			values.push_back(std::stod(match.str(3)));
			values.push_back(std::stod(match.str(5)));
			return true;
		}
		if (!required)
		{
			values.push_back(defaultValue);
			return true;
		}
		return false;
	}

	static std::string Tag(const char* ns, const std::string& tag, bool open = true)
	{
		const std::string ws = R"xx([\n\t\r ]*)xx"; // whitespace
		return ws + (open ? "<" : "</") + ns + ":" + tag + ">";
	}
};

void ParseApp1(const vector<uint8_t>& dataIn, size_t offset, const string& filePrefix)
{
	// XMP? https://www.adobe.com/products/xmp.html,
	// https://stackoverflow.com/questions/23253281/reading-jpg-files-xmp-metadata
	// https://github.com/adobe/XMP-Toolkit-SDK/blob/main/docs/XMPSpecificationPart3.pdf

	vector<uint8_t> xmpData;

	uint16_t len = dataIn[offset] * 256 + dataIn[offset + 1];
	offset += 2;
	if (len >= 2) len -= 2;

	string header = "http://ns.adobe.com/xap/1.0/";
	for (auto c : header)
	{
		if (dataIn[offset++] != static_cast<uint8_t>(c))
			return; // not proper header
	}

	UltraHdr hdr;
	const string text(reinterpret_cast<const char*>(&dataIn[offset]), len - header.size());
	if (hdr.ParseXmp(text))
	{
		cout << "UltraHDR parameters:\n";
		hdr.Dump(cout, "   ");

		const auto outfilename = format("{}_hdrgm.txt", filePrefix);
		std::ofstream output(outfilename);
		hdr.Dump(output);
		output.close();
		cout << format("File written: HDR text to {}\n", outfilename);
	}
}

// splits a multipart file at the end of image markers
// useful for splitting multipart files
void SplitMultipartFile(const string& filename, const string& filePrefix)
{
	cout << format("Splitting {}\n", filename);
	// read entire file
	ifstream instream(filename, ios::in | ios::binary);
	const vector<uint8_t> data((istreambuf_iterator<char>(instream)), istreambuf_iterator<char>());
	instream.close();

	// walk it
	uint16_t chunk = 0; // last 2 bytes during scan
	size_t offset = 0;
	size_t start = 0; // start offset of current file
	int fileIndex = 1; // suffix of file
	while (offset < data.size())
	{
		size_t startOffset = offset;
		chunk = data[offset] * 256 + data[offset + 1];
		if ((chunk & 0xFF00) == 0xFF00 && (chunk >= 0xFFC0))
		{
			// process chunk
			offset += 2;
			size_t length = 0;
			if (chunk != 0xFFD8 && chunk != 0xFFD9) // not SOI or EOI
			{
				// must skip chunk of length
				length = data[offset] * 256 + data[offset + 1];
				offset += length;
			}
			if (chunk == 0xFFD9)
			{
				// end of image EOI, split it
				const auto outfilename = format("{}_split_{}.jpg", filePrefix, fileIndex++);
				std::ofstream output(outfilename, std::ios::binary);
				output.write(reinterpret_cast<const char*>(&data[start]), offset - start);
				output.close();
				start = offset; // next file
				cout << format("File written: {} to {}\n", (fileIndex == 2 ? "image" : "Gain map"), outfilename);
			}
			if (chunk == 0xFFE1) // APP1 chunk
			{
				// try to get xmp/ultra hdr info
				ParseApp1(data, startOffset + 2, filePrefix);
			}
			cout << format("Chunk {:4X} offset {:8X} length {}\n", chunk, startOffset, length);

			if (chunk == 0xFFDA) // must read till end
			{
				// read frame, - skip ahead till next chunk
				while (chunk != 0xFFD9) // overrun here crashes
				{
					chunk = static_cast<uint16_t>((chunk << 8) | data.at(offset++));
				}
				if (chunk == 0xFFD9)
					offset -= 2; // back off               
			}
		}
		else
		{
			cout << format("ERROR: Unknown chunk {:4X} at {:8X}, exiting....\n", chunk, offset);
			return;
		}
	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cerr << format("Call with a jpeg file to split UltraHDR on command line.\n");
		return -1;
	}
	const string filename(argv[1]);
	const auto filestem = fs::path(filename).stem().string(); // path/and/filename_stem
	SplitMultipartFile(filename, filestem);
	cout << format("Done. By Chris Lomont 2024\n");
	return 0;
}
