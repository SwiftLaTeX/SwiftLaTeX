#ifndef COLOR_HPP
#define COLOR_HPP

#include <string>
#include <valarray>
#include <vector>

#ifdef TRANSPARENT
#undef TRANSPARENT
#endif

class Color {
	public:
		static bool SUPPRESS_COLOR_NAMES;
		static const Color BLACK;
		static const Color WHITE;
		static const Color TRANSPARENT;

		enum class ColorSpace {GRAY, RGB, CMYK, LAB};

	public:
		Color () noexcept =default;
		explicit Color (uint32_t rgb) noexcept : _rgb(rgb)         {}
		Color (uint8_t r, uint8_t g, uint8_t b) noexcept  {setRGB(r,g,b);}
		Color (double r, double g, double b) noexcept     {setRGB(r,g,b);}
		explicit Color (const std::valarray<double> &rgb) noexcept {setRGB(rgb);}
		explicit Color (const std::string &name);
		explicit operator uint32_t () const            {return _rgb;}
		bool operator == (const Color &c) const        {return _rgb == c._rgb;}
		bool operator != (const Color &c) const        {return _rgb != c._rgb;}
		bool operator < (const Color &c) const         {return _rgb < c._rgb;}
		Color operator *= (double c);
		Color operator * (double c) const              {return Color(*this) *= c;}
		void setRGB (uint8_t r, uint8_t g, uint8_t b)  {_rgb = (r << 16) | (g << 8) | b;}
		void setRGB (double r, double g, double b);
		void setRGB (const std::valarray<double> &rgb) {setRGB(rgb[0], rgb[1], rgb[2]);}
		bool setPSName (std::string name, bool case_sensitive=true);
		void setGray (uint8_t g)  {setRGB(g,g,g);}
		void setGray (double g) {setRGB(g,g,g);}
		void setGray (const std::valarray<double> &gray) {setRGB(gray[0], gray[0], gray[0]);}
		void setHSB (double h, double s, double b);
		void setCMYK (double c, double m, double y, double k);
		void setCMYK (const std::valarray<double> &cmyk);
		void setXYZ (double x, double y, double z);
		void setXYZ (const std::valarray<double> &xyz);
		void setLab (double l, double a, double b);
		void setLab (const std::valarray<double> &lab);
		double getGray () const;
		void getGray (std::valarray<double> &gray) const;
		void getRGB (double &r, double &g, double &b) const;
		void getRGB (std::valarray<double> &rgb) const;
		void getCMYK (double &c, double &m, double &y, double &k) const;
		void getCMYK (std::valarray<double> &cmyk) const;
		void getXYZ (double &x, double &y, double &z) const;
		void getLab (double &l, double &a, double &b) const;
		void getLab (std::valarray<double> &lab) const;
		double deltaE (const Color &c) const;
		std::string rgbString () const;
		std::string svgColorString (bool rgbonly) const;
		std::string svgColorString () const {return svgColorString(SUPPRESS_COLOR_NAMES);}
		static void CMYK2RGB (const std::valarray<double> &cmyk, std::valarray<double> &rgb);
		static void RGB2CMYK (const std::valarray<double> &rgb, std::valarray<double> &cmyk);
		static void HSB2RGB (const std::valarray<double> &hsb, std::valarray<double> &rgb);
		static void RGB2XYZ (std::valarray<double> rgb, std::valarray<double> &xyz);
		static void XYZ2RGB (const std::valarray<double> &xyz, std::valarray<double> &rgb);
		static void RGB2Lab (const std::valarray<double> &rgb, std::valarray<double> &lab);
		static void Lab2XYZ (const std::valarray<double> &lab, std::valarray<double> &xyz);
		static int numComponents (ColorSpace colorSpace);

	private:
		uint32_t _rgb=0;
};

#endif