using System;
using System.Runtime.InteropServices;
using System.Text;

namespace NGraphite
{
	public class Graphite2Api
	{
		public Graphite2Api()
		{
		}

		#region Font methods
		/// <summary>
		/// Returns version information on this engine
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_engine_version")]
		internal static extern void EngineVersion(out int nMajor, out int nMinor, out int nBugFix);

		/// <summary>
		/// type describing function to retrieve font table information
		/// </summary>
		internal delegate /*gr_face*/ IntPtr GetTableFn(IntPtr appFaceHandle, uint name, out int len);

		/// <summary>
		/// Create a gr_face object given application information and a getTable function
		/// </summary>
		/// <returns>
		/// gr_face or NULL if the font fails to load for some reason.
		/// </returns>
		/// <param name='appFaceHandle'>
		/// This is application specific information that is passed to the getTable
  		/// function. The appFaceHandle must stay alive as long as the gr_face is alive.
		/// </param>
		/// <param name='getTable'>
		/// This function is called whenever graphite needs access to a table of data
		/// in the font.
		/// </param>
		/// <param name='faceOptions'>
		/// Bitfield describing various options. See enum gr_face_options for details.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_make_face")]
		internal static extern /*gr_face*/ IntPtr MakeFace(IntPtr appFaceHandle, GetTableFn getTable, uint faceOptions);

		// This only exists if GRAPHITE2_NSEGCACHE was not defined when graphite was compiled.
		/// <summary>
		/// Create a gr_face object given application information, with subsegmental caching support
		/// </summary>
		/// <returns>
		/// gr_face or NULL if the font fails to load.
		/// </returns>
		/// <param name='appFaceHandle'>
		/// is a pointer to application specific information that is passed to getTable.
  		/// This may not be NULL and must stay alive as long as the gr_face is alive.
		/// </param>
		/// <param name='getTable'>
		/// The function graphite calls to access font table data
		/// </param>
		/// <param name='segCacheMaxSize'>
		/// How large the segment cache is.
		/// </param>
		/// <param name='faceOptions'>
		/// Bitfield of values from enum gr_face_options
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_make_face_with_seg_cache")]
		internal static extern /*gr_face*/ IntPtr MakeFaceWithSegCache(IntPtr appFaceHandle, GetTableFn getTable, uint segCacheMaxSize, uint faceOptions);

		/// <summary>
		/// Convert a tag in a string into a gr_uint32
		/// </summary>
		/// <returns>
		/// gr_uint32 tag, zero padded
		/// </returns>
		/// <param name='str'>
		/// str a nul terminated string of which at most the first 4 characters are read
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_str_to_tag")]
		internal static extern UInt32 StrToTag(string str);

		/// <summary>
		/// Convert a gr_uint32 tag into a string
		/// </summary>
		/// <param name='tag'>
		///  contains the tag to convert
		/// </param>
		/// <param name='str'>
		/// is a pointer to a char array of at least size 4 bytes. The first 4 bytes of this array
  		/// will be overwritten by this function. No nul is appended.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_tag_to_str")]
		internal static extern void TagToStr(UInt32 tag, IntPtr str);

		/// <summary>
		/// Get feature values for a given language or default
		/// </summary>
		/// <returns>
		/// a copy of the default feature values for a given language. The application must call
  		/// gr_featureval_destroy() to free this object when done.
		/// </returns>
		/// <param name='pFace'>
		/// The font face to get feature values from
		/// </param>
		/// <param name='langname'>
		/// The language tag to get feature values for. If there is no such language or
  		/// langname is 0, the default feature values for the font are returned.
  		/// langname is right 0 padded and assumes lowercase. Thus the en langauge
  		/// would be 0x656E0000. Langname may also be space padded, thus 0x656E2020.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_face_featureval_for_lang")]
		internal static extern /*gr_feature_val */ IntPtr FaceFeaturevalForLang(/* gr_face */ IntPtr pFace, UInt32 langname);

		/// <summary>
		/// Get feature reference for a given feature id from a face
		/// </summary>
		/// <returns>
		/// a feature reference corresponding to the given id. This data is part of the gr_face and
		/// will be freed when the face is destroyed.
		/// </returns>
		/// <param name='pFace'>
		/// Font face to get information on.
		/// </param>
		/// <param name='featid'>
		/// Feature id tag to get reference to.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_face_find_fref")]
		internal static extern /* gr_feature_ref*/ IntPtr FaceFindFref(/* gr_face */ IntPtr pFace, Int32 featid);

		/// <summary>
		/// Returns number of feature references in a face
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_face_n_fref")]
		internal static extern UInt16 FaceNFref(/* gr_face */ IntPtr pFace);

		/// <summary>
		/// Returns feature reference at given index in face
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_face_fref")]
		internal static extern /* gr_feature_ref */ IntPtr FaceFref(/*gr_face*/ IntPtr pFace, UInt16 i);

		/// <summary>
		/// Return number of languages the face knows about
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_face_n_languages")]
		internal static extern ushort FaceNLanguages(/*gr_face*/ IntPtr pFace);

		/// <summary>
		/// Returns a language id corresponding to a language of given index in the face
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_face_lang_by_index")]
		internal static extern UInt32 FaceLangByIndex(/*gr_face*/ IntPtr pFace, UInt16 i);

		/// <summary>
		/// Destroy the given face and free its memory
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_face_destroy")]
		internal static extern void FaceDestroy(/*gr_face*/ IntPtr pFace);

		/// <summary>
		/// Returns the number of glyphs in the face
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_face_n_glyphs")]
		internal static extern short FaceNGlyphs(/*gr_face*/ IntPtr pFace);


		// Only exists if GRAPHITE2_NFILEFACE was not defined when libgraphite2 was compiled
		/// <summary>
		/// Create gr_face from a font file
		/// </summary>
		/// <returns>
		/// gr_face that accesses a font file directly. Returns NULL on failure.
		/// </returns>
		/// <param name='filename'>
		///  Full path and filename to font file
		/// </param>
		/// <param name='faceOptions'>
		/// Bitfile from enum gr_face_options to control face options.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_make_file_face")]
		internal static extern /*gr_face*/ IntPtr MakeFileFace(string filename, FaceOptions faceOptions);

		// Only exists if GRAPHITE2_NFILEFACE was not defined when libgraphite2 was compiled
		/// <summary>
		/// Create gr_face from a font file, with subsegment caching support.
		/// </summary>
		/// <returns>
		/// gr_face that accesses a font file directly. Returns NULL on failure.
		/// </returns>
		/// <param name='filename'>
		/// Full path and filename to font file
		/// </param>
		/// <param name='segCacheMaxSize'>
		/// Specifies how big to make the cache in segments.
		/// </param>
		/// <param name='faceOptions'>
		/// Bitfield from enum gr_face_options to control face options.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_make_file_face_with_seg_cache")]
		internal static extern /*gr_face*/ IntPtr MakeFileFaceWithSegCache(string filename, uint segCacheMaxSize, FaceOptions faceOptions);

		/// <summary>
		///  Create a font from a face
		/// </summary>
		/// <returns>
		/// gr_font Call font_destroy to free this font
		/// </returns>
		/// <param name='ppm'>
		/// Resolution of the font in pixels per em
		/// </param>
		/// <param name='face'>
		/// Face this font corresponds to. This must stay alive as long as the font is alive.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_make_font")]
		internal static extern /*gr_font*/ IntPtr MakeFont(float ppm, /*gr_face*/ IntPtr face);

		/// <summary>
		/// query function to find the hinted advance width of a glyph
		/// </summary>
		internal delegate float AdvanceFn(IntPtr appFontHandle, UInt16 glyphid);

		/// <summary>
		///  Creates a font with hinted advance width query function
		/// </summary>
		/// <returns>
		/// gr_font to be destroyed via font_destroy
		/// </returns>
		/// <param name='ppm'>
		/// size of font in pixels per em
		/// </param>
		/// <param name='appFontHandle'>
		/// font specific information that must stay alive as long as the font does
		/// </param>
		/// <param name='advance'>
		/// function to call with appFontHandle and glyphid to get horizontal advance in pixels.
		/// </param>
		/// <param name='face'>
		/// the face this font corresponds to. Must stay alive as long as the font does.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_make_font_with_advance_fn")]
		internal static extern /*gr_font*/ IntPtr MakeFontWithAdvanceFn(float ppm, IntPtr appFontHandle, AdvanceFn advance, /*gr_face*/IntPtr face);

		/// <summary>
		/// Free a font
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_font_destroy")]
		internal static extern void FontDestroy(/*gr_font*/ IntPtr font);

		/// <summary>
		/// get a feature value
		/// </summary>
		/// <returns>
		/// value of specific feature or 0 if any problems.
		/// </returns>
		/// <param name='pfeatureref'>
		///  gr_feature_ref to the feature
		/// </param>
		/// <param name='feats'>
		/// containing all the values
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_fref_feature_value")]
		internal static extern UInt16 FrefFeatureValue(/*gr_feature_ref*/ IntPtr pfeatureref, /*gr_feature_val*/IntPtr feats);

		/// <summary>
		/// set a feature value
		/// </summary>
		/// <returns>
		/// false if there were any problems (value out of range, etc.)
		/// </returns>
		/// <param name='pfeatureref'>
		/// gr_feature_ref to the feature
		/// </param>
		/// <param name='val'>
		/// value to set the feature to
		/// </param>
		/// <param name='pDest'>
		/// the gr_feature_val containing all the values for all the features
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_fref_set_feature_value")]
		internal static extern int FrefSetFeatureValue(/*gr_feature_ref*/ IntPtr pfeatureref, UInt16 val, /*gr_feature_val*/IntPtr pDest);

		/// <summary>
		/// Returns the id tag for a gr_feature_ref
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_fref_id")]
		internal static extern UInt32 FrefId(/*gr_feature_ref*/ IntPtr pfeatureref);

		/// <summary>
		/// Returns number of values a feature may take, given a gr_feature_ref.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_fref_n_values")]
		internal static extern UInt16 FrefNValues(/*gr_feature_ref*/ IntPtr pfeatureref);

		/// <summary>
		/// Returns the value associated with a particular value in a feature
		/// </summary>
		/// <returns>
		/// value.
		/// </returns>
		/// <param name='pfeatureref'>
		///  gr_feature_ref of the feature of interest
		/// </param>
		/// <param name='settingno'>
		/// Index up to the return value of gr_fref_n_values() of the value
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_fref_value")]
		internal static extern UInt16 FrefValue(/*gr_feature_ref*/ IntPtr pfeatureref, UInt16 settingno);

		/// <summary>
		///  Returns a string of the UI name of a feature
		/// </summary>
		/// <returns>
		/// string of the UI name, in the encoding form requested. Call gr_label_destroy() after use.
		/// </returns>
		/// <param name='pfeatureref'>
		///  gr_feature_ref of the feature
		/// </param>
		/// <param name='langid'>
		/// This is a pointer since the face may not support a string in the requested
  		/// language. The actual language of the string is returned in langId
		/// </param>
		/// <param name='utf'>
		/// Encoding form for the string
		/// </param>
		/// <param name='length'>
		/// Used to return the length of the string returned in bytes.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_fref_label", CharSet = CharSet.Ansi)]
		internal static extern IntPtr FrefLabel(/*gr_feature_ref*/ IntPtr pfeatureref, ref UInt16 langid, Encform utf, out UInt32 length);

		/// <summary>
		/// Return a UI string for a possible value of a feature
		/// </summary>
		/// <returns>
		/// string of the UI name, in the encoding form requested. nul terminated. Call gr_label_destroy()
  		/// after use.
		/// </returns>
		/// <param name='pfeatureref'>
		/// gr_feature_ref of the feature
		/// </param>
		/// <param name='settingsno'>
		/// Value setting index
		/// </param>
		/// <param name='langId'>
		/// This is a pointer to the requested language. The requested language id is
  		/// replaced by the actual language id of the string returned.
		/// </param>
		/// <param name='utf'>
		///  Encoding form for the string
		/// </param>
		/// <param name='length'>
		/// Returns the length of the string returned in bytes.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_fref_value_label")]
		internal static extern IntPtr FrefValueLabel(/*gr_feature_ref*/ IntPtr pfeatureref, UInt16 settingsno, ref UInt16 langId, Encform utf, out UInt32 length);

		/// <summary>
		/// Destroy a previously returned label string
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_label_destroy")]
		internal static extern void LabelDestroy(IntPtr label);

		/// <summary>
		/// Copies a gr_feature_val
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_featureval_clone")]
		internal static extern /*gr_feature_val*/ IntPtr FeaturevalClone(/*gr_feature_val*/ IntPtr pfeatures);

		/// <summary>
		/// Destroys a gr_feature_val
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_featureval_destroy")]
		internal static extern void FeatureValDestroy(/*gr_feature_val*/ IntPtr pfeatures);

		#endregion

		#region Segment methods

		/// <summary>
		/// Returns Unicode character for a charinfo.
		/// </summary>
		/// <param name='p'>
		/// Pointer to charinfo to return information on.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_cinfo_unicode_char")]
		internal static extern UInt32 CinfoUnicodeChar(/*gr_char_info**/ IntPtr p/*not NULL*/);

		/// <summary>
		/// Returns breakweight for a charinfo.
		/// </summary>
		/// <returns>
		/// Breakweight is a number between -50 and 50 indicating the cost of a
		/// break before or after this character.
		/// </returns>
		/// <param name='p'>
		/// Pointer to charinfo to return information on.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_cinfo_break_weight")]
		internal static extern int CinfoBreakWeight(/*gr_char_info**/ IntPtr p/*not NULL*/);

		/// <summary>
		/// Returns the slot index that after this character is after in the slot stream
		/// </summary>
		/// <returns>
		/// after slot index between 0 and gr_seg_n_slots()
		/// </returns>
		/// <param name='p'>
		/// Pointer to charinfo to return information on.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_cinfo_after")]
		internal static extern int CinfoAfter(/*gr_char_info**/IntPtr p/*not NULL*/);

		/// <summary>
		/// Returns the slot index that before this character is before in the slot stream
		/// </summary>
		/// <returns>
		/// before slot index between 0 and gr_seg_n_slots()
		/// </returns>
		/// <param name='p'>
		/// Pointer to charinfo to return information on.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_cinfo_before")]
		internal static extern int CinfoBefore(/*gr_char_info**/IntPtr p/*not NULL*/);

		/// <summary>
		/// Returns the code unit index of this character in the input string
		/// </summary>
		/// <returns>
		/// code unit index between 0 and the end of the string
		/// </returns>
		/// <param name='p'>
		/// Pointer to charinfo to return information on.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_cinfo_base")]
		internal static extern int CinfoBase(/*gr_char_info**/IntPtr p/*not NULL*/);

		/// <summary>
		/// Returns the number of unicode characters in a string.
		/// </summary>
		/// <returns>
		/// number of characters in the string
		/// </returns>
		/// <param name='enc'>
		/// the type of data in the string: utf8, utf16, utf32
		/// </param>
		/// <param name='buffer_begin'>
		/// The start of the string
		/// </param>
		/// <param name='buffer_end'>
		/// Measure up to the first nul or when end is reached, whichever is earliest.
		/// This parameter may be NULL.
		/// </param>
		/// <param name='pError'>
		/// If there is a structural fault in the string, the location is returned
		/// in this variable. If no error occurs, pError will contain NULL. NULL
		/// may be passed for pError if no such information is required.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_count_unicode_characters")]
		internal static extern int CountUnicodeCharacters(Encform enc, IntPtr buffer_begin, IntPtr buffer_end, ref IntPtr pError);

		/// <summary>
		/// Creates and returns a segment.
		/// </summary>
		/// <returns>
		/// a segment that needs seg_destroy called on it. May return NULL if bad problems
		/// in segment processing.
		/// </returns>
		/// <param name='font'>
		/// Gives the size of the font in pixels per em for final positioning. If
		/// NULL, positions are returned in design units, i.e. at a ppm of the upem
		/// of the face.
		/// </param>
		/// <param name='face'>
		/// The face containing all the non-size dependent information.
		/// </param>
		/// <param name='script'>
		/// This is a tag containing a script identifier that is used to choose
		/// which graphite table within the font to use. Maybe 0. Tag may be 4 chars
		/// NULL padded in LSBs or space padded in LSBs.
		/// </param>
		/// <param name='pFeats'>
		/// Pointer to a feature values to be used for the segment. Only one
		/// feature values may be used for a segment. If NULL the default features
		/// for the font will be used.
		/// </param>
		/// <param name='enc'>
		/// Specifies what encoding form the string is in (utf8, utf16, utf32)
		/// </param>
		/// <param name='pStart'>
		/// Start of the string
		/// </param>
		/// <param name='nChars'>
		/// Number of unicode characters to process in the string. The string will
  		/// be processed either up to the first NULL or until nChars have been
  		/// processed. nChars is also used to initialise the internal memory
  		/// allocations of the segment. So it is wise not to make nChars too much
  		/// greater than the actual number of characters being processed.
		/// </param>
		/// <param name='dir'>
		/// Specifies whether the segment is processed right to left (1) or left to
		/// right (0) and whether to run the internal bidi pass, if a font requests it.
		/// See enum gr_bidirtl for details.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_make_seg")]
		internal static extern /*gr_segment**/IntPtr MakeSeg(/*gr_font**/IntPtr font, /*gr_face**/IntPtr face, UInt32 script, /*gr_feature_val**/IntPtr pFeats, Encform enc, IntPtr pStart, int nChars, Bidirtl dir);

		/// <summary>
		/// Destroys a segment, freeing the memory.
		/// </summary>
		/// <param name='p'>
		/// The segment to destroy
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_seg_destroy")]
		internal static extern void SegDestroy(/*gr_segment**/IntPtr p);

		/// <summary>
		/// Returns the advance for the whole segment.
		/// </summary>
		/// <returns>
		/// Returns the width of the segment up to the next glyph origin after the segment
		/// </returns>
		[DllImport("graphite2.dll", EntryPoint="gr_seg_advance_X")]
		internal static extern float SegAdvanceX(/*gr_segment**/IntPtr pSeg/*not NULL*/);

		/// <summary>
		/// Returns the height advance for the segment.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_seg_advance_Y")]
		internal static extern float SegAdvanceY(/*gr_segment**/IntPtr pSeg/*not NULL*/);

		/// <summary>
		/// Returns the number of gr_char_infos in the segment.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_seg_n_cinfo")]
		internal static extern uint SegNCinfo(/*gr_segment**/IntPtr pSeg/*not NULL*/);

		/// <summary>
		/// Returns a gr_char_info at a given index in the segment.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_seg_cinfo")]
		internal static extern /*gr_char_info**/IntPtr SegCinfo(/*gr_segment*/IntPtr pSeg/*not NULL*/, uint index/*must be <number_of_CharInfo*/);

		/// <summary>
		/// Returns the number of glyph gr_slots in the segment.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_seg_n_slots")]
		internal static extern uint SegNSlots(/*gr_segment**/IntPtr pSeg/*not NULL*/);      //one slot per glyph

		/// <summary>
		/// Returns the first gr_slot in the segment.
		///
		/// The first slot in a segment has a gr_slot_prev_in_segment() of NULL. Slots are owned
		/// by their segment and are destroyed along with the segment.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_seg_first_slot")]
		internal static extern /*gr_slot*/IntPtr SegFirstSlot(/*gr_segment**/IntPtr pSeg/*not NULL*/);    //may give a base slot or a slot which is attached to another

		/// <summary>
		/// Returns the last gr_slot in the segment.
		///
		/// The last slot in a segment has a gr_slot_next_in_segment() of NULL
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_seg_last_slot")]
		internal static extern /*gr_slot**/IntPtr SegLastSlot(/*gr_segment**/IntPtr pSeg/*not NULL*/);    //may give a base slot or a slot which is attached to another

		/// <summary>
		/// Justifies a linked list of slots for a line to a given width
		///
		/// Passed a pointer to the start of a linked list of slots corresponding to a line, as
		/// set up by gr_slot_linebreak_before, this function will position the glyphs in the line
		/// to take up the given width. It is possible to specify a subrange within the line to process.
		/// This allows skipping of line initial or final whitespace, for example. While this will ensure
		/// that the subrange fits width, the line will still be positioned with the first glyph of the
		/// line at 0. So the resulting positions may be beyond width.
		/// </summary>
		/// <param name='pSeg'>
		/// Pointer to the segment
		/// </param>
		/// <param name='pStart'>
		/// Pointer to the start of the line linked list (including skipped characters)
		/// </param>
		/// <param name='pFont'>
		/// Font to use for positioning
		/// </param>
		/// <param name='width'>
		/// Width in pixels in which to fit the line
		/// </param>
		/// <param name='flags'>
		/// Indicates line ending types. Default is linked list is a full line
		/// </param>
		/// <param name='pFirst'>
		/// If not NULL, the first slot in the list to be considered part of the line (so can skip)
		/// </param>
		/// <param name='pLast'>
		/// If not NULL, the last slot to process in the line (allow say trailing whitespace to be skipped)
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_seg_justify")]
		internal static extern void SegJustify(/*gr_segment**/IntPtr pSeg/*not NULL*/, /*gr_slot**/IntPtr pStart/*not NULL*/, /*gr_font **/ IntPtr pFont, double width, JustFlags flags, /*gr_slot**/IntPtr pFirst, /*gr_slot**/IntPtr pLast);

		/// <summary>
		/// Returns the next slot along in the segment.
		///
		/// Slots are held in a linked list. This returns the next in the linked list. The slot
		/// may or may not be attached to another slot. Returns NULL at the end of the segment.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_next_in_segment")]
		internal static extern /*gr_slot**/IntPtr SlotNextInSegment(/*gr_slot**/ IntPtr p);

		/// <summary>
		/// Returns the previous slot along in the segment.
		///
		/// Slots are held in a doubly linked list. This returns the previos slot in the linked
		/// list. This slot may or may not be attached to it. Returns NULL at the start of the
		/// segment.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_prev_in_segment")]
		internal static extern /*gr_slot**/IntPtr SlotPrevInSegment(/*gr_slot**/IntPtr p);

		/// <summary>
		/// Returns the attachment parent slot of this slot.
		///
		/// Attached slots form a tree. This returns the parent of this slot in that tree. A
		/// base glyph which is not attached to another glyph, always returns NULL.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_attached_to")]
		internal static extern /*gr_slot**/IntPtr SlotAttachedTo(/*gr_slot**/IntPtr p);

		/// <summary>
		/// Returns the first slot attached to this slot.
		///
		/// Attached slots form a singly linked list from the parent. This returns the first
		/// slot in that list. Note that this is a reference to another slot that is also in
		/// the main segment doubly linked list.
		///
		/// if gr_slot_first_attachment(p) != NULL then gr_slot_attached_to(gr_slot_first_attachment(p)) == p.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_first_attachment")]
		internal static extern /*gr_slot**/IntPtr SlotFirstAttachment(/*gr_slot**/IntPtr p);

		/// <summary>
		/// Returns the next slot attached to our attachment parent.
		///
		/// This returns the next slot in the singly linked list of slots attached to this
		/// slot's parent. If there are no more such slots, NULL is returned. If there is
		/// no parent, i.e. the passed slot is a base, then the next base in graphical order
		/// (ltr even for rtl text) is returned.
		///
		/// if gr_slot_next_sibling_attachment(p) != NULL then gr_slot_attached_to(gr_slot_next_sibling_attachment(p)) == gr_slot_attached_to(p).
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_next_sibling_attachment")]
		internal static extern /*gr_slot**/IntPtr SlotNextSiblingAttachment(/*gr_slot**/IntPtr p);

		/// <summary>
		/// Returns glyph id of the slot
		///
		/// Each slot has a glyphid which is rendered at the position given by the slot. This
		/// glyphid is the real glyph to be rendered and never a pseudo glyph.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_gid")]
		internal static extern ushort SlotGid(/*gr_slot**/IntPtr p);

		/// <summary>
		/// Returns X offset of glyph from start of segment
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_origin_X")]
		internal static extern float SlotOriginX(/*gr_slot**/IntPtr p);

		/// <summary>
		/// Returns Y offset of glyph from start of segment
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_origin_Y")]
		internal static extern float SlotOriginY(/*gr_slot**/IntPtr p);

		/// <summary>
		/// Returns the glyph advance for this glyph as adjusted for kerning
		/// </summary>
		/// <param name='p'>
		/// Slot to give results for
		/// </param>
		/// <param name='face'>
		/// face gr_face of the glyphs. May be NULL if unhinted advances used
		/// </param>
		/// <param name='font'>
		/// font gr_font to scale for pixel results. If NULL returns design units advance. If not NULL then returns pixel advance based on hinted or scaled glyph advances in the font. face must be passed for hinted advances to be used.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_advance_X")]
		internal static extern float SlotAdvanceX(/*gr_slot**/IntPtr p, /*gr_face**/IntPtr face, /*gr_font **/IntPtr font);

		/// <summary>
		/// Returns the vertical advance for the glyph in the slot adjusted for kerning
		///
		/// Returns design units unless font is not NULL in which case the pixel value is returned scaled for the given font
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_advance_Y")]
		internal static extern float SlotAdvanceY(/*gr_slot**/IntPtr p, /*gr_face**/IntPtr face, /*gr_font **/IntPtr font);

		/// <summary>
		/// Returns the gr_char_info index before us
		///
		/// Returns the index of the gr_char_info that a cursor before this slot, would put
		/// an underlying cursor before.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_before")]
		internal static extern int SlotBefore(/*gr_slot**/IntPtr p/*not NULL*/);

		/// <summary>
		/// Returns the gr_char_info index after us
		///
		/// Returns the index of the gr_char_info that a cursor after this slot would put an
		/// underlying cursor after.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_after")]
		internal static extern int SlotAfter(/*gr_slot**/IntPtr p/*not NULL*/);

		/// <summary>
		/// Returns the index of this slot in the segment
		///
		/// Returns the index given to this slot during final positioning. This corresponds to the value returned br gr_cinfo_before()
		/// and gr_cinfo_after()
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_index")]
		internal static extern uint SlotIndex(/*gr_slot**/IntPtr p/*not NULL*/);

		/// <summary>
		/// Return a slot attribute value
		///
		/// Given a slot and an attribute along with a possible subattribute, return the
		/// corresponding value in the slot. See enum gr_attrCode for details of each attribute.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_attr")]
		internal static extern int SlotAttr(/*gr_slot**/IntPtr p/*not NULL*/, /*gr_segment**/IntPtr pSeg/*not NULL*/, AttrCode index, byte subindex); //tbd - do we need to expose this?

		/// <summary>
		/// Returns whether text may be inserted before this glyph [check this isn't inverted]
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_can_insert_before")]
		internal static extern int SlotCanInsertBefore(/*gr_slot**/IntPtr p);

		/// <summary>
		/// Returns the original gr_char_info index this slot refers to.
		///
		/// Each Slot has a gr_char_info that it originates from. This is that gr_char_info. The index is passed to gr_seg_cinfo(). This
		/// information is useful for testing.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_original")]
		internal static extern int SlotOriginal(/*gr_slot**/IntPtr p/*not NULL*/);

		/// <summary>
		/// Breaks a segment into lines.
		///
		/// Breaks the slot linked list at the given point in the linked list. It is up
		/// to the application to keep track of the first slot on each line.
		/// </summary>
		[DllImport("graphite2.dll", EntryPoint="gr_slot_linebreak_before")]
		internal static extern void SlotLinebreakBefore(/*gr_slot **/ IntPtr p/*not NULL*/);

		#endregion

		#region Log methods

		/// <summary>
		/// Start logging all segment creation and updates on the provided face.  This
		/// is logged to a JSON file, see "Segment JSON Schema.txt" for a precise
		/// definition of the file
		/// </summary>
		/// <returns>
		/// true if the file was successfully created and logging is correctly initialised.
		/// </returns>
		/// <param name='face'>
		///  the gr_face whose segments you want to log to the given file
		/// </param>
		/// <param name='log_path'>
		/// a utf8 encoded file name and path to log to.
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_start_logging", CharSet = CharSet.Ansi)]
		internal static extern bool StartLogging(/*gr_face **/IntPtr face, string log_path);


		/// <summary>
		/// GStop logging on the given face.  This will close the log file created by
		/// gr_start_logging.
		/// </summary>
		/// <param name='face'>
		/// the gr_face whose segments you want to stop logging
		/// </param>
		[DllImport("graphite2.dll", EntryPoint="gr_stop_logging", CharSet = CharSet.Ansi)]
		internal static extern void StopLogging(/*gr_face **/IntPtr face);

		#endregion
	}
}
