using System;
using NUnit.Framework;
using NGraphite;

namespace NGraphiteTests
{
	[TestFixture()]
	public class FeatureRefTests
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
		public void FeatureValue_OnTestFeature_ReturnsZero()
		{
			using(Featureval val = _face.FeaturevalForLang("en"))
			{
				FeatureRef testFeature = CreateTestFeatureRef();
				Assert.AreEqual(0, testFeature.FeatureValue(val));
			}
		}
		
		[Test()]
		public void SetFeatureValue_SetValueInRange_ReturnsSuccessAndValueIsChanged()
		{
			using(Featureval val = _face.FeaturevalForLang("en"))
			{
				FeatureRef testFeature = CreateTestFeatureRef();
				int ret = testFeature.SetFeatureValue(1, val);
				Assert.AreEqual(1, ret);
				Assert.AreEqual(1, testFeature.FeatureValue(val));
			}
		}
		
		[Test()]
		public void Id_OnTestFeature_ReturnsExpectedId()
		{
			Assert.AreEqual(1801744244, CreateTestFeatureRef().Id);
		}
		
		[Test()]
		public void NValues_OnTestFeature_ReturnsExpectedNumberOfValues()
		{
			Assert.AreEqual(2, CreateTestFeatureRef().NValues());
		}
		
		[Test()]
		public void Value_OnTestFeature_ReturnsExpectedValueAssociatedWithFeature()
		{
			Assert.AreEqual(1, CreateTestFeatureRef().Value(1));
		}
		
		[Test()]
		public void Label_asUtf8_ExpectedStringReturn()
		{			
			string label = CreateTestFeatureRef().Label("en", Encform.Utf8);
			Assert.AreEqual("Khamti style dots", label);
		}
				
		[Test()]
		public void Label_asUtf16_ExpectedStringReturn()
		{			
			string label = CreateTestFeatureRef().Label("en", Encform.Utf16);
			Assert.AreEqual("Khamti style dots", label);
		}
		
		[Test()]
		public void IdLabel()
		{
			Assert.AreEqual("kdot", CreateTestFeatureRef().IdLabel);
		}
		
		[Test()]
		public void ValueLabel_OnTestFeature_ReturnsFalseString()
		{
			Assert.AreEqual("False", CreateTestFeatureRef().ValueLabel(0, "en", Encform.Utf8));
		}
		
		#region test helpers
		
		FeatureRef CreateTestFeatureRef()
		{			
			return _face.FindFref(1801744244);
		}
		
		#endregion
	}
}

