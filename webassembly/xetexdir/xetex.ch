/****************************************************************************\
 Part of the XeTeX typesetting system
 Copyright (c) 1994-2008 by SIL International
 Copyright (c) 2009-2012 by Jonathan Kew
 Copyright (c) 2012 by Khaled Hosny

 SIL Author(s): Jonathan Kew

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the copyright holders
shall not be used in advertising or otherwise to promote the sale,
use or other dealings in this Software without prior written
authorization from the copyright holders.
\****************************************************************************/

@x
\let\maybe=\iffalse
@y
\let\maybe=\iftrue
@z

@x [1] m.2 l.188 - banner
@d banner==TeX_banner
@d banner_k==TeX_banner_k
@y
@d banner==XeTeX_banner
@d banner_k==XeTeX_banner
@z

@x [2.20] l.579 - printable characters
xchr: array [ASCII_code] of text_char;
   { specifies conversion of output characters }
xprn: array [ASCII_code] of ASCII_code;
   { non zero iff character is printable }
@y
@!xchr: ^text_char;
  {dummy variable so tangle doesn't complain; not actually used}
@z

@x [2.23] l.723 - Translate characters if desired, otherwise allow them all.
{Initialize |xchr| to the identity mapping.}
for i:=0 to @'37 do xchr[i]:=i;
for i:=@'177 to @'377 do xchr[i]:=i;
@y
@z

@x [2.24] l.733 - Don't reinitialize xord.
for i:=0 to @'176 do xord[xchr[i]]:=i;
{Set |xprn| for printable ASCII, unless |eight_bit_p| is set.}
for i:=0 to 255 do xprn[i]:=(eight_bit_p or ((i>=" ")and(i<="~")));

{The idea for this dynamic translation comes from the patch by
 Libor Skarvada \.{<libor@@informatics.muni.cz>}
 and Petr Sojka \.{<sojka@@informatics.muni.cz>}. I didn't use any of the
 actual code, though, preferring a more general approach.}

{This updates the |xchr|, |xord|, and |xprn| arrays from the provided
 |translate_filename|.  See the function definition in \.{texmfmp.c} for
 more comments.}
if translate_filename then read_tcx_file;
@y
@z

@x [3.26] l.789 - name_of_file is no longer an array
@!name_of_file:^text_char;
@y
@!name_of_file:^UTF8_code; {we build filenames in utf8 to pass to the OS}
@z

@x
@!name_of_file16:array[1..file_name_size] of UTF16_code;@;@/
@y
@!name_of_file16:^UTF16_code;
@z

@x [3.30] l.888 - Array size of input buffer is determined at runtime.
@!buffer:^ASCII_code; {lines of characters being read}
@y
@!buffer:^UnicodeScalar; {lines of characters being read}
@z

@x [3.32] l.961 - `term_in' and `term_out' are standard input and output.
@d term_in==stdin {the terminal as an input file}
@y
@z

@x [3.32] l.961
@!bound_default:integer; {temporary for setup}
@y
@!term_in:unicode_file;
@#
@!bound_default:integer; {temporary for setup}
@z

@x [5.61] l.1556 - Print rest of banner, eliminate misleading `(no format preloaded)'.
if translate_filename then begin
  wterm(' (');
  fputs(translate_filename, stdout);
  wterm_ln(')');
@y
if translate_filename then begin
  wterm(' (WARNING: translate-file "');
  fputs(translate_filename, stdout);
  wterm_ln('" ignored)');
@z

@x We add a new |history| value for failure of the output driver
has been detected. It has four possible values: |spotless|, |warning_issued|,
|error_message_issued|, and |fatal_error_stop|.
@y
has been detected. It has five possible values: |spotless|, |warning_issued|,
|error_message_issued|, |fatal_error_stop|, and |output_failure|.
@z

@x
@d fatal_error_stop=3 {|history| value when termination was premature}
@y
@d fatal_error_stop=3 {|history| value when termination was premature}
@d output_failure=4 {|history| value when output driver returned an error}
@z

@x
@!history:spotless..fatal_error_stop; {has the source input been clean so far?}
@y
@!history:spotless..output_failure; {has the source input been clean so far?}
@z

@x [6.84] l.1904 - Implement the switch-to-editor option.
    begin edit_name_start:=str_start[edit_file.name_field];
    edit_name_length:=str_start[edit_file.name_field+1] -
                      str_start[edit_file.name_field];
@y
    begin edit_name_start:=str_start_macro(edit_file.name_field);
    edit_name_length:=str_start_macro(edit_file.name_field+1) -
                      str_start_macro(edit_file.name_field);
@z

@x [8.110] l.2423
@d max_halfword==@"FFFFFFF {largest allowable value in a |halfword|}
@y
@d max_halfword==@"3FFFFFFF {largest allowable value in a |halfword|}
@z

@x [15.209] l.4165
@d shorthand_def=95 {code definition ( \.{\\chardef}, \.{\\countdef}, etc.~)}
@y
@d shorthand_def=97 {code definition ( \.{\\chardef}, \.{\\countdef}, etc.~)}
@z

@x [16.215] - e-TeX last_node_type
last_glue:=max_halfword; last_penalty:=0; last_kern:=0;
@y
last_glue:=max_halfword; last_penalty:=0; last_kern:=0;
last_node_type:=-1;
@z

@x [17.222] l.4523 - frozen_special, for source specials.
@d frozen_null_font=frozen_control_sequence+11
@y
@d frozen_null_font=frozen_control_sequence+12+prim_size
@z

@x [17.230] l.4731
@d char_sub_code_base=math_code_base+256 {table of character substitutions}
@d int_base=char_sub_code_base+256 {beginning of region 5}
@y
@d char_sub_code_base=math_code_base+number_usvs {table of character substitutions}
@d int_base=char_sub_code_base+number_usvs {beginning of region 5}
@z

@x [17.236] l.4960 - first web2c, then e-TeX additional integer parameters
@d int_pars=web2c_int_pars {total number of integer parameters}
@#
@d etex_int_base=tex_int_pars {base for \eTeX's integer parameters}
@y
@d etex_int_base=web2c_int_pars {base for \eTeX's integer parameters}
@z

@x [22.304] l.6536 - texarray; additions for file:line:error style.
@!input_file : ^alpha_file;
@y
@!input_file : ^unicode_file;
@z

@x [29.516] l.9992 - filenames: more_name
begin if (c=" ") and stop_at_space and (not quoted_filename) then
  more_name:=false
else  if c="""" then begin
  quoted_filename:=not quoted_filename;
@y
begin if stop_at_space and (c=" ") and (file_name_quote_char=0) then
  more_name:=false
else if stop_at_space and (file_name_quote_char<>0) and (c=file_name_quote_char) then begin
  file_name_quote_char:=0;
  more_name:=true;
  end
else if stop_at_space and (file_name_quote_char=0) and ((c="""") or (c="'")) then begin
  file_name_quote_char:=c;
  quoted_filename:=true;
@z

@x [29.517] l.10002 - end_name: string recycling
@!j,@!s,@!t: pool_pointer; {running indices}
@!must_quote:boolean; {whether we need to quote a string}
@y
@!j: pool_pointer; {running index}
@z
@x [29.517] l.10002 - end_name: string recycling
str_room(6); {Room for quotes, if needed.}
{add quotes if needed}
if area_delimiter<>0 then begin
  {maybe quote |cur_area|}
  must_quote:=false;
  s:=str_start[str_ptr];
  t:=str_start[str_ptr]+area_delimiter;
  j:=s;
  while (not must_quote) and (j<t) do begin
    must_quote:=str_pool[j]=" "; incr(j);
    end;
  if must_quote then begin
    for j:=pool_ptr-1 downto t do str_pool[j+2]:=str_pool[j];
    str_pool[t+1]:="""";
    for j:=t-1 downto s do str_pool[j+1]:=str_pool[j];
    str_pool[s]:="""";
    if ext_delimiter<>0 then ext_delimiter:=ext_delimiter+2;
    area_delimiter:=area_delimiter+2;
    pool_ptr:=pool_ptr+2;
    end;
  end;
{maybe quote |cur_name|}
s:=str_start[str_ptr]+area_delimiter;
if ext_delimiter=0 then t:=pool_ptr else t:=str_start[str_ptr]+ext_delimiter-1;
must_quote:=false;
j:=s;
while (not must_quote) and (j<t) do begin
  must_quote:=str_pool[j]=" "; incr(j);
  end;
if must_quote then begin
  for j:=pool_ptr-1 downto t do str_pool[j+2]:=str_pool[j];
  str_pool[t+1]:="""";
  for j:=t-1 downto s do str_pool[j+1]:=str_pool[j];
  str_pool[s]:="""";
  if ext_delimiter<>0 then ext_delimiter:=ext_delimiter+2;
  pool_ptr:=pool_ptr+2;
  end;
if ext_delimiter<>0 then begin
  {maybe quote |cur_ext|}
  s:=str_start[str_ptr]+ext_delimiter-1;
  t:=pool_ptr;
  must_quote:=false;
  j:=s;
  while (not must_quote) and (j<t) do begin
    must_quote:=str_pool[j]=" "; incr(j);
    end;
  if must_quote then begin
    str_pool[t+1]:="""";
    for j:=t-1 downto s do str_pool[j+1]:=str_pool[j];
    str_pool[s]:="""";
    pool_ptr:=pool_ptr+2;
    end;
  end;
@y
@z

@x [29.517] l.10011 - end_name: string recycling
    for j:=str_start[str_ptr+1] to pool_ptr-1 do
@y
    for j:=str_start_macro(str_ptr+1) to pool_ptr-1 do
@z

@x [29.517] l.10016 - end_name: string recycling
    for j:=str_start[str_ptr+1] to pool_ptr-1 do
@y
    for j:=str_start_macro(str_ptr+1) to pool_ptr-1 do
@z

@x [29.518] l.10042 - print_file_name: quote if spaces in names.
@d check_quoted(#) == {check if string |#| needs quoting}
if #<>0 then begin
  j:=str_start[#];
  while (not must_quote) and (j<str_start[#+1]) do begin
    must_quote:=str_pool[j]=" "; incr(j);
@y
@d check_quoted(#) == {check if string |#| needs quoting}
if #<>0 then begin
  j:=str_start_macro(#);
  while ((not must_quote) or (quote_char=0)) and (j<str_start_macro(#+1)) do begin
    if str_pool[j]=" " then must_quote:=true
    else if (str_pool[j]="""") or (str_pool[j]="'") then begin
      must_quote:=true;
      quote_char:="""" + "'" - str_pool[j];
    end;
    incr(j);
@z

@x [29.518] l.10042 - print_file_name: quote if spaces in names.
@d print_quoted(#) == {print string |#|, omitting quotes}
if #<>0 then
  for j:=str_start[#] to str_start[#+1]-1 do
    if so(str_pool[j])<>"""" then
      print(so(str_pool[j]))
@y
@d print_quoted(#) == {print string |#|, omitting quotes}
if #<>0 then
  for j:=str_start_macro(#) to str_start_macro(#+1)-1 do begin
    if str_pool[j]=quote_char then begin
      print(quote_char);
      quote_char:="""" + "'" - quote_char;
      print(quote_char);
    end;
    print(str_pool[j]);
  end
@z

@x [29.518] l.10042 - print_file_name: quote if spaces in names.
@!j:pool_pointer; {index into |str_pool|}
begin
must_quote:=false;
check_quoted(a); check_quoted(n); check_quoted(e);
{FIXME: Alternative is to assume that any filename that has to be quoted has
 at least one quoted component...if we pick this, a number of insertions
 of |print_file_name| should go away.
|must_quote|:=((|a|<>0)and(|str_pool|[|str_start|[|a|]]=""""))or
              ((|n|<>0)and(|str_pool|[|str_start|[|n|]]=""""))or
              ((|e|<>0)and(|str_pool|[|str_start|[|e|]]=""""));}
if must_quote then print_char("""");
print_quoted(a); print_quoted(n); print_quoted(e);
if must_quote then print_char("""");
@y
@!quote_char: integer; {current quote char (single or double)}
@!j:pool_pointer; {index into |str_pool|}
begin
must_quote:=false;
quote_char:=0;
check_quoted(a); check_quoted(n); check_quoted(e);
if must_quote then begin
  if quote_char=0 then quote_char:="""";
  print_char(quote_char);
end;
print_quoted(a); print_quoted(n); print_quoted(e);
if quote_char<>0 then print_char(quote_char);
@z

@x [29.519] l.10051 - have append_to_name skip quotes.
@d append_to_name(#)==begin c:=#; if not (c="""") then begin incr(k);
  if k<=file_name_size then name_of_file[k]:=xchr[c];
  end end
@y
@d append_to_name(#)==begin c:=#; incr(k);
  if k<=file_name_size then begin
      if (c < 128) then name_of_file[k]:=c
      else if (c < @"800) then begin
        name_of_file[k]:=@"C0 + c div @"40; incr(k);
        name_of_file[k]:=@"80 + c mod @"40;
      end else begin
		name_of_file[k]:=@"E0 + c div @"1000; incr(k);
		name_of_file[k]:=@"80 + (c mod @"1000) div @"40; incr(k);
		name_of_file[k]:=@"80 + (c mod @"1000) mod @"40;
      end
    end
  end
@z

@x [29.519] l.10047 - pack_file_name, leave room for the extra null
name_of_file:= xmalloc_array (ASCII_code, length(a)+length(n)+length(e)+1);
@y
name_of_file:= xmalloc_array (UTF8_code, (length(a)+length(n)+length(e))*3+1);
@z

@x [29.523] l.10095 - Change to pack_buffered_name as with pack_file_name.
name_of_file := xmalloc_array (ASCII_code, n+(b-a+1)+format_ext_length+1);
for j:=1 to n do append_to_name(xord[ucharcast(TEX_format_default[j])]);
@y
name_of_file := xmalloc_array (UTF8_code, n+(b-a+1)+format_ext_length+1);
for j:=1 to n do append_to_name(TEX_format_default[j]);
@z

@x [29.523] l.10095 - Change to pack_buffered_name as with pack_file_name.
  append_to_name(xord[ucharcast(TEX_format_default[j])]);
@y
  append_to_name(TEX_format_default[j]);
@z

@x [29.525] l.10174 - make_name_string
  k:=1;
  while (k<=name_length)and(more_name(name_of_file[k])) do
@y
  k:=0;
  while (k<name_length16)and(more_name(name_of_file16[k])) do
@z

@x [29.526] l.10194 - stop scanning file name if we're at end-of-line.
  {If |cur_chr| is a space and we're not scanning a token list, check
   whether we're at the end of the buffer. Otherwise we end up adding
   spurious spaces to file names in some cases.}
  if (cur_chr=" ") and (state<>token_list) and (loc>limit) then goto done;
@y
@z

@x [29.536] l.10331
  wlog(' (');
  fputs(translate_filename, log_file);
  wlog(')');
@y
  wlog(' (WARNING: translate-file "');
  fputs(translate_filename, log_file);
  wlog('" ignored)');
@z

@x [29.537] l.10338 - start_input
var temp_str: str_number;
begin scan_file_name; {set |cur_name| to desired file name}
@y
var temp_str: str_number;
@!k:0..file_name_size; {index into |name_of_file16|}
begin scan_file_name; {set |cur_name| to desired file name}
@z

@x [29.537] l.10338 - start_input
     and a_open_in(cur_file, kpse_tex_format) then
    goto done;
@y
     and u_open_in(cur_file, kpse_tex_format, XeTeX_default_input_mode, XeTeX_default_input_encoding) then
    {At this point |name_of_file| contains the actual name found, as a UTF8 string.
     We convert to UTF16, then extract the |cur_area|, |cur_name|, and |cur_ext| from it.}
    begin
    make_utf16_name;
    name_in_progress:=true;
    begin_name;
    stop_at_space:=false;
    k:=0;
    while (k<name_length16)and(more_name(name_of_file16[k])) do
      incr(k);
    stop_at_space:=true;
    end_name;
    name_in_progress:=false;
    goto done;
    end;
@z

@x [30.549] l.10682 - texarray
@!font_bc: ^eight_bits;
  {beginning (smallest) character code}
@!font_ec: ^eight_bits;
  {ending (largest) character code}
@y
@!font_bc: ^UTF16_code;
  {beginning (smallest) character code}
@!font_ec: ^UTF16_code;
  {ending (largest) character code}
@z

@x [30.549] l.10682 - texarray
@!font_false_bchar: ^nine_bits;
  {|font_bchar| if it doesn't exist in the font, otherwise |non_char|}
@y
@!font_false_bchar: ^nine_bits;
  {|font_bchar| if it doesn't exist in the font, otherwise |non_char|}
@#
@!font_layout_engine: ^void_pointer; {either an CFDictionaryRef or a XeTeXLayoutEngine}
@!font_mapping: ^void_pointer; { |TECkit_Converter| or 0 }
@!font_flags: ^char; { flags:
  0x01: |font_colored|
  0x02: |font_vertical| }
@!font_letter_space: ^scaled; { letterspacing to be applied to the font }
@!loaded_font_mapping: void_pointer; { used by |load_native_font| to return mapping, if any }
@!loaded_font_flags: char; { used by |load_native_font| to return flags }
@!loaded_font_letter_space: scaled;
@!loaded_font_design_size: scaled;
@!mapped_text: ^UTF16_code; { scratch buffer used while applying font mappings }
@!xdv_buffer: ^char; { scratch buffer used in generating XDV output }
@z

@x [30.561] l.10939 - Check lengths
else print(" not loadable: Metric (TFM) file not found");
@y
else print(" not loadable: Metric (TFM) file or installed font not found");
@z

@x
@ We need a few subroutines for |new_character|.

@p @t\4@>@<Declare subroutines for |new_character|@>@;
@y
@ The subroutines for |new_character| have been moved.
@z

@x [30.582] l.11276 - MLTeX: call |effective_char| in |new_character|
@p function new_character(@!f:internal_font_number;@!c:eight_bits):pointer;
@y
@p function new_character(@!f:internal_font_number;@!c:ASCII_code):pointer;
@z

@x [30.582] l.11276 - MLTeX: call |effective_char| in |new_character|
begin ec:=effective_char(false,f,qi(c));
@y
begin
if is_native_font(f) then
  begin new_character:=new_native_character(f,c); return;
  end;
ec:=effective_char(false,f,qi(c));
@z

@x [32.619] l.12294 - MLTeX: substitute character in |hlist_out|
label reswitch, move_past, fin_rule, next_p, continue, found;
@y
label reswitch, move_past, fin_rule, next_p, continue, found, check_next, end_node_run;
@z

@x
if list_ptr(p)=null then cur_v:=cur_v+height(p)+depth(p)
@y
if list_ptr(p)=null then begin
    if upwards then cur_v:=cur_v-depth(p)-height(p) else cur_v:=cur_v+height(p)+depth(p);
  end
@z

@x [32.645] l.12780 - use print_file_name
  print_nl("Output written on "); print_file_name(0, output_file_name, 0);
@.Output written on x@>
  print(" ("); print_int(total_pages);
  if total_pages<>1 then print(" pages")
  else print(" page");
  print(", "); print_int(dvi_offset+dvi_ptr); print(" bytes).");
  b_close(dvi_file);
@y
  k:=dvi_close(dvi_file);
  if k=0 then begin
    print_nl("Output written on "); print(output_file_name);
@.Output written on x@>
    print(" ("); print_int(total_pages);
    if total_pages<>1 then print(" pages")
    else print(" page");
    if no_pdf_output then begin
      print(", "); print_int(dvi_offset+dvi_ptr); print(" bytes).");
    end else print(").");
  end else begin
    print_nl("Error "); print_int(k); print(" (");
    if no_pdf_output then print_c_string(strerror(k))
    else print("driver return code");
    print(") generating output;");
    print_nl("file "); print(output_file_name); print(" may not be valid.");
    history:=output_failure;
    end;
@z

@x [43.943] l.18348 - bigtrie: Larger hyphenation tries.
@!trie_used:array[ASCII_code] of trie_opcode;
@y
@!trie_used:array[0..biggest_lang] of trie_opcode;
@z

@x [43.946] l.18416 - bigtrie: And larger tries again.
for k:=0 to 255 do trie_used[k]:=min_trie_op;
@y
for k:=0 to biggest_lang do trie_used[k]:=min_trie_op;
@z

@xx [43.958] l.18638 - bigtrie: Larger tries.
  begin for r:=0 to 256 do clear_trie;
  trie_max:=256;
@y
  begin for r:=0 to max_hyph_char do clear_trie;
  trie_max:=max_hyph_char;
@z

@x [49.1222] l.22833 - MLTeX: \charsubdef primitive
else begin n:=cur_chr; get_r_token; p:=cur_cs; define(p,relax,256);
@y
else begin n:=cur_chr; get_r_token; p:=cur_cs; define(p,relax,too_big_usv);
@z

@x [49.1275] l.23441 - Same stuff as for \input, this time for \openin.
var c:0..1; {1 for \.{\\openin}, 0 for \.{\\closein}}
@!n:0..15; {stream number}
@y
var c:0..1; {1 for \.{\\openin}, 0 for \.{\\closein}}
@!n:0..15; {stream number}
@!k:0..file_name_size; {index into |name_of_file16|}
@z

@x [49.1275] l.23441 - Same stuff as for \input, this time for \openin.
     and a_open_in(read_file[n], kpse_tex_format) then
    read_open[n]:=just_open;
@y
     and u_open_in(read_file[n], kpse_tex_format, XeTeX_default_input_mode, XeTeX_default_input_encoding) then
    begin
    make_utf16_name;
    name_in_progress:=true;
    begin_name;
    stop_at_space:=false;
    k:=0;
    while (k<name_length16)and(more_name(name_of_file16[k])) do
      incr(k);
    stop_at_space:=true;
    end_name;
    name_in_progress:=false;
    read_open[n]:=just_open;
    end;
@z

@x [50.1302] l.23690 - store_fmt_file
@!format_engine: ^text_char;
@y
@!format_engine: ^char;
@z

@x [50.1303] l.23722 - load_fmt_file
@!format_engine: ^text_char;
@!dummy_xord: ASCII_code;
@!dummy_xchr: text_char;
@!dummy_xprn: ASCII_code;
@y
@!format_engine: ^char;
@z

@x [50,1307] l.23779 - texarray
format_engine:=xmalloc_array(text_char,x+4);
@y
format_engine:=xmalloc_array(char,x+4);
@z

@x [50,1307] l.23779 - texarray
@<Dump |xord|, |xchr|, and |xprn|@>;
@y
@z

@x [50.1308] l.23793 - texarray
format_engine:=xmalloc_array(text_char, x);
@y
format_engine:=xmalloc_array(char, x);
@z

@x [50.1308] l.23793 - texarray
@<Undump |xord|, |xchr|, and |xprn|@>;
@y
@z

@x [50.1309] l.23814 - Make dumping/undumping more efficient.
dump_things(str_start[0], str_ptr+1);
@y
dump_things(str_start_macro(too_big_char), str_ptr+1-too_big_char);
@z

@x [50.1310] l.23829 - Make dumping/undumping more efficient.
undump_checked_things(0, pool_ptr, str_start[0], str_ptr+1);@/
@y
undump_checked_things(0, pool_ptr, str_start_macro(too_big_char), str_ptr+1-too_big_char);@/
@z

@x [50.1322] l.24000 - Make dumping/undumping more efficient - tfm
  print_file_name(font_name[k],font_area[k],"");
@y
  if is_native_font(k) or (font_mapping[k]<>0) then
    begin print_file_name(font_name[k],"","");
    print_err("Can't \dump a format with native fonts or font-mappings");
    help3("You really, really don't want to do this.")
    ("It won't work, and only confuses me.")
    ("(Load them at runtime, not as part of the format file.)");
    error;
    end
  else print_file_name(font_name[k],font_area[k],"");
@z

@x [50.1322] l.24031 - Make dumping/undumping more efficient - tfm
begin {Allocate the font arrays}
@y
begin {Allocate the font arrays}
font_mapping:=xmalloc_array(void_pointer, font_max);
font_layout_engine:=xmalloc_array(void_pointer, font_max);
font_flags:=xmalloc_array(char, font_max);
font_letter_space:=xmalloc_array(scaled, font_max);
@z

@x [50.1322] l.24031 - Make dumping/undumping more efficient - tfm
font_bc:=xmalloc_array(eight_bits, font_max);
font_ec:=xmalloc_array(eight_bits, font_max);
@y
font_bc:=xmalloc_array(UTF16_code, font_max);
font_ec:=xmalloc_array(UTF16_code, font_max);
@z

@x [50.1322] l.24031 - Make dumping/undumping more efficient - tfm
undump_things(font_check[null_font], font_ptr+1-null_font);
@y
for k:=null_font to font_ptr do font_mapping[k]:=0;
undump_things(font_check[null_font], font_ptr+1-null_font);
@z

@x [51.1332] l.24203 - make the main program a procedure, for eqtb hack.
  setup_bound_var (15000)('max_strings')(max_strings);
@y
  setup_bound_var (15000)('max_strings')(max_strings);
  max_strings:=max_strings+too_big_char; {the |max_strings| value doesn't include the 64K synthetic strings}
@z

@x [51.1332] l.24203 - make the main program a procedure, for eqtb hack.
  buffer:=xmalloc_array (ASCII_code, buf_size);
@y
  buffer:=xmalloc_array (UnicodeScalar, buf_size);
@z

@x [51.1332] l.24203 - make the main program a procedure, for eqtb hack.
  input_file:=xmalloc_array (alpha_file, max_in_open);
@y
  input_file:=xmalloc_array (unicode_file, max_in_open);
@z

@x [51.1332] l.24203 (ca.) texarray
  line_stack:=xmalloc_array (integer, max_in_open);
@y
  line_stack:=xmalloc_array (integer, max_in_open);
  eof_seen:=xmalloc_array (boolean, max_in_open);
  grp_stack:=xmalloc_array (save_pointer, max_in_open);
  if_stack:=xmalloc_array (pointer, max_in_open);
@z

@x [51.1333] l.24254 - Print new line before termination; switch to editor if necessary.
    print_file_name(0, log_name, 0); print_char(".");
@y
    print(log_name); print_char(".");
@z

@x [51.1337] l.24371 (ca.) texarray
  trie_root:=0; trie_c[0]:=si(0); trie_ptr:=0;
@y
  trie_root:=0; trie_c[0]:=si(0); trie_ptr:=0;
  hyph_root:=0; hyph_start:=0;
@z

@x [51.1337] l.24371 - Allocate hyphenation tries, do char translation, MLTeX
  {Allocate and initialize font arrays}
@y
  {Allocate and initialize font arrays}
  font_mapping:=xmalloc_array(void_pointer, font_max);
  font_layout_engine:=xmalloc_array(void_pointer, font_max);
  font_flags:=xmalloc_array(char, font_max);
  font_letter_space:=xmalloc_array(scaled, font_max);
@z

@x [51.1337] l.24371 - Allocate hyphenation tries, do char translation, MLTeX
  font_bc:=xmalloc_array(eight_bits, font_max);
  font_ec:=xmalloc_array(eight_bits, font_max);
@y
  font_bc:=xmalloc_array(UTF16_code, font_max);
  font_ec:=xmalloc_array(UTF16_code, font_max);
@z

@x [51.1337] l.24371 - Allocate hyphenation tries, do char translation, MLTeX
  param_base[null_font]:=-1;
@y
  font_mapping[null_font]:=0;
  param_base[null_font]:=-1;
@z

@x [53.1348] (do_extension) Remove unused variables
var k:integer; {all-purpose integers}
@y
var i,@!j,@!k:integer; {all-purpose integers}
@z

@x [53.1370] l.24779 - system: (write_out) \write18{foo} => system(foo).
    print(so(str_pool[str_start[str_ptr]+d])); {N.B.: not |print_char|}
@y
    print(so(str_pool[str_start_macro(str_ptr)+d])); {N.B.: not |print_char|}
@z

@x [53.1370] l.24779 - system: (write_out) \write18{foo} => system(foo).
        str_pool[str_start[str_ptr]+d]:=xchr[str_pool[str_start[str_ptr]+d]];
        if (str_pool[str_start[str_ptr]+d]=null_code)
@y
      if (str_pool[str_start_macro(str_ptr)+d]=null_code)
@z

@x [53.1370] l.24779 - convert from UTF-16 to UTF-8 for system(3).
      runsystem_ret := runsystem(conststringcast(addressof(
                                              str_pool[str_start[str_ptr]])));
@y
      if name_of_file then libc_free(name_of_file);
      name_of_file := xmalloc(cur_length * 3 + 2);
      k := 0;
      for d:=0 to cur_length-1 do
        append_to_name(str_pool[str_start_macro(str_ptr)+d]);
      name_of_file[k+1] := 0;
      runsystem_ret := runsystem(conststringcast(name_of_file+1));
@z

@x [53.1370] l.24779 - system: (write_out) \write18{foo} => system(foo).
  pool_ptr:=str_start[str_ptr];  {erase the string}
@y
  pool_ptr:=str_start_macro(str_ptr);  {erase the string}
@z

@x [53a.1379] l.??? -etex command line switch
@!init if (buffer[loc]="*")and(format_ident=" (INITEX)") then
@y
@!init if (etex_p or(buffer[loc]="*"))and(format_ident=" (INITEX)") then
@z

@x [53a.1379] l.??? -etex command line switch
  incr(loc); eTeX_mode:=1; {enter extended mode}
@y
  if buffer[loc]="*" then incr(loc);
  eTeX_mode:=1; {enter extended mode}
@z

@x [53a.1383] l.??? -etex command line switch
@!eTeX_mode: 0..1; {identifies compatibility and extended mode}
@y
@!eTeX_mode: 0..1; {identifies compatibility and extended mode}
@!etex_p: boolean; {was the -etex option specified}
@z

@x [53a.1391] l.??? texarray
@!eof_seen : array[1..max_in_open] of boolean; {has eof been seen?}
@y
@!eof_seen : ^boolean; {has eof been seen?}
@z

@x [53a.1506] l.??? texarray
@!grp_stack : array[0..max_in_open] of save_pointer; {initial |cur_boundary|}
@!if_stack : array[0..max_in_open] of pointer; {initial |cond_ptr|}
@y
@!grp_stack : ^save_pointer; {initial |cur_boundary|}
@!if_stack : ^pointer; {initial |cond_ptr|}
@z

@x [54.1376] l.24903 - print_csnames
      for c := str_start[text(h)] to str_start[text(h) + 1] - 1
@y
      for c := str_start_macro(text(h)) to str_start_macro(text(h) + 1) - 1
@z

@x [54.1376] l.24903 - Dumping/Undumping
@ Dumping the |xord|, |xchr|, and |xprn| arrays.  We dump these always
in the format, so a TCX file loaded during format creation can set a
default for users of the format.

@<Dump |xord|, |xchr|, and |xprn|@>=
dump_things(xord[0], 256);
dump_things(xchr[0], 256);
dump_things(xprn[0], 256);

@ Undumping the |xord|, |xchr|, and |xprn| arrays.  This code is more
complicated, because we want to ensure that a TCX file specified on
the command line will override whatever is in the format.  Since the
tcx file has already been loaded, that implies throwing away the data
in the format.  Also, if no |translate_filename| is given, but
|eight_bit_p| is set we have to make all characters printable.

@<Undump |xord|, |xchr|, and |xprn|@>=
if translate_filename then begin
  for k:=0 to 255 do undump_things(dummy_xord, 1);
  for k:=0 to 255 do undump_things(dummy_xchr, 1);
  for k:=0 to 255 do undump_things(dummy_xprn, 1);
  end
else begin
  undump_things(xord[0], 256);
  undump_things(xchr[0], 256);
  undump_things(xprn[0], 256);
  if eight_bit_p then
    for k:=0 to 255 do
      xprn[k]:=1;
end;


@y
@z

@x
  while s>255 do  {first 256 strings depend on implementation!!}
@y
  while s>65535 do  {first 64K strings don't really exist in the pool!}
@z

@x
@!mltex_enabled_p:boolean;  {enable character substitution}
@y
@!mltex_enabled_p:boolean;  {enable character substitution}
@!native_font_type_flag:integer; {used by XeTeX font loading code to record which font technology was used}
@!xtx_ligature_present:boolean; {to suppress tfm font mapping of char codes from ligature nodes (already mapped)}
@z

@x [54/web2c.???] l.??? needed earlier
replacement, but always existing character |font_bc[f]|.
@^inner loop@>

@<Declare additional functions for ML\TeX@>=
function effective_char(@!err_p:boolean;
@y
replacement, but always existing character |font_bc[f]|.
@^inner loop@>

@<Declare \eTeX\ procedures for sc...@>=
function effective_char(@!err_p:boolean;
@z

@x
begin result:=c;  {return |c| unless it does not exist in the font}
@y
begin if (not xtx_ligature_present) and (font_mapping[f]<>nil) then
  c:=apply_tfm_font_mapping(font_mapping[f],c);
xtx_ligature_present:=false;
result:=c;  {return |c| unless it does not exist in the font}
@z

@x
begin if not mltex_enabled_p then
@y
begin if (not xtx_ligature_present) and (font_mapping[f]<>nil) then
  c:=apply_tfm_font_mapping(font_mapping[f],c);
xtx_ligature_present:=false;
if not mltex_enabled_p then
@z

@x
effective_char_info:=null_character;
exit:end;
@y
effective_char_info:=null_character;
exit:end;
@#
@<Declare subroutines for |new_character|@>@;
@z
