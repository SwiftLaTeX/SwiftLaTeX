#include "XeTeXHTML.h"
#include "XeTeXColor.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <stack>
#include <unordered_map>
using namespace std;
static stack<Color> _bgColorStack;
static stack<Color> _colorStack;
static unordered_map<unsigned int, unordered_map<unsigned int, unsigned int>>
    charglyph_map;
static string _svgBuffer;
unsigned int read_charglyph_map(unsigned int f, unsigned int glyph) {
  if (charglyph_map.count(f) == 0) {
    return 0;
  }

  if (charglyph_map[f].count(glyph) == 0) {
    return 0;
  }

  return charglyph_map[f][glyph];
}

void store_charglyph_map(unsigned int f, unsigned int glyph,
                         unsigned int character) {
  // printf("c:%d--->g:%d\n", character, glyph);
  if (read_charglyph_map(f, glyph) == 0) {
    // printf("c:%d--->g:%d\n", character, glyph);
    charglyph_map[f][glyph] = character;
  }
}

void clean_charglyph_map() { charglyph_map.clear(); }

static double read_double(istream &is) {
  is.clear();
  double v;
  is >> v;
  if (is.fail()) {
    printf("Failed to parse color value");
    v = 0;
  }
  return v;
}

/** Reads multiple double values from a given stream. The number of
 *  values read is determined by the size of the result vector.
 *  @param[in]  is stream to be read from
 *  @param[out] vec the resulting values */
static void read_doubles(istream &is, vector<double> &vec) {
  for (double &val : vec)
    val = read_double(is);
}

static Color readColor(const string &model, istream &is) {
  Color color;
  if (model == "rgb") {
    vector<double> rgb(3);
    read_doubles(is, rgb);
    color.setRGB(rgb[0], rgb[1], rgb[2]);
  } else if (model == "cmyk") {
    vector<double> cmyk(4);
    read_doubles(is, cmyk);
    color.setCMYK(cmyk[0], cmyk[1], cmyk[2], cmyk[3]);
  } else if (model == "hsb") {
    vector<double> hsb(3);
    read_doubles(is, hsb);
    color.setHSB(hsb[0], hsb[1], hsb[2]);
  } else if (model == "gray") {
    color.setGray(read_double(is));
  } else if (!color.setPSName(model, true)) {
    printf("Failed to parse color name");
    color.setRGB(0., 0., 0.);
  }

  return color;
}

static Color readColor(istream &is) {
  string model;
  is >> model;
  return readColor(model, is);
}

void processColorSpecial(const char *bytes) {
  string specialStr(bytes);
  istringstream is(specialStr);
  string ignore;
  is >> ignore;
  string cmd;
  is >> cmd;
  if (cmd == "push") {
    _colorStack.push(readColor(is));
  } else if (cmd == "pop") {
    if (!_colorStack.empty()) // color pop
      _colorStack.pop();
  } else {
    while (!_colorStack.empty())
      _colorStack.pop();
    _colorStack.push(readColor(cmd, is));
  }
}

void processBackgroundSpecial(const char *bytes) {
  string specialStr(bytes);
  istringstream is(specialStr);
  string ignore;
  is >> ignore;
  _bgColorStack.push(readColor(is));
}

void getBackgroundColor(int *r, int *g, int *b) {
  *r = 255;
  *g = 255;
  *b = 255;
  if (!_bgColorStack.empty()) {
    double r_d, g_d, b_d;
    _bgColorStack.top().getRGB(r_d, g_d, b_d);
    *r = int(255 * r_d);
    *g = int(255 * g_d);
    *b = int(255 * b_d);
  }
}

void getDocumentColor(int *r, int *g, int *b) {
  *r = 0;
  *g = 0;
  *b = 0;
  if (!_colorStack.empty()) {
    double r_d, g_d, b_d;
    _colorStack.top().getRGB(r_d, g_d, b_d);
    *r = int(255 * r_d);
    *g = int(255 * g_d);
    *b = int(255 * b_d);
  }
}

#define PI 3.14159265
int processPDFSpecial(const char *bytes, double x, double y, char *buf,
                    unsigned int bufsize) {
  bool has_scale = 0, has_xscale = 0, has_yscale = 0, has_rotate = 0,
      has_matrix = 0, has_bbox = 0;
  double xscale = 1.0, yscale = 1.0, rotate = 0.0;
  double width = 0, height = 0;
  double bboxllx = 0, bboxlly = 0, bboxurx = 0, bboxury = 0;
  double matrix0 = 0, matrix1 = 0, matrix2 = 0, matrix3 = 0, matrix4 = 0,
         matrix5 = 0;
  double clip = 0;
  double page = 0;
  int error = 0;
  string input(bytes);
  istringstream is(input);
  string ignore, cmd, filename;
  is >> ignore;
  // cout << input << endl;
  while (is >> cmd) {
    // cout << "cmd: " << cmd << endl;
    if (cmd == "width") {
      string withUnit;
      if (!(is >> withUnit)) {
        error = 1;
        break;
      }
      width = std::stoi(withUnit);
      width *= 72.0 / 72.27;
    } else if (cmd == "height") {
      string withUnit;
      if (!(is >> withUnit)) {
        error = 2;
        break;
      }
      height = std::stoi(withUnit);
      height *= 72.0 / 72.27;
    } else if (cmd == "scale") {
      if (!(is >> xscale)) {
        error = 4;
        break;
      }
      yscale = xscale;
      has_scale = 1;
    } else if (cmd == "xscale") {
      if (!(is >> xscale)) {
        error = 5;
        break;
      }
      has_xscale = 1;

    } else if (cmd == "yscale") {
      if (!(is >> yscale)) {
        error = 6;
        break;
      }
      has_yscale = 1;
    } else if (cmd == "rotate") {
      if (!(is >> rotate)) {
        error = 7;
        break;
      }
      rotate = (360 - rotate) * PI / 180.0;
      has_rotate = 1;
    } else if (cmd == "bbox") {
      if (!(is >> bboxllx)) {
        error = 8;
        break;
      }
      if (!(is >> bboxlly)) {
        error = 9;
        break;
      }
      if (!(is >> bboxurx)) {
        error = 10;
        break;
      }
      if (!(is >> bboxury)) {
        error = 11;
        break;
      }
      has_bbox = 1;

    } else if (cmd == "matrix") {
      if (!(is >> matrix0)) {
        error = 12;
        break;
      }
      if (!(is >> matrix1)) {
        error = 13;
        break;
      }
      if (!(is >> matrix2)) {
        error = 14;
        break;
      }
      if (!(is >> matrix3)) {
        error = 15;
        break;
      }
      if (!(is >> matrix4)) {
        error = 16;
        break;
      }
      if (!(is >> matrix5)) {
        error = 17;
        break;
      }
      has_matrix = 1;
    } else if (cmd == "clip") {
      if (!(is >> clip)) {
        error = 18;
        break;
      }
    } else if (cmd == "hide") {
      // Do nothing
    } else if (cmd == "page") {
      if (!(is >> page)) {
        error = 19;
        break;
      }
    } else if (cmd.rfind("(") == 0) {
      filename = cmd;
      filename.pop_back();
      filename.erase(0, 1);
    } else {
      cout << "Unexpected command: " << cmd << endl;
      error = 20;
      break;
    }
  }

  if (!error) {
    /* Check consistency */
    if (has_xscale && width != 0) {
      cout << "Can't supply both width and xscale. Ignore xscale." << endl;
      xscale = 1.0;
    } else if (has_yscale && height != 0) {
      cout << "Can't supply both height/depth and yscale. Ignore yscale."
           << endl;
      yscale = 1.0;
    } else if (has_scale && (has_xscale || has_yscale)) {
      cout << "Can't supply overall scale along with axis scales." << endl;
      error = -1;
    } else if (has_matrix &&
               (has_scale || has_xscale || has_yscale || has_rotate)) {
      cout << "Can't supply transform matrix along with scales or rotate. "
              "Ignore scales and rotate."
           << endl;
    }
  }

  if (error) {
    cout << "Error detected " << error << endl;
    return -1;
  }

  std::stringstream os;

  os.precision(3);

  os << fixed;
  

  if (x != 0 || y != 0) {

    if (has_bbox) {
      double bbox_height = bboxury - bboxlly;
      double bbox_width = bboxurx - bboxllx;
      if (width == 0 && height == 0) {
      	// x -= bbox_width;
        y -= bbox_height;
        os << "width='" << bbox_width << "' ";
        os << "height='" << bbox_height << "' ";
      } else if (width > 0 && height > 0) {
        y -= height;
        // x -= width;
        os << "width='" << width << "' ";
        os << "height='" << height << "' ";
      } else if (width > 0 && height == 0) {
        double ra = bbox_width / width;
        double ra_height = bbox_height / ra;
        y -= ra_height;
        // x -= width;
        os << "width='" << width << "' ";
        os << "height='" << ra_height << "' ";

      } else if (width == 0 && height > 0) {
        double ra = bbox_height / height;
        double ra_width = bbox_width / ra;
        y -= height;
        // x -= ra_width;
        os << "width='" << ra_width << "' ";
        os << "height='" << height << "' ";
      }
      os << "x='" << x << "' ";
      os << "y='" << y << "' ";
    }
  }

  if (has_bbox && clip > 0) {
    os << "viewBox='" << bboxllx << " " << bboxlly << " " << bboxurx << " "
       << bboxury << "' ";
  }

  if (has_matrix || has_yscale || has_xscale || has_scale || has_rotate) {
    if (!has_matrix) {
      double c, s;
      c = cos(rotate);
      s = sin(rotate);
      matrix0 = xscale * c;
      matrix1 = xscale * s;
      matrix2 = -yscale * s;
      matrix3 = yscale * c;
      matrix4 = 0;
      matrix5 = 0;
    }
    os << "transform='matrix(" << matrix0 << "," << matrix1 << "," << matrix2
     << "," << matrix3 << "," << matrix4 << "," << matrix5 << ")' ";
  }
  
  if (filename.size()) {
    os << "url='" << filename << "' ";
  }

  string finalanswer = os.str();
  strncpy(buf, finalanswer.c_str(), bufsize);

  return 1;
}

// static unordered_map<unsigned int, unsigned long long> meta_map;
// unsigned long long readmeta(unsigned int addr) {
// 	auto iter = meta_map.find(addr);
// 	if (iter != meta_map.end()) {
// 		return iter->second;
// 	}
// 	return 0;
// }
// void storemeta(unsigned int addr, unsigned long long meta) {
// 	if (meta != 0) {
// 		meta_map[addr] = meta;
// 		printf("s: %ld\n", meta);
// 	}
// }
// void copymeta(unsigned int dst, unsigned int src) {
// 	unsigned long long meta = readmeta(src);
// 	if (meta != 0) {
// 		meta_map[dst] = meta;
// 		// printf("c: %d\n", meta_map.size());
// 	}
// }
// void clearmeta(unsigned int addr) {
// 	meta_map.erase(addr);
// }