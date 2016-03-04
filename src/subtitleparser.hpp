#include "subtitles.hpp"
#include <string>
namespace SubtitleParser {
    Subtitles & const fromTxt(std::string & filename);
    Subtitles & const fromLyr(std::string & lyr_filename, std::string & frame_filename)
}
