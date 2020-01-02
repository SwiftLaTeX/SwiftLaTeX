import math
import fontforge


def map_to_private(code):
    private_mapping = code + 0xE000
    if private_mapping > 0xF8FF:
        private_mapping = private_mapping - 0xF8FF + 0xF0000
        if private_mapping > 0xFFFFD:
            private_mapping = private_mapping - 0xFFFFD + 0x100000
            if private_mapping > 0x10FFFD:
                print("I tried my best~, but no available mapping found")
    #print("mapping %d to %d" %(code, private_mapping))
    return private_mapping


def illegal(c):
    return (c <= 0x20) or (c >= 0x7F and c <= 0xA0) or (c == 0xAD) \
            or (c == 0x061C) or (c == 0x1361) \
            or (c >= 0x200B and c <= 0x200F) or (c == 0x2028) or (c == 0x2029) \
            or (c >= 0x202A and c <= 0x202E) or (c >= 0x2066 and c <= 0x2069) \
            or (c >= 0xD800 and c <= 0xDFFF) or (c == 0xFEFF) or (c == 0xFFFC) \
            or (c == 0xFFFE) or (c == 0xFFFF)

fonturls = {}
fhandle = open("fonts.txt")

for line in fhandle:
    line = line.replace("\n", "")
    parts = line.split(" ")
    fontname = parts[0]
    fonturl = parts[1]
    fonturls[fontname] = fonturl

def handlefont(key):
    print("Open font " + key)
    fontins = fontforge.open(fonturls[key])
    alreadyallocate = {}
    error_detected = False
    for (idontuse, g) in enumerate(fontins.glyphs()):
        g.altuni = None

        index = g.encoding
        if illegal(index):
            index = map_to_private(index)
        g.unicode = index

        if index not in alreadyallocate:
            alreadyallocate[index] = g.glyphname
        else:
            print("this index %d is allocated to %s, not able to allocate for %s" % (index, alreadyallocate[index], g.glyphname))
            error_detected = True
        g.glyphname = ""

    fontins.reencode("UnicodeFull")
    fontins.generate("output/%s.woff" % key)
    fontins.close()
    print("Close font %s %s" % (key, error_detected))
    return error_detected


count = 0
for key in fonturls:
    count += 1
    handlefont(key)



