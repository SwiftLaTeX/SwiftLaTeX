using System;

namespace NGraphite
{
	public class Segment : IDisposable
	{
		IntPtr _segment;

		internal Segment(IntPtr segment)
		{
			_segment = segment;
		}

		internal IntPtr SegPtr
		{
			get { return _segment; }
		}

		public float AdvanceX
		{
			get { return Graphite2Api.SegAdvanceX(_segment); }
		}

		public float AdvanceY
		{
			get { return Graphite2Api.SegAdvanceY(_segment); }
		}

		public uint NCinfo
		{
			get { return Graphite2Api.SegNCinfo(_segment); }
		}

		public Slot FirstSlot
		{
			get
			{
				IntPtr slotPtr = Graphite2Api.SegFirstSlot(_segment);
				if (slotPtr == IntPtr.Zero)
					return null;
				return new Slot(slotPtr);
			}
		}

		public Slot LastSlot
		{
			get
			{
				IntPtr slotPtr = Graphite2Api.SegLastSlot(_segment);
				if (slotPtr == IntPtr.Zero)
					return null;
				return new Slot(slotPtr);
			}
		}

		public CharInfo Cinfo(uint index)
		{
			IntPtr charInfoPtr = Graphite2Api.SegCinfo(_segment, index);
			if (charInfoPtr == IntPtr.Zero)
				return null;

			return new CharInfo(charInfoPtr);
		}

		public void Justify(Slot start, Font font, double width, JustFlags flags, Slot first, Slot last)
		{
			IntPtr firstPtr = first != null ? first.SlotPtr : IntPtr.Zero;
			IntPtr lastPtr = last != null ? last.SlotPtr : IntPtr.Zero;
			Graphite2Api.SegJustify(_segment, start.SlotPtr, font.FontPtr, width, flags, firstPtr, lastPtr);
		}

		#region IDisposable implementation
		public void Dispose()
		{
			Graphite2Api.SegDestroy(_segment);
		}
		#endregion
	}
}
