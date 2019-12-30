using System;
using System.Runtime.InteropServices;

namespace NGraphite
{
	public class Font : IDisposable
	{
		IntPtr _font;
		Face _face;
		
		internal IntPtr FontPtr
		{
			get { return _font; }
		}
		
		public Font(float ppm, Face face)
		{
			_face = face;
			_font = Graphite2Api.MakeFont(ppm, _face.FacePtr);
		}
		
		public Segment MakeSeg(UInt32 script, Featureval feats, string str, Bidirtl dir)
		{
			IntPtr strPtr = Marshal.StringToCoTaskMemUni(str);
			IntPtr error = new IntPtr(0);
			int numCodePoints = Graphite2Api.CountUnicodeCharacters(Encform.Utf16, strPtr, IntPtr.Zero, ref error);
			IntPtr segmentPtr = Graphite2Api.MakeSeg(_font, _face.FacePtr, script, feats.FeatureValPtr, Encform.Utf16, strPtr, numCodePoints, dir);
			Marshal.FreeCoTaskMem(strPtr);
			return new Segment(segmentPtr);
		}

		#region IDisposable implementation
		public void Dispose()
		{
			Graphite2Api.FontDestroy(_font);
		}
		#endregion
	}
}

