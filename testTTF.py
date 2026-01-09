# with open('build/IconfontViewer-Debug-GNU-Windows-AMD64/iconfont.ttf', 'rb') as f:
#     # 找到 'post' 表偏移（需解析目录）
#     # 或直接搜索 b'post'
#     data = f.read()
#     idx = data.find(b'post')
#     if idx != -1:
#         print("post at:", hex(idx))
#         # format is 4 bytes after 'post' header (which is 12 bytes from tag)
#         fmt = int.from_bytes(data[idx+4:idx+8], 'big')
#         print("post format:", fmt, "=>", hex(fmt))
#         # 0x00020000 = format 2.0, 0x00030000 = format 3.0


# from fontTools.ttLib import TTFont
# fontPath = "build/IconfontViewer-Debug-GNU-Windows-AMD64/iconfont.ttf"
# fontObj = TTFont(fontPath)
# nameTable = fontObj["name"]
# postTable = fontObj["post"]
# cmapTable = fontObj["cmap"]
# headTable = fontObj["head"]
#
# print(f"name table [{len(nameTable.names)}]: ", nameTable.names)
# print(f"post table [{len(postTable.extraNames)}]: ", postTable.extraNames)
# print(f"cmap table [{len(cmapTable.tables)}]: ", cmapTable.tables)
# print(f"cmap table version: ", cmapTable.tableVersion)
#
# count = 0
# for name in nameTable.names:
#     print(f"name table{count}: ",name.nameID, name.platformID, name.toUnicode(), name)
#     count += 1
#
# for table in cmapTable.tables:
#     print("------------------------------")
#     print(f"cmap table format: ", table.format)
#     print(f"cmap table platformID: ", table.platformID)
#     print(f"cmap table ttFont: ", table.ttFont)
#     print(f"cmap table platEncID: ", table.platEncID)
#     print(f"cmap table language: ", table.language)

    # print(f"cmap table {table.platformEncodingID}: ", table.platformEncodingID)
    # print(f"cmap table {table.languageID}: ", table.languageID)

from fontTools.ttLib import TTFont

# 加载 TTF 文件
font = TTFont('build/IconfontViewer-Debug-GNU-Windows-AMD64/iconfont.ttf')  # 替换为你的 TTF 文件路径

# print(font['post'].format)  # 如果报错，说明无 post 表
# print(font.getGlyphName(1)) # 尝试获取第一个 glyph 名称
# 获取 cmap 表（字符编码到 glyph 名称的映射）

# if 'post' in font:
#
#     post_table = font['post']
#     print("post format:", post_table.formatType)
#
#     # 尝试列出前几个 glyph 名称
#     for i in range(min(5, len(font.getGlyphOrder()))):
#         print(f"glyph {i}: {font.getGlyphName(i)}")
# else:
#     print("No 'post' table!")


cmap = font.getBestCmap()  # 返回 dict: {unicode_int: glyph_name}
name = font.getGlyphNames()
keys = font.keys()
print(f"Name[{len(name)}]: {name}")
print(f"Key[{len(keys)}]: {keys}")
for name in font['name'].names:
    print(f"Name: {name}")
# 打印所有图标
count = 0
for unicode_int, glyph_name in cmap.items():
    count += 1
    unicode_hex = f"U+{unicode_int:04X}"
    print(f"[{count}]Unicode: {unicode_hex} ({chr(unicode_int)}) -> Glyph Name: {glyph_name}")