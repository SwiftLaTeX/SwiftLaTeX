using System;

namespace NGraphite
{
	/// <summary>
	/// Used for looking up slot attributes. Most are already available in other functions
	/// </summary>
	public enum AttrCode
	{
		/// adjusted glyph advance in x direction in design units
		SlatAdvX = 0,
		/// adjusted glyph advance in y direction (usually 0) in design units
		SlatAdvY,
		/// returns 0. Deprecated.
		SlatAttTo,
		/// This slot attaches to its parent at the given design units in the x direction
		SlatAttX,
		/// This slot attaches to its parent at the given design units in the y direction
		SlatAttY,
		/// This slot attaches to its parent at the given glyph point (not implemented)
		SlatAttGpt,
		/// x-direction adjustment from the given glyph point (not implemented)
		SlatAttXOff,
		/// y-direction adjustment from the given glyph point (not implemented)
		SlatAttYOff,
		/// Where on this glyph should align with the attachment point on the parent glyph in the x-direction.
		SlatAttWithX,
		/// Where on this glyph should align with the attachment point on the parent glyph in the y-direction
		SlatAttWithY,
		/// Which glyph point on this glyph should align with the attachment point on the parent glyph (not implemented).
		SlatWithGpt,
		/// Adjustment to gr_slatWithGpt in x-direction (not implemented)
		SlatAttWithXOff,
		/// Adjustment to gr_slatWithGpt in y-direction (not implemented)
		SlatAttWithYOff,
		/// Attach at given nesting level (not implemented)
		SlatAttLevel,
		/// Line break breakweight for this glyph
		SlatBreak,
		/// Ligature component reference (not implemented)
		SlatCompRef,
		/// bidi directionality of this glyph (not implemented)
		SlatDir,
		/// Whether insertion is allowed before this glyph
		SlatInsert,
		/// Final positioned position of this glyph relative to its parent in x-direction in pixels
		SlatPosX,
		/// Final positioned position of this glyph relative to its parent in y-direction in pixels
		SlatPosY,
		/// Amount to shift glyph by in x-direction design units
		SlatShiftX,
		/// Amount to shift glyph by in y-direction design units
		SlatShiftY,
		/// attribute user1
		SlatUserDefnV1,
		/// not implemented
		SlatMeasureSol,
		/// not implemented
		SlatMeasureEol,
		/// Amount this slot can stretch (not implemented)
		SlatJStretch,
		/// Amount this slot can shrink (not implemented)
		SlatJShrink,
		/// Granularity by which this slot can stretch or shrink (not implemented)
		SlatJStep,
		/// Justification weight for this glyph (not implemented)
		SlatJWeight,
		/// Amount this slot mush shrink or stretch in design units
		SlatJWidth,
		/// SubSegment split point
		SlatSegSplit = SlatJStretch + 29,
		/// User defined attribute, see subattr for user attr number
		SlatUserDefn,

		/// not implemented
		SlatMax,
		/// not implemented
		SlatNoEffect = SlatMax + 1
	}
}
