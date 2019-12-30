using System;

namespace NGraphite
{
	public enum BreakWeight
	{
			BreakNone = 0,
			/* after break weights */
			BreakWhiteSpace = 10,
			BreakWord = 15,
			BreakIntra = 20,
			BreakLetter = 30,
			BreakClip = 40,
			/* before break weights */
			BreakBeforeWhitespace = -10,
			BreakBeforeWord = -15,
			BreakBeforeIntra = -20,
			BreakBeforeLetter = -30,
			BreakBeforeClip = -40
	}
}

