//
// Created by clan4 on 2026/1/7.
//

#include "ttfparser.h"
#include <QFileInfo>
#include <QFontDatabase>

namespace cstk_ttf
{
    TtfParser::TtfParser(const QString &ttfFile) : _ttfFile(ttfFile),
                                                   _fontId(-1),
                                                   _fontName(""),
                                                   _font(QFont()),
                                                   _data({}),
                                                   _isValid(false),
                                                   _errorMessage("No Error")
    {
        _isValid = _parse();
    }

    bool TtfParser::isValid() const
    {
        return _isValid;
    }

    QFont TtfParser::getFont() const
    {
        return _font;
    }

    TtfData TtfParser::getData() const
    {
        return _data;
    }

    QString TtfParser::getErrorMessage() const
    {
        return _errorMessage;
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

        FT_Library library;
        FT_Face face;

        if (FT_Init_FreeType(&library))
        {
            _errorMessage = "Failed to initialize FreeType.";
            return false;
        }

        QByteArray pathBytes = _ttfFile.toLocal8Bit();
        if (FT_New_Face(library, pathBytes.constData(), 0, &face))
        {
            FT_Done_FreeType(library);
            _errorMessage = "Failed to load font file.";
            return false;
        }

        if (!FT_HAS_GLYPH_NAMES(face))
        {
            _errorMessage = "Font does not support glyph names (post table missing or disabled).";
            return false;
        }

        FT_UInt gindex;
        FT_ULong charcode = FT_Get_First_Char(face, &gindex);
        while (gindex != 0)
        {
            char name_buf[256] = {0};
            if (FT_Get_Glyph_Name(face, gindex, name_buf, sizeof(name_buf)) == 0)
            {
                QString name(name_buf);
                _data.insert(charcode, name);
            }
            charcode = FT_Get_Next_Char(face, charcode, &gindex);
        }

        FT_Done_Face(face);
        FT_Done_FreeType(library);

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
}
