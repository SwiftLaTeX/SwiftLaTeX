using System;
using NUnit.Framework;
using NGraphite;

namespace NGraphiteTests
{
	[TestFixture()]
	public class SegmentTests
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
		public void AdvanceX_OnTestSegment_ReturnsExpectedAdvance()
		{
			Assert.AreEqual(90.234375f, _segment.AdvanceX);
		}
		
		[Test()]
		public void AdvanceY_OnTestSegment_ReturnsExpectedAdvance()
		{
			Assert.AreEqual(0.0f, _segment.AdvanceY);
		}
		
		[Test()]
		public void NCinfo_OnTestSegment_ReturnsExpectedNumber()
		{
			Assert.AreEqual(11, _segment.NCinfo);
		}
		
		[Test()]
		public void Cinfo_OnTestSegment_ReturnsNonNullCharInfo()
		{
			CharInfo charInfo = _segment.Cinfo(0);
			Assert.IsNotNull(charInfo);
		}
		
		[Test()]
		public void FirstSlot_OnTestSegment_ReturnsNonNullSlot()
		{
			Slot slot = _segment.FirstSlot;
			Assert.NotNull(slot);
		}
		
		[Test()]
		public void LastSlot_OnTestSegment_ReturnsNonNullSlot()
		{
			Slot slot = _segment.LastSlot;
			Assert.NotNull(slot);
		}
		
		[Test()]
		public void Justify_SpecifyNullForFirstAndLast_DoesNotThrowException()
		{
			Slot firstSlot = _segment.FirstSlot;
			_segment.Justify(firstSlot, _font, 100, JustFlags.JustEndInline, null, null);
		}
		
		[Test()]
		public void Justify_SpecifyNonNullFirstAndLast_DoesNotThrowException()
		{
			Slot firstSlot = _segment.FirstSlot;
			Slot lastSlot = _segment.LastSlot;
			_segment.Justify(firstSlot, _font, 100, JustFlags.JustEndInline, firstSlot, lastSlot);
		}
	}
}

