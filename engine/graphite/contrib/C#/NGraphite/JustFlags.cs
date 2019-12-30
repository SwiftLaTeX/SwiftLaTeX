using System;

namespace NGraphite
{
	public enum JustFlags
	{
		// Indicates that the start of the slot list is not at the start of a line
		JustStartInline = 1,
		// Indicates that the end of the slot list is not at the end of a line
		JustEndInline = 2
	}
}

