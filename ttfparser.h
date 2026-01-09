//
// Created by clan4 on 2026/1/7.
//

#ifndef ICONFONTVIEWER_TTFPARSER_H
#define ICONFONTVIEWER_TTFPARSER_H

#include <QFont>
#include <QMap>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace cstk_ttf
{
    using TtfData = QMap<quint32, QString>;
    class TtfParser
    {
    public:
        TtfParser(const QString &ttfFile);
        ~TtfParser() = default;

        bool isValid() const;
        QFont getFont() const;
        TtfData getData() const;

        QString getErrorMessage() const;
    private:
        QString _ttfFile;
        int _fontId;
        QString _fontName;
        QFont _font;
        TtfData _data;
        bool _isValid;
        QString _errorMessage;

        bool _parse();
    };
}
#endif //ICONFONTVIEWER_TTFPARSER_H