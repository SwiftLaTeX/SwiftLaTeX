using System;

namespace NGraphite
{
	public class Slot
	{
		IntPtr _slot;
		
		internal Slot(IntPtr slot)
		{
			_slot = slot;
		}
		
		internal IntPtr SlotPtr
		{
			get { return _slot; }	
		}
		
		public Slot NextInSegment
		{
			get
			{ 				
				return CreateSlotOrReturnNull(Graphite2Api.SlotNextInSegment(_slot));
			}
		}
		
		public Slot PrevInSegment
		{
			get
			{				
				return CreateSlotOrReturnNull(Graphite2Api.SlotPrevInSegment(_slot));
			}
		}

		public Slot AttachedTo
		{
			get
			{				
				return CreateSlotOrReturnNull(Graphite2Api.SlotAttachedTo(_slot));
			}
		}
		
		public Slot FirstAttachment
		{
			get
			{				
				return CreateSlotOrReturnNull(Graphite2Api.SlotFirstAttachment(_slot));
			}
		}
		
		public Slot NextSiblingAttachment
		{
			get
			{
				return CreateSlotOrReturnNull(Graphite2Api.SlotNextSiblingAttachment(_slot));
			}
		}
		
		public ushort Gid
		{
			get
			{
				return Graphite2Api.SlotGid(_slot);	
			}
		}
		
		public float OriginX
		{
			get
			{
				return Graphite2Api.SlotOriginX(_slot);	
			}
		}
		
		public float OriginY
		{
			get
			{
				return Graphite2Api.SlotOriginY(_slot);	
			}
		}
		
		public int Before
		{
			get
			{
				return Graphite2Api.SlotBefore(_slot);	
			}			
		}
		
		public int After
		{
			get
			{
				return Graphite2Api.SlotAfter(_slot);	
			}
		}
		
		public uint Index
		{
			get
			{
				return Graphite2Api.SlotIndex(_slot);	
			}
		}
		
		public bool CanInsertBefore
		{
			get { return Graphite2Api.SlotCanInsertBefore(_slot) != 0; }	
		}
		
		public int Original
		{
			get { return Graphite2Api.SlotOriginal(_slot); }
		}
		
		public float AdvanceX(Face face, Font font)
		{
			return Graphite2Api.SlotAdvanceX(_slot, face.FacePtr, font.FontPtr);
		}
		
		public float AdvanceY(Face face, Font font)
		{
			return Graphite2Api.SlotAdvanceY(_slot, face.FacePtr, font.FontPtr);
		}
		
		public int Attr(Segment seg, AttrCode index, byte subindex)
		{
			return Graphite2Api.SlotAttr(_slot, seg.SegPtr, index, subindex);
		}
		
		public void LineBreakBefore()
		{
			Graphite2Api.SlotLinebreakBefore(_slot);	
		}
		
		#region Helper Methods
		/// <summary>
		/// Creates the slot or return null of slotPtr == IntPtr.Zero.
		/// </summary>
		/// <returns>
		/// a slot or null.
		/// </returns>
		/// <param name='slotPtr'>
		/// Slot ptr.
		/// </param>
		private Slot CreateSlotOrReturnNull(IntPtr slotPtr)
		{
			if (slotPtr == IntPtr.Zero)
				return null;
			return new Slot(slotPtr);
		}
		#endregion
	}
}

