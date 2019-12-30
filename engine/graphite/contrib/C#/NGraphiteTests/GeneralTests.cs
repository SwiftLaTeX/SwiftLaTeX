using NUnit.Framework;
using NGraphite;
using System.Collections.Generic;

namespace NGraphiteTests
{
	[TestFixture()]
	public class GeneralTests
	{
		[Test()]
		public void EnumerateAllFeaturesInAFont()
		{
			var idDescDictionary = new Dictionary<string, string>();
			
			using (var face = new Face(TestConstants.PaduakFontLocation, FaceOptions.face_default))
			{
				for(int i = 0; i < face.NFref(); i++)
				{
					var featureRef = face.Fref((ushort)i);					
					var refLabel = featureRef.Label("en", Encform.Utf16);
					
					idDescDictionary.Add(featureRef.IdLabel, refLabel);
				}
			}
			
			string val;
			idDescDictionary.TryGetValue("kdot", out val);
			Assert.AreEqual("Khamti style dots", val);
			
			idDescDictionary.TryGetValue("nnya", out val);
			Assert.AreEqual("Disable great nnya", val);
			
		}
	}
}

