// PyDvi - A Python Library to Process DVI Stream
// Copyright (C) 2014 Fabrice Salvaire
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////////

import * as Tfm from "./tfm";
import * as fontdata from "./fonts.json";

const NO_TAG = 0;
const LIG_TAG = 1;
const LIST_TAG = 2;
const EXT_TAG = 3;

const KERN_OPCODE = 128;

// Defines the tables present in a TFM file.
const tables = {header: 0,
                character_info: 1,
                width: 2,
                height: 3,
                depth: 4,
                italic_correction: 5,
                lig_kern: 6,
                kern: 7,
                extensible_character: 8,
                font_parameter: 9
               };

/*** Compute the pointer to the word element index *index* from the base *base*.  A word
     element has a size of 32-bit.

     ``prt = base + 4*index``
***/
function word_ptr(base, index) {
  return base + 4*index;
}

class TFMParser {
  position : number;
  stream : Buffer;
  length : number;
  table_lengths : number[];
  table_pointers : number[];
  entire_file_length : number;
  smallest_character_code : number;
  largest_character_code : number;  
  number_of_chars : number;  
  tfm : Tfm.Tfm;
  
  constructor(buffer) {
    this.position = 0;
    this.stream = buffer;
    
    this.read_lengths();
    this.read_header();
    this.read_font_parameters();
    this.read_lig_kern_programs();
    this.read_characters();
  }

  seek(position) {
    this.position = position;
  }

  read_unsigned_byte1(p?) {
    if (p) this.position = p;
    var result = this.stream.readUInt8(this.position);
    this.position = this.position + 1;
    return result;
  }

  
  read_unsigned_byte2(p?) {
    if (p) this.position = p;
    var result = this.stream.readUInt16BE(this.position);
    this.position = this.position + 2;
    return result;
  }

  read_unsigned_byte4(p?) {
    if (p) this.position = p;
    var result = this.stream.readUInt32BE(this.position);
    this.position = this.position + 4;
    return result;
  }

  read_four_byte_numbers_in_table(table, index) {
        /*** Return the four numbers in table *table* at index *index*. 
         ***/
    this.seek(this.position_in_table(table, index));
    return [ this.read_unsigned_byte1(),
             this.read_unsigned_byte1(),
             this.read_unsigned_byte1(),
             this.read_unsigned_byte1() ];
  }

  read_extensible_recipe(index) {
        /*** Return the extensible recipe, four numbers, at index *index*.

        Extensible characters are specified by an extensible recipe, which consists of four bytes
        called top, mid, bot, and rep (in this order). These bytes are the character codes of
        individual pieces used to build up a large symbol. If top, mid, or bot are zero, they are
        not present in the built-up result. For example, an extensible vertical line is like an
        extensible bracket, except that the top and bottom pieces are missing.
        ***/ 
    return this.read_four_byte_numbers_in_table(tables.extensible_character, index);
  }
  
  // BADBAD
  read_fix_word(p?) {
    if (p) this.position = p;
    var result = this.stream.readUInt32BE(this.position);
    this.position = this.position + 4;
    return result;
  }

  read_fix_word_in_table(table, position) {
    return this.read_fix_word(this.position_in_table(table, position));
  }

  // BADBAD
  read_bcpl(position?) {
    if (position) this.position = position;
    var length = this.read_unsigned_byte1();
    var result = this.stream.slice( this.position, this.position + length ).toString('ascii');
    this.position += length;
    return result;
  }
  
  seek_to_table(table, position?) {
    if (position)
      this.seek( this.position_in_table( table, position ) );
    else
      this.seek(this.table_pointers[table]);
  }
  
  position_in_table(table, index) {
    return word_ptr(this.table_pointers[table], index);
  }

    /* The fist 24 bytes (6 words) of a TFM file contain twelve 16-bit
       integers that give the lengths of the various subsequent
       portions of the file. These twelve integers are, in order:
         
       * lf = length of the entire file, in words;
       * lh = length of the header data, in words;
       * bc = smallest character code in the font;
       * ec = largest character code in the font;
       * nw = number of words in the width table;
       * nh = number of words in the height table;
       * nd = number of words in the depth table;
       * ni = number of words in the italic correction table;
       * nl = number of words in the lig/kern table;
       * nk = number of words in the kern table;
       * ne = number of words in the extensible character table;
       * np = number of font parameter words.
       
      They are all nonnegative and less than 2**15. We must have
      ``bc - 1 <= ec <= 255``, ``ne <= 256``, and
         
      ``lf = 6 + lh + (ec - bc + 1) + nw + nh + nd + ni + nl + nk + ne + np``.

      Note that a font may contain as many as 256 characters (if ``bc = 0`` and ``ec = 255``), and
      as few as 0 characters (if ``bc = ec + 1``).
      
      The rest of the TFM file may be regarded as a sequence of ten data arrays having the
      informal specification:

      ========== ===================== ====================
      header     array [0  ... lh - 1] of stuff
      char info  array [bc ... ec    ] of char info word
      width      array [0  ... nw - 1] of fix word
      height     array [0  ... nh - 1] of fix word
      depth      array [0  ... nd - 1] of fix word
      italic     array [0  ... ni - 1] of fix word
      lig kern   array [0  ... nl - 1] of lig kern command
      kern       array [0  ... nk - 1] of fix word
      exten      array [0  ... ne - 1] of extensible recipe
      param      array [1  ... np    ] of fix word
      ========== ===================== ====================
    ***/
  read_lengths() {
    //////////////////////
    // Read and set table lengths
    this.table_lengths = [];

    this.seek(0);
    
    this.entire_file_length = this.read_unsigned_byte2();
    var header_length = this.read_unsigned_byte2();
    this.smallest_character_code = this.read_unsigned_byte2();
    this.largest_character_code = this.read_unsigned_byte2();

    var header_data_length_min = 18;
    this.table_lengths[tables.header] = Math.max(header_data_length_min, header_length);
    
    this.number_of_chars = this.largest_character_code - this.smallest_character_code + 1;
    this.table_lengths[tables.character_info] = this.number_of_chars;

    // read the last lengths
    for( var i = tables.width; i <= tables.font_parameter; i++ ) {
      this.table_lengths[i] = this.read_unsigned_byte2();
    }



    //////////////////////
    // Compute table pointers
    this.table_pointers = [];

    // The header starts at 24 bytes
    this.table_pointers[tables.header] = 24;

    for( var table = tables.header; table < tables.font_parameter; table++ ) {
      this.table_pointers[table+1] = this.position_in_table(table, this.table_lengths[table]);
    }
    
    //////////////////////
    // Sanity check
    var length = this.position_in_table(tables.font_parameter, this.table_lengths[tables.font_parameter]);
    if (length != word_ptr(0, this.entire_file_length)) {
      throw Error('Bad TFM file');
    }

    return;
  }

  /*** The first data array is a block of header information, which contains general facts
       about the font.  The header must contain at least two words, and for TFM files to be used
       with Xerox printing software it must contain at least 18 words, allocated as described
       below.

        ``header[0]`` is a 32-bit check sum that TEX will copy into
        the DVI output file whenever it uses the font.  Later on when
        the DVI file is printed, possibly on another computer, the
        actual font that gets used is supposed to have a check sum
        that agrees with the one in the TFM file used by TEX.  In this
        way, users will be warned about potential incompatibilities.
        (However, if the check sum is zero in either the font file or
        the TFM file, no check is made.)  The actual relation between
        this check sum and the rest of the TFM file is not important;
        the check sum is simply an identification number with the
        property that incompatible fonts almost always have distinct
        check sums.

        ``header[1]`` is a fix word containing the design size of the font, in units of TEX points
        (7227 TEX points = 254 cm).  This number must be at least 1.0; it is fairly arbitrary, but
        usually the design size is 10.0 for a "10 point" font, i.e., a font that was designed to
        look best at a 10-point size, whatever that really means.  When a TEX user asks for a font
        "at delta pt", the effect is to override the design size and replace it by delta, and to
        multiply the x and y coordinates of the points in the font image by a factor of delta
        divided by the design size.  All other dimensions in the TFM file are fix word numbers in
        design-size units.  Thus, for example, the value of ``param[6]``, one em or ``\quad``, is
        often the fix word value ``2**20 = 1.0``, since many fonts have a design size equal to one
        em.  The other dimensions must be less than 16 design-size units in absolute value; thus,
        ``header[1]`` and ``param[1]`` are the only fix word entries in the whole TFM file whose
        first byte might be something besides 0 or 255.
        
        ``header[2 ... 11]``, if present, contains 40 bytes that identify the character coding
        scheme.  The first byte, which must be between 0 and 39, is the number of subsequent ASCII
        bytes actually relevant in this string, which is intended to specify what
        character-code-to-symbol convention is present in the font.  Examples are ASCII for standard
        ASCII, TeX text for fonts like cmr10 and cmti9, TeX math extension for cmex10, XEROX text
        for Xerox fonts, GRAPHIC for special-purpose non- alphabetic fonts, UNSPECIFIED for the
        default case when there is no information.  Parentheses should not appear in this name.
        (Such a string is said to be in BCPL format.)

        ``header[12 ... 16]``, if present, contains 20 bytes that name the font family (e.g., CMR or
        HELVETICA), in BCPL format.  This field is also known as the "font identifier."

        ``header[17]``, if present, contains a first byte called the ``seven_bit_safe_flag``, then
        two bytes that are ignored, and a fourth byte called the *face*.  If the value of the fourth
        byte is less than 18, it has the following interpretation as a "weight, slope, and
        expansion": Add 0 or 2 or 4 (for medium or bold or light) to 0 or 1 (for roman or italic) to
        0 or 6 or 12 (for regular or condensed or extended).  For example, 13 is ``0+1+12``, so it
        represents medium italic extended.  A three-letter code (e.g., MIE) can be used for such
        face data.

        ``header[18 ... whatever]`` might also be present; the individual words are simply called
        ``header[18]``, ``header[19]``, etc., at the moment.
        ***/        

  read_header() {
    this.seek_to_table(tables.header);

    // Read header[0 ... 1]
    var checksum = this.read_unsigned_byte4();
    var designSize = this.read_fix_word();
        
    // Read header[2 ... 11] if there
    var character_info_table_position = this.table_pointers[tables.character_info];
    var position = this.position;
    
    var  character_coding_scheme;
    if (position < character_info_table_position) 
      character_coding_scheme = this.read_bcpl();

    // Read header[12 ... 16] if there
    var character_coding_scheme_length = 40; // bytes (11 - 2 + 1) * 4 = 10 * 4 
    position += character_coding_scheme_length;
    var family;
    if (position < character_info_table_position)
      family = this.read_bcpl(position);

    // Read header[12 ... 16] if there
    var family_length = 20; // bytes (16 - 12 +1) * 4 = 5 * 4 
    position += family_length;
    if (position < character_info_table_position) {
      var seven_bit_safe_flag = this.read_unsigned_byte1(position);
      this.read_unsigned_byte2();
      var face = this.read_unsigned_byte1();
    }

    // Fixme: complete
    // don't read header [18 ... whatever]

    this.tfm = new Tfm.Tfm(
                   this.smallest_character_code,
                   this.largest_character_code,
                   checksum,
                   designSize,
                   character_coding_scheme,
                   family);

  }

          /*** The final portion of a TFM fie is the param array, which is another sequence of fix word
        values.

        * param[1] = ``slant`` is the amount of italic slant, which is used to help position
          accents.  For example, slant = .25 means that when you go up one unit, you also go .25
          units to the right.  The slant is a pure number; it's the only fix word other than the
          design size itself that is not scaled by the design size.
        * param[2] = ``space`` is the normal spacing between words in text. Note that character " "
          in the font need not have anything to do with blank spaces.
        * param[3] = ``space_stretch`` is the amount of glue stretching between words.
        * param[4] = ``space_shrink`` is the amount of glue shrinking between words.
        * param[5] = ``x_height`` is the height of letters for which accents don't have to be
          raised or lowered.
        * param[6] = ``quad`` is the size of one em in the font.
        * param[7] = ``extra_space`` is the amount added to param[2] at the ends of sentences.

        When the character coding scheme is ``TeX math symbols``, the font is supposed to have 15
        additional parameters called ``num1``, ``num2``, ``num3``, ``denom1``, ``denom2``, ``sup1``,
        ``sup2``, ``sup3``, ``sub1``, ``sub2``, ``supdrop``, ``subdrop``, ``delim1``, ``delim2``,
        and ``axis_height``, respectively.  When the character coding scheme is ``TeX math
        extension``, the font is supposed to have six additional parameters called
        ``defaul_rule_thickness`` and ``big_op_spacing1`` through ``big_op_spacing5``.
        ***/        
  read_font_parameters() {
    this.seek_to_table(tables.font_parameter);
    var stream = this;

    if (this.tfm.character_coding_scheme == 'TeX math italic') {
      // undocumented in tftopl web
      //throw 'undocumented character coding scheme';
    } else {
      // Read the seven fix word parameters
      this.tfm.set_font_parameters( [...Array(7).keys()].map( function() { return stream.read_fix_word(); } ) );
    }

    if (this.tfm.character_coding_scheme == 'TeX math symbols') {
      // Read the additional 15 fix word parameters
      this.tfm.set_math_symbols_parameters( [...Array(15).keys()].map( function() { return stream.read_fix_word(); } ) );
    }
    
    if ((this.tfm.character_coding_scheme == 'TeX math extension') ||
        (this.tfm.character_coding_scheme == 'euler substitutions only')) {
      // Read the additional 6 fix word parameters
      this.tfm.set_math_extension_parameters( [...Array(6).keys()].map( function() { return stream.read_fix_word(); } ) );
    }
  }

        /*** The lig kern array contains instructions in a simple programming language that explains
        what to do for special letter pairs. Each word is a lig kern command of four bytes.

        * first byte: ``skip_byte``, indicates that this is the final program step if the byte is
          128 or more, otherwise the next step is obtained by skipping this number of intervening
          steps.
        * second byte: ``next_char``, "if ``next_char`` follows the current character, then perform
          the operation and stop, otherwise continue."
        * third byte: ``op_byte``, indicates a ligature step if less than 128, a kern step otherwise.
        * fourth byte: ``remainder``.

        In a kern step, an additional space equal to ``kern[256 * (op_byte + 128) + remainder]`` is
        inserted between the current character and next char.  This amount is often negative, so
        that the characters are brought closer together by kerning; but it might be positive.

        There are eight kinds of ligature steps, having ``op_byte`` codes ``4a+2b+c`` where ``0 <= a
        <= b+c`` and ``0 <= b; c <= 1``.  The character whose code is remainder is inserted between
        the current character and next char; then the current character is deleted if ``b = 0``, and
        next char is deleted if ``c = 0``; then we pass over a characters to reach the next current
        character (which may have a ligature/kerning program of its own).
        
        Notice that if ``a = 0`` and ``b = 1``, the current character is unchanged; if ``a = b`` and
        ``c = 1``, the current character is changed but the next character is unchanged.

        If the very first instruction of the lig kern array has ``skip_byte = 255``, the
        ``next_char`` byte is the so-called right boundary character of this font; the value of
        ``next_char`` need not lie between ``bc`` and ``ec``. If the very last instruction of the
        lig kern array has ``skip_byte = 255``, there is a special ligature/kerning program for a
        left boundary character, beginning at location ``256 * op_byte + remainder``.  The
        interpretation is that TEX puts implicit boundary characters before and after each
        consecutive string of characters from the same font.  These implicit characters do not
        appear in the output, but they can affect ligatures and kerning.

        If the very first instruction of a character's ``lig_kern`` program has ``skip_byte > 128``,
        the program actually begins in location ``256 * op_byte + remainder``.  This feature allows
        access to large lig kern arrays, because the first instruction must otherwise appear in a
        location ``<= 255``.

        Any instruction with ``skip_byte > 128`` in the lig kern array must have ``256 * op_byte +
        remainder < nl``.  If such an instruction is encountered during normal program execution, it
        denotes an unconditional halt; no ligature command is performed.
        ***/  
  read_lig_kern_programs() {
    // Fixme: complete special cases
    this.seek_to_table(tables.lig_kern);
    
    // Read very first instruction of the table
    var first_skip_byte = this.read_unsigned_byte1();
    var next_char  = this.read_unsigned_byte1();
    var op_byte = this.read_unsigned_byte1();
    var remainder =  this.read_unsigned_byte1();
    
    if (first_skip_byte == 255) {
      var right_boundary_char = next_char;
      throw Error('Font has right boundary char');
    }

    this.seek_to_table(tables.lig_kern, this.table_lengths[tables.lig_kern] - 1);
    var last_skip_byte = this.read_unsigned_byte1();
    next_char  = this.read_unsigned_byte1();
     op_byte = this.read_unsigned_byte1();
     remainder =  this.read_unsigned_byte1();
                                                            
    if (last_skip_byte == 255) {
      let left_boundary_char_program_index = 256*op_byte + remainder;
      throw Error('Font has left boundary char program');
    }

    // Read the instructions
    var first_instruction = true;
    for( var i = 0; i < this.table_lengths[tables.lig_kern]; i++ ) {
      this.seek_to_table(tables.lig_kern, i);
      var skip_byte = this.read_unsigned_byte1();
      next_char  = this.read_unsigned_byte1();
      op_byte = this.read_unsigned_byte1();
      remainder =  this.read_unsigned_byte1();
      
      // Large lig/kern table ?
      if (first_instruction && (skip_byte > 128)) {
        var large_index = 256*op_byte + remainder;
        skip_byte = this.read_unsigned_byte1();
        next_char  = this.read_unsigned_byte1();
        op_byte = this.read_unsigned_byte1();
        remainder =  this.read_unsigned_byte1();
      }

      // Last step ?
      var stop = skip_byte >= 128;

      if (op_byte >= KERN_OPCODE) {
        // Kern step
        var kern_index = 256*(op_byte - KERN_OPCODE) + remainder;
        var kern = this.read_fix_word_in_table(tables.kern, kern_index);
        // Fixme: self registration ?
        new Tfm.TfmKern(this.tfm, i, stop, next_char, kern);
      } else {
        // Ligature step
        var number_of_chars_to_pass_over = op_byte >> 2;
        var current_char_is_deleted = (op_byte & 0x02) == 0;
        var next_char_is_deleted    = (op_byte & 0x01) == 0;
        var ligature_char_code = remainder;
        
        // Fixme: self registration ?
        new Tfm.TfmLigature(this.tfm,
                            i,
                            stop,
                            next_char,
                            ligature_char_code,
                            number_of_chars_to_pass_over,
                            current_char_is_deleted,
                            next_char_is_deleted);
      }

      first_instruction = (stop == true);
    }
  }

        /*** Next comes the char info array, which contains one char info word per character.  Each
        char info word contains six fields packed into four bytes as follows.

        * first byte: ``width_index`` (8 bits)
        * second byte: ``height_index`` (4 bits) times 16, plus depth index (4 bits)
        * third byte: ``italic_index`` (6 bits) times 4, plus tag (2 bits)
        * fourth byte: ``remainder`` (8 bits)

        The actual width of a character is ``width[width_index]``, in design-size units; this is a
        device for compressing information, since many characters have the same width.  Since it is
        quite common for many characters to have the same height, depth, or italic correction, the
        TFM format imposes a limit of 16 different heights, 16 different depths, and 64 different
        italic corrections.

        Incidentally, the relation ``width[0] = height[0] = depth[0] = italic[0] = 0`` should
        always hold, so that an index of zero implies a value of zero.  The width index should never
        be zero unless the character does not exist in the font, since a character is valid if and
        only if it lies between ``bc`` and ``ec`` and has a nonzero width index.

        The tag field in a char info word has four values that explain how to interpret the remainder field.

        * ``tag = 0`` (``no_tag``) means that remainder is unused.
        * ``tag = 1`` (``lig_tag``) means that this character has a ligature/kerning program
          starting at ``lig_kern[remainder]``.
        * ``tag = 2`` (``list_tag``) means that this character is part of a chain of characters of
          ascending sizes, and not the largest in the chain.  The remainder field gives the
          character code of the next larger character.
        * ``tag = 3`` (``ext_tag``) means that this character code represents an extensible
          character, i.e., a character that is built up of smaller pieces so that it can be made
          arbitrarily large.  The pieces are specified in ``exten[remainder]``.
        * ``no_tag = 0`` vanilla character
        * ``lig_tag = 1`` character has a ligature/kerning program
        * ``list_tag = 2`` character has a successor in a charlist
        * ``ext_tag = 3`` character is extensible
        ***/

  read_characters() {
    // Read the character information table
    for (var c = this.smallest_character_code; c < this.largest_character_code; c++ ) {
      this.process_char(c);
    }
  }

  process_char(c) {
    /*** Process the character code *c* in the character information table.
     ***/
    var info = this.read_char_info(c);
    var width_index = info.width_index;
    var height_index = info.height_index;
    var depth_index = info.depth_index;
    var italic_index = info.italic_index;
    var tag = info.tag;
    var remainder  = info.remainder;

    // Get the parameters in the corresponding tables
    var width = 0;
    if (width_index != 0)
      width = this.read_fix_word_in_table(tables.width, width_index);
    // warning: euex10 has zero width characters

    var height = 0;
    if (height_index != 0) 
      height = this.read_fix_word_in_table(tables.height, height_index)

    var depth = 0;
    if (depth_index != 0)
      depth = this.read_fix_word_in_table(tables.depth, depth_index)

    var italic_correction = 0;
    if (italic_index != 0)
      italic_correction = this.read_fix_word_in_table(tables.italic_correction, italic_index)

    // Interpret the tag field
    var lig_kern_program_index;
    var next_larger_char;
    var extensible_recipe;
    if (tag == LIG_TAG) 
      lig_kern_program_index = remainder;
    
    if (tag == LIST_TAG)
      next_larger_char = remainder;
    
    if (tag == EXT_TAG) 
      extensible_recipe = this.read_extensible_recipe(remainder);

    if (extensible_recipe !== undefined) {
      // Fixme: self registration ?
      new Tfm.TfmExtensibleChar(this.tfm,
                                c,
                                width,
                                height,
                                depth,
                                italic_correction,
                                extensible_recipe,
                                lig_kern_program_index,
                                next_larger_char);
    } else {
      // Fixme: self registration ?
      new Tfm.TfmChar(this.tfm,
                      c,
                      width,
                      height,
                      depth,
                      italic_correction,
                      lig_kern_program_index,
                      next_larger_char);
    }
  }
    
  read_char_info(c) {
    /*** Read the character code *c* data in the character information table.
     ***/        
    var index = c - this.smallest_character_code;
    var bytes = [];
    this.seek_to_table( tables.character_info, index);
    bytes[0] = this.read_unsigned_byte1();
    bytes[1] = this.read_unsigned_byte1();
    bytes[2] = this.read_unsigned_byte1();
    bytes[3] = this.read_unsigned_byte1();    

    return {
      width_index  : bytes[0],
      height_index : bytes[1] >> 4,
      depth_index  : bytes[1] & 0xF,
      italic_index : bytes[2] >> 6,
      tag          : bytes[2] & 0x3,
      remainder    : bytes[3]
    };
  }
}

function parse(buffer) {
  var p = new TFMParser(buffer);
  return p.tfm;
}

export function tfmData( fontname ) {
  if (fontdata[fontname]) {
    let buffer = Buffer.from(fontdata[fontname], 'base64');
    return buffer;
  }
    
  throw Error(`Could not find font ${fontname}`);
}

export function loadFont(fontname) {
  return parse(tfmData(fontname));
}
