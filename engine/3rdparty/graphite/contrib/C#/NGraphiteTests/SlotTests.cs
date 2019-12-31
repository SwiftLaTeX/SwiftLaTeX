using System;
using NUnit.Framework;
using NGraphite;

namespace NGraphiteTests
{
	[TestFixture()]
	public class SlotTests
	{
		Face _face;
		Font _font;
		Featureval _featureval;
		Segment _segment;
		
		[SetUp]
		public void PerTestSetup()
		{
			_face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default);	
			_font = new Font(20.0f, _face);
			_featureval = _face.FeaturevalForLang("en");
			_segment = _font.MakeSeg(0, _featureval, "hello world", Bidirtl.Nobidi);					
		}
		
		[TearDown]
		public void PerTestTearDown()
		{
			_face.Dispose();
			_font.Dispose();
			_featureval.Dispose();
			_segment.Dispose();
		}
		
		[Test()]
		public void NextInSegment_OnFirstSlotInTestSegment_ReturnsNonNullSlot()
		{
			Assert.NotNull(_segment.FirstSlot.NextInSegment);
		}
		
		[Test()]
		public void PrevInSegment_OnLastSlotInTestSegment_ReturnsNonNullSlot()
		{
			Assert.NotNull(_segment.LastSlot.PrevInSegment);
		}
		
		[Test()]
		public void AttachedTo_OnFirstSlotInTestSegment_ReturnsNullSlot()
		{
			Assert.IsNull(_segment.FirstSlot.AttachedTo);
		}
		
		[Test()]
		public void FirstAttachment_OnFirstSlotInTestSegment_ReturnsNullSlot()
		{
			Assert.IsNull(_segment.FirstSlot.FirstAttachment);
		}
		
		[Test()]
		public void NextSiblingAttachment_OnFirstSlotInTestSegment_ReturnsNonNullSlot()
		{
			Assert.IsNotNull(_segment.FirstSlot.NextSiblingAttachment);
		}
		
		[Test()]
		public void Gid_OnFirstSlotInTestSegment_ReturnsExpectedId()
		{
			Assert.AreEqual(75, _segment.FirstSlot.Gid);
		}
		
		[Test()]
		public void OriginX_OnFirstSlotInTestSegment_ReturnsExpectedValue()
		{
			Assert.AreEqual(0f, _segment.FirstSlot.OriginX);
		}
		
		[Test()]
		public void OriginY_OnFirstSlotInTestSegment_ReturnsExpectedValue()
		{
			Assert.AreEqual(0f, _segment.FirstSlot.OriginY);
		}
		
		[Test()]
		public void AdvanceX_OnFirstSlotInTestSegment_ReturnsExpectedValue()
		{
			Assert.AreEqual(9.9609375f, _segment.FirstSlot.AdvanceX(_face, _font));
		}
		
		[Test()]
		public void AdvanceY_OnFirstSlotInTestSegment_ReturnsExpectedValue()
		{
			Assert.AreEqual(0f, _segment.FirstSlot.AdvanceY(_face, _font));
		}
		
		[Test()]
		public void Before_OnFirstSlotInTestSegment_ReturnsZero()
		{
			Assert.AreEqual(0, _segment.FirstSlot.Before);
		}
		
		[Test()]
		public void After_OnFirstSlotInTestSegment_ReturnsZero()
		{
			Assert.AreEqual(0, _segment.FirstSlot.After);
		}
		
		[Test()]
		public void Index_OnFirstSlotInTestSegment_ReturnsZero()
		{
			Assert.AreEqual(0, _segment.FirstSlot.Index);
		}
		
		[Test()]
		public void Attr_OnFirstSlotInTestSegment_ReturnsExpectedAttributeValue()
		{
			Assert.AreEqual(510, _segment.FirstSlot.Attr(_segment, AttrCode.SlatAdvX, 0));
		}
		
		[Test()]
		public void CanInsertBefore_OnFirstSlotInTestSegment_ReturnsTrue()
		{
			Assert.IsTrue(_segment.FirstSlot.CanInsertBefore);
		}
		
		[Test()]
		public void Original_OnFirstSlotInTestSegment_ReturnsExpectedCharInfoValue()
		{
			Assert.AreEqual(0, _segment.FirstSlot.Original);
		}
		
		[Test()]
		public void LineBreakBefore_OnLastSlotInTestSegment_DoesNotThrowException()
		{
			// note: if we call LineBreakBefore on FirstSlot it graphite seg faults
			// eg: _segment.FirstSlot.LineBreakBefore();
			
			_segment.LastSlot.LineBreakBefore();
		}		
	}
}

