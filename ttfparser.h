//
// Created by clan4 on 2026/1/7.
//

#ifndef ICONFONTVIEWER_TTFPARSER_H
#define ICONFONTVIEWER_TTFPARSER_H

#include <QFont>
#include <QList>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace cstk_ttf
{
    struct TtfData
    {
        quint16 index;
        quint32 unicode;
        QString name;
    };

    struct SvgPathResult
    {
        QString pathData;
        double xMin, yMin, width, height;
    };

    using TtfDataList = QList<TtfData>;

    class TtfParser
    {
    public:
        TtfParser(const QString &ttfFile = "");
        ~TtfParser();

        TtfParser(const TtfParser &other) = delete;
        TtfParser &operator=(const TtfParser &other) = delete;

        TtfParser(TtfParser &&other) noexcept;
        TtfParser &operator=(TtfParser &&other) noexcept;

        bool isValid() const;
        QFont getFont() const;
        TtfDataList getDataList() const;
        qsizetype getDataCount() const;
        TtfData getData(const quint32 unicode) const;

        QString getErrorMessage() const;

        QString getSvgData(const quint32 unicode, const QString &colorName);

    private:
        QString _ttfFile;
        int _fontId;
        QString _fontName;
        QFont _font;
        TtfDataList _data;
        bool _isValid;
        QString _errorMessage;

        FT_Library _library;
        FT_Face _face;

        bool _isLibraryInit;
        bool _isFaceInit;

        bool _parse();
        static SvgPathResult _processOutline(const FT_Outline* outline, quint16 upem);
        static double _ftXToDouble(FT_Pos x);
        static double _ftYToDouble(FT_Pos y, quint16 upem);
    };
}
#endif //ICONFONTVIEWER_TTFPARSER_H