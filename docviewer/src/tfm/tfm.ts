// PyDvi - A Python Library to Process DVI Stream
// Copyright (C) 2014 Fabrice Salvaire
//;
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

/*  This module handles TeX Font Metric.

The class :class:`PyDvi.Tfm` handles the font's metric.  To get a :class:`PyDvi.Tfm` instance for a
particular font use the static method :meth:`PyDvi.TfmParser.TfmParser.parse`.  For example use this
code for the font "cmr10"::

  tfm = TfmParser.parse('cmr10', '/usr/share/texmf/fonts/tfm/public/cm/cmr10.tfm')

The number of characters in the font can be obtained using the function :func:`len`::

  >>> len(tfm)
  128

Each character's metric is stored in a :class:`TfmChar` instance that can be accessed using the char
code as index on the :class:`Tfm` class instance.  For example to get the metric of the character
"A" use::

   tfm[ord('A')]

 */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;

export class TfmChar {
  tfm : Tfm;
  char_code : number;
  width : number;
  height : number;
  depth : number;
  italic_correction : number;
  lig_kern_program_index : number;
  next_larger_char : number;

  
  constructor(
    tfm,
    char_code,
    width,
    height,
    depth,
    italic_correction,
    lig_kern_program_index,
    next_larger_char) {

    this.tfm = tfm;
    tfm.set_char(char_code, this);

    this.char_code = char_code;
    this.width = width;
    this.height = height;
    this.depth = depth;
    this.italic_correction = italic_correction;

    this.lig_kern_program_index = lig_kern_program_index;
    this.next_larger_char = next_larger_char;
  }
  
  scaled_width(scale_factor) {
    /*  Return the scaled width by *scale_factor*.  */
    return this.width * scale_factor;
  }

  scaled_height(scale_factor) {
    /*  Return the scaled height by *scale_factor*.  */
    return (this.height * scale_factor);
  }

  scaled_depth(scale_factor) {
    /*  Return the scaled depth by *scale_factor*.  */
    return Number(this.depth * scale_factor);
  }

  scaled_dimensions(scale_factor) {
    /*  Return the 3-tuple made of the scaled width, height and depth by *scale_factor*.  */
    return [this.width, this.height, this.depth].map( function(x) { return x * scale_factor; } );
  }

  next_larger_tfm_char() {
    /*  Return the :class:`TfmChar` instance for the next larger char if it exists else return
        :obj:`None`. */;

    if (this.next_larger_char !== null) {
      return this.tfm.get_char(this.next_larger_char);
    } else {
      return null;
    }
  }    

  get_lig_kern_program(self) {
    /*  Get the ligature/kern program of the character.  */;

    if (this.lig_kern_program_index !== null) {
      return this.tfm.get_lig_kern_program(this.lig_kern_program_index);
    } else {
      return null;
    }
  }
}

/*  This class encapsulates a TeX Font Metric for an extensible Glyph. */
export class TfmExtensibleChar extends TfmChar {
  top : number;
  mid : number;
  bot : number;
  rep : number;
  
  constructor(
    tfm,
    char_code,
    width,
    height,
    depth,
    italic_correction,
    extensible_recipe,
    lig_kern_program_index,
    next_larger_char) {
    
    super(tfm,
          char_code,
          width,
          height,
          depth,
          italic_correction,
          lig_kern_program_index,
          next_larger_char);
    
    // BADBAD
    this.top, this.mid, this.bot, this.rep = extensible_recipe;
  }
}

export class TfmLigKern {
  tfm : Tfm;
  stop : number;
  index : number;
  next_char: TfmChar;
  
  constructor(tfm, index, stop, next_char) {
    this.tfm = tfm;
    this.stop = stop;
    this.index = index;
    this.next_char = next_char;
    this.tfm.add_lig_kern(this);
  }
}


/*  This class represents a Kerning Program Instruction. */
export class TfmKern extends TfmLigKern {
  kern: number;
  
  constructor(tfm, index, stop, next_char, kern) {
    super(tfm, index, stop, next_char);
    this.kern = kern;
  }
}

/*  This class represents a Ligature Program Instruction. */
export class TfmLigature extends TfmLigKern {
  ligature_char_code: number;
  number_of_chars_to_pass_over: number;
  current_char_is_deleted: boolean;
  next_char_is_deleted: boolean;
  
    constructor(tfm,
                 index,
                 stop,
                 next_char,
                 ligature_char_code,
                 number_of_chars_to_pass_over,
                 current_char_is_deleted,
                 next_char_is_deleted) {

      super(tfm, index, stop, next_char);
      this.ligature_char_code = ligature_char_code;
      this.number_of_chars_to_pass_over = number_of_chars_to_pass_over;
      this.current_char_is_deleted = current_char_is_deleted;
      this.next_char_is_deleted = next_char_is_deleted;
    }
}

/*  This class encapsulates a TeX Font Metric for a font. */
export class Tfm {
  smallest_character_code : number;
  largest_character_code : number;
  checksum : number;
  designSize : number;
  character_coding_scheme : string;
  family : string;

  slant: number;
  spacing: number;
  space_stretch: number;
  space_shrink: number;
  x_height: number;
  quad: number;
  extra_space: number;
  num1: number;
  num2: number;
  num3: number;
  denom1: number;
  denom2: number;
  sup1: number;
  sup2: number;
  sup3: number;
  sub1: number;
  sub2: number;
  supdrop: number;
  subdrop: number;
  delim1: number;
  delim2: number;
  axis_height: number;
  default_rule_thickness: number;
  big_op_spacing: number;

  _lig_kerns: TfmLigKern[];
  characters: any;
  
  constructor(  
                 smallest_character_code,
                 largest_character_code,
                 checksum,
                 designSize,
                 character_coding_scheme,
                 family) {

    this.smallest_character_code = smallest_character_code;
    this.largest_character_code = largest_character_code;
    this.checksum = checksum;
    this.designSize = designSize;
    this.character_coding_scheme = character_coding_scheme;
    this.family = family;

    this._lig_kerns = [];
    this.characters = {};
  }

  get_char(x) {
    return this.characters[x];
  }

  set_char(x,y) {
    this.characters[x] = y;
  }
  
 set_font_parameters(parameters) {
        /*  Set the font parameters.  */;
      this.slant = parameters[0];
      this.spacing = parameters[1];
      this.space_stretch = parameters[2];
      this.space_shrink = parameters[3];
      this.x_height = parameters[4];
      this.quad = parameters[5];
      this.extra_space = parameters[6];
    }
  
 set_math_symbols_parameters(parameters) {
      /*  Set the math symbols parameters.  */;
      this.num1 = parameters[0];
      this.num2 = parameters[1];
      this.num3 = parameters[2];
      this.denom1 = parameters[3];
      this.denom2 = parameters[4];
      this.sup1 = parameters[5];
      this.sup2 = parameters[6];
      this.sup3 = parameters[7];
      this.sub1 = parameters[8];
      this.sub2 = parameters[9];
      this.supdrop = parameters[10];
      this.subdrop = parameters[11];
      this.delim1 = parameters[12];
      this.delim2 = parameters[13];
      this.axis_height = parameters[14];
    }

  set_math_extension_parameters(parameters) {
    this.default_rule_thickness = parameters[0];
    this.big_op_spacing = parameters.slice(1);
  }

  add_lig_kern(obj) {
    /*  Add a ligature/kern program *obj*.  */;
    this._lig_kerns.push(obj);
  }

  get_lig_kern_program(i) {
    /*  Return the ligature/kern program at index *i*.  */;
    return this._lig_kerns[i];
  }
}

