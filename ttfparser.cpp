//
// Created by clan4 on 2026/1/7.
//

#include "ttfparser.h"
#include <QFileInfo>
#include <QFontDatabase>

namespace cstk_ttf
{
    TtfParser::TtfParser(const QString& ttfFile) : _ttfFile(ttfFile),
                                                   _fontId(-1),
                                                   _fontName(""),
                                                   _font(QFont()),
                                                   _data({}),
                                                   _isValid(false),
                                                   _errorMessage("No Error"),
                                                   _isLibraryInit(false),
                                                   _isFaceInit(false)
    {
        if (!_ttfFile.isEmpty())
        {
            _isValid = _parse();
        }
    }

    TtfParser::~TtfParser()
    {
        if (_isFaceInit)
        {
            FT_Done_Face(_face);
        }
        if (_isLibraryInit)
        {
            FT_Done_FreeType(_library);
        }
    }

    TtfParser::TtfParser(TtfParser&& other) noexcept
        : _ttfFile(std::move(other._ttfFile)),
          _fontId(std::exchange(other._fontId, 0)),
          _fontName(std::move(other._fontName)),
          _font(std::move(other._font)),
          _data(std::move(other._data)),
          _isValid(std::exchange(other._isValid, false)),
          _errorMessage(std::move(other._errorMessage))
    {
        _library = std::exchange(other._library, nullptr);
        _face = std::exchange(other._face, nullptr);

        _isLibraryInit = std::exchange(other._isLibraryInit, false);
        _isFaceInit = std::exchange(other._isFaceInit, false);
    }

    TtfParser& TtfParser::operator=(TtfParser&& other) noexcept
    {
        if (this != &other)
        {
            _ttfFile = std::move(other._ttfFile);
            _fontId = std::exchange(other._fontId, 0);
            _fontName = std::move(other._fontName);
            _font = std::move(other._font);
            _data = std::move(other._data);
            _isValid = std::exchange(other._isValid, false);
            _errorMessage = std::move(other._errorMessage);

            _library = std::exchange(other._library, nullptr);
            _face = std::exchange(other._face, nullptr);

            _isLibraryInit = std::exchange(other._isLibraryInit, false);
            _isFaceInit = std::exchange(other._isFaceInit, false);
        }
        return *this;
    }

    bool TtfParser::isValid() const
    {
        return _isValid;
    }

    QFont TtfParser::getFont() const
    {
        return _font;
    }

    TtfDataList TtfParser::getDataList() const
    {
        return _data;
    }

    qsizetype TtfParser::getDataCount() const
    {
        return _data.count();
    }

    TtfData TtfParser::getData(const quint32 unicode) const
    {
        for (const auto& data : _data)
        {
            if (data.unicode == unicode)
            {
                return data;
            }
        }
        return {};
    }

    QString TtfParser::getErrorMessage() const
    {
        return _errorMessage;
    }

    QString TtfParser::getSvgData(const quint32 unicode, const QString& colorName)
    {
        if (!isValid())
        {
            return "";
        }

        FT_Set_Pixel_Sizes(_face, 0, 512);
        FT_UInt glyphIndex = getData(unicode).index;
        if (glyphIndex == 0)
        {
            _errorMessage = QString("Glyph not found for codepoint ")
                + QString("U+%1").arg(unicode, 4, 16, QLatin1Char('0')).toUpper();
            return "";
        }
        if (FT_Load_Glyph(_face, glyphIndex, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_IGNORE_TRANSFORM))
        {
            _errorMessage = "Failed to load glyph";
            return "";
        }
        if (_face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
        {
            _errorMessage = "Glyph is not an outline (maybe bitmap?)";
            return "";
        }

        long upem = _face->units_per_EM;
        if (upem == 0) { upem = 1024; }

        SvgPathResult result = _processOutline(&_face->glyph->outline, upem);
        if (result.pathData.isEmpty() || result.width <= 0 || result.height <= 0)
        {
            _errorMessage = "No outline data";
            return "";
        }

        QString svgData = "<svg xmlns=\"http://www.w3.org/2000/svg\" ";
        svgData += QString("viewBox=\"%1 %2 %3 %4\">\n").arg(result.xMin).arg(result.yMin).arg(result.width).arg(
            result.height);
        svgData += QString("  <path d=\"%1\" fill=\"%2\"/>\n").arg(result.pathData).arg(colorName);
        svgData += "</svg>\n";

        return svgData;
    }

    bool TtfParser::_parse()
    {
        const QFileInfo ttfInfo(_ttfFile);

        if (!ttfInfo.exists() || !ttfInfo.isFile() || !ttfInfo.isReadable())
        {
            _errorMessage = "File not exists or not readable";
            return false;
        }

        if (ttfInfo.suffix().toLower() != "ttf")
        {
            _errorMessage = "File is not a ttf file";
            return false;
        }

        if (FT_Init_FreeType(&_library))
        {
            _errorMessage = "Failed to initialize FreeType.";
            return false;
        }

        _isLibraryInit = true;

        QByteArray pathBytes = _ttfFile.toLocal8Bit();
        if (FT_New_Face(_library, pathBytes.constData(), 0, &_face))
        {
            _errorMessage = "Failed to load font file.";
            return false;
        }

        _isFaceInit = true;

        if (!FT_HAS_GLYPH_NAMES(_face))
        {
            _errorMessage = "Font does not support glyph names (post table missing or disabled).";
            return false;
        }

        FT_UInt gindex;
        FT_ULong charcode = FT_Get_First_Char(_face, &gindex);
        while (gindex != 0)
        {
            char name_buf[256] = {0};
            if (FT_Get_Glyph_Name(_face, gindex, name_buf, sizeof(name_buf)) == 0)
            {
                QString name(name_buf);
                _data.emplace_back(gindex, charcode, name);
            }
            charcode = FT_Get_Next_Char(_face, charcode, &gindex);
        }

        _fontId = QFontDatabase::addApplicationFont(_ttfFile);
        if (_fontId == -1)
        {
            _errorMessage = "Failed to add font to QFontDatabase.";
            return false;
        }

        _fontName = QFontDatabase::applicationFontFamilies(_fontId).first();
        _font = QFont(_fontName);
        return true;
    }

    SvgPathResult TtfParser::_processOutline(const FT_Outline* outline, quint16 upem)
    {
        SvgPathResult res;
        res.xMin = res.yMin = std::numeric_limits<double>::max();
        double xMax = -std::numeric_limits<double>::max();
        double yMax = -std::numeric_limits<double>::max();

        std::ostringstream path;
        const FT_Vector* points = outline->points;
        unsigned char* tags = outline->tags;
        unsigned short* contours = outline->contours;
        int n_points = outline->n_points;
        int n_contours = outline->n_contours;

        if (n_points == 0) return res;

        std::vector<bool> is_endpoint(n_points, false);
        for (int i = 0; i < n_contours; ++i)
        {
            is_endpoint[contours[i]] = true;
        }

        auto updateBounds = [&](double x, double y)
        {
            if (x < res.xMin) res.xMin = x;
            if (x > xMax) xMax = x;
            if (y < res.yMin) res.yMin = y;
            if (y > yMax) yMax = y;
        };

        auto toSvgPoint = [&](FT_Pos x, FT_Pos y) -> std::pair<double, double>
        {
            double x_svg = static_cast<double>(x) / 64.0;
            double y_svg = static_cast<double>(upem) - static_cast<double>(y) / 64.0;
            return {x_svg, y_svg};
        };

        int start = 0;
        for (int c = 0; c < n_contours; ++c)
        {
            int end = contours[c];
            int first_on = -1;
            for (int i = start; i <= end; ++i)
            {
                if (tags[i] & FT_CURVE_TAG_ON)
                {
                    first_on = i;
                    break;
                }
            }
            if (first_on == -1)
            {
                if (end - start >= 1)
                {
                    double x = (static_cast<double>(points[start].x) + points[start + 1].x) / 2.0 / 64.0;
                    double y = static_cast<double>(upem) - (static_cast<double>(points[start].y) + points[start + 1].y)
                        / 2.0 / 64.0;
                    path << "M " << x << " " << y << " ";
                    updateBounds(x, y);
                    first_on = start; // dummy
                }
                else
                {
                    start = end + 1;
                    continue;
                }
            }
            else
            {
                auto [x0, y0] = toSvgPoint(points[first_on].x, points[first_on].y);
                path << "M " << x0 << " " << y0 << " ";
                updateBounds(x0, y0);
            }

            int i = first_on;
            bool started = (first_on != -1);
            do
            {
                if (!started)
                {
                    i = start;
                    started = true;
                }
                else
                {
                    i = (i == end) ? start : i + 1;
                }

                if (tags[i] & FT_CURVE_TAG_ON)
                {
                    auto [x, y] = toSvgPoint(points[i].x, points[i].y);
                    path << "L " << x << " " << y << " ";
                    updateBounds(x, y);
                }
                else
                {
                    int next = (i == end) ? start : i + 1;
                    int next_next = (next == end) ? start : next + 1;

                    auto [cx, cy] = toSvgPoint(points[i].x, points[i].y);

                    if (tags[next] & FT_CURVE_TAG_ON)
                    {
                        auto [ax, ay] = toSvgPoint(points[next].x, points[next].y);
                        path << "Q " << cx << " " << cy << " " << ax << " " << ay << " ";
                        updateBounds(ax, ay);
                        i = next;
                    }
                    else
                    {
                        auto [nx, ny] = toSvgPoint(points[next].x, points[next].y);
                        double ax = (cx + nx) / 2.0;
                        double ay = (cy + ny) / 2.0;
                        path << "Q " << cx << " " << cy << " " << ax << " " << ay << " ";
                        updateBounds(ax, ay);
                    }
                }
            }
            while (i != first_on && !(i == end && (tags[i] & FT_CURVE_TAG_ON)));

            path << "Z ";
            start = end + 1;
        }

        res.width = xMax - res.xMin;
        res.height = yMax - res.yMin;
        res.pathData = path.str().data();
        return res;
    }

    double TtfParser::_ftXToDouble(FT_Pos x)
    {
        return static_cast<double>(x) / 64.0;
    }

    double TtfParser::_ftYToDouble(FT_Pos y, quint16 upem)
    {
        return static_cast<double>(upem) - static_cast<double>(y) / 64.0;
    }
}
