using System;
using NUnit.Framework;
using NGraphite;
using System.IO;

namespace NGraphiteTests
{
	[TestFixture()]
	public class FaceTests
	{
		[Test()]
		public void Face_ConstructPaduakFaceInstance_DoesNotThrowException()
		{
			new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default).Dispose();
		}
		
		[Test()]
		public void Face_ConstructPaduakFaceInstanceWithSegCache_DoesNotThrowException()
		{
			new Face(TestConstants.PaduakFontLocation, 100, FaceOptions.face_default).Dispose();
		}
		
		[Test()]
		public void Face_ConstructWithNonGraphiteFont_ThrowsArgumentException()
		{
			Assert.Throws<ArgumentException>(() => new Face(TestConstants.DejaVuSansLocation, FaceOptions.face_default));
		}
		
		[Test()]
		public void Face_ConstructWithNonInvalidFileName_ThrowsArgumentException()
		{
			Assert.Throws<ArgumentException>(() => new Face(TestConstants.DejaVuSansLocation, FaceOptions.face_default));
		}
		
		[Test()]
		public void Face_ConstructWithNonGraphiteFontWithSegCache_ThrowsArgumentException()
		{
			Assert.Throws<ArgumentException>(() => new Face(TestConstants.DejaVuSansLocation, 50, FaceOptions.face_default));
		}
		
		[Test()]
		public void Face_ConstructWithNonInvalidFileNameWithSegCache_ThrowsArgumentException()
		{
			Assert.Throws<ArgumentException>(() => new Face(TestConstants.DejaVuSansLocation, 50, FaceOptions.face_default));
		}
		
		[Test()]
		public void IsGraphiteFont_WithGraphiteFont_ReturnsTrue()
		{
			Assert.IsTrue(Face.IsGraphiteFont(TestConstants.PaduakFontLocation));
		}
		
		[Test()]
		public void IsGraphiteFont_WithNonGraphiteFont_ReturnsFalse()
		{
			Assert.IsFalse(Face.IsGraphiteFont(TestConstants.DejaVuSansLocation));
		}
		
		[Test()]
		public void FeaturevalForLang_EnLang_ReturnsNonNullFeatureval()
		{
			using (var face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default))
			{
				Featureval val = face.FeaturevalForLang("en");
				Assert.NotNull(val);					
			}
		}
		
		[Test()]
		public void FindFref_FindFreatureForkdotString_ReturnsNonNullFeatureRef()
		{
			using (var face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default))
			{
				FeatureRef featureRef = face.FindFref(1801744244);
				Assert.NotNull(featureRef);
			}
		}
		
		[Test()]
		public void NFref_PaduakFace_ReturnNineOrMoreFeatures()
		{
			using (var face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default))
			{
				Assert.GreaterOrEqual(face.NFref(), 9);
			}
		}
		
		[Test()]
		public void Fref_PaduakFace_ReturnNonNullFeatureRef()
		{
			using (var face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default))
			{
				FeatureRef featureRef = face.Fref(0);
				Assert.NotNull(featureRef);
			}
		}
		
		[Test()]
		public void NLanguages_PaduakFace_ReturnsExpectedNumberOfLanguages()
		{
			using (var face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default))
			{
				Assert.AreEqual(3, face.NLanguages());
			}
		}
		
		[Test()]
		public void LangByIndex_PaduakFaceIndexZero_ReturnsNonZeroLangId()
		{
			using (var face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default))
			{
				Assert.AreNotEqual(0, face.LangByIndex(0));	
			}
		
		}
		
		[Test()]
		public void NGlyphs_PaduakFace_ReturnsExpectedNumberOfGlyphs()
		{
			using (var face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default))
			{
				Assert.AreEqual(445, face.NGlyphs());
			}
		}
		
		[Test()]
		public void StartLogging_PaduakFace_LogFileIsCreated()
		{
			int major, minor, bugfix;
			Graphite2Api.EngineVersion(out major, out minor, out bugfix);
			
			if (major <= 1 && minor < 2)
			{
				using (var face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default))
				{
					Assert.Throws<NotSupportedException>(() => face.StartLogging(String.Empty));
					Assert.Throws<NotSupportedException>(() => face.StopLogging());
				}
				return;
			}
			
			using (var face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default))
			{
				string filename = Path.GetTempPath() + Guid.NewGuid().ToString();
				Assert.IsTrue(face.StartLogging(filename));
				Assert.IsTrue(File.Exists(filename));
				face.StopLogging();
				File.Delete(filename);
			}
		}
	}
}

