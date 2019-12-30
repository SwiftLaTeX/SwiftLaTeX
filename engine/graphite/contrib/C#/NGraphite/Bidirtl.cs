using System;

namespace NGraphite
{
	public enum Bidirtl
	{
		/// Underlying paragraph direction is RTL
		Rtl = 1,
		/// Set this to not run the bidi pass internally, even if the font asks for it.
		/// This presumes that the segment is in a single direction.
		Nobidi = 2,
		/// Disable auto mirroring for rtl text
		Nomirror = 4
	}
}
