using System;
using NUnit.Framework;
using NGraphite;

namespace NGraphiteTests
{
	[TestFixture()]
	public class FeaturevalTests
	{
		Face _face;
		
		[SetUp]
		public void PerTestSetup()
		{
			_face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default);
		}
		
		[TearDown]
		public void PerTestTearDown()
		{
			_face.Dispose();
		}
		
		[Test()]
		public void Clone_TestFeatureVal_ReturnsNonNullFeatureValWhichIsDifferentInstanceToClonedObject()
		{
			using(Featureval val = _face.FeaturevalForLang("en"))
			{
				Featureval clonedVal = val.Clone();
				Assert.NotNull(clonedVal);
				Assert.AreNotSame(clonedVal, val);
			}
		}
	}
}

