using System;
using NUnit.Framework;
using NGraphite;

namespace NGraphiteTests
{
	[TestFixture()]
	public class CharInfoTests
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
		public void UnicodeChar_OnFirstCharInfoInTestSegment_ReturnsExpectedChar()
		{
			Assert.AreEqual(104, CreateTestCharInfo(0).UnicodeChar);
		}
		
		[Test()]
		public void BreakWeight_OnFirstCharInfoInTestSegment_ReturnsExpectedValue()
		{
			Assert.AreEqual(-30, CreateTestCharInfo(0).BreakWeight);
		}
		
		[Test()]
		public void After_OnFirstCharInfoInTestSegment_ReturnsZero()
		{
			Assert.AreEqual(0, CreateTestCharInfo(0).After);
		}
		
		[Test()]
		public void Before_OnFirstCharInfoInTestSegment_ReturnsZero()
		{
			Assert.AreEqual(0, CreateTestCharInfo(0).Before);
		}
		
		[Test()]
		public void Base_OnFirstCharInfoInTestSegment_ExpectedIndex()
		{
			Assert.AreEqual(0, CreateTestCharInfo(0).Base);
		}
		
		#region Test Helpers
		
		protected CharInfo CreateTestCharInfo(uint index)
		{
			return _segment.Cinfo(index);
		}
		
		#endregion
		
	}
}

