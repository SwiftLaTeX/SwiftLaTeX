using System;
using NUnit.Framework;
using NGraphite;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;

namespace NGraphiteTests
{
	[TestFixture]
	public class Graphite2ApiTests
	{
		const string PaduakFontLocation = TestConstants.PaduakFontLocation;

		#region FONT tests
		[Test]
		public void EngineVersion_ReturnsSensibleVersionValues()
		{
			int major, minor, bugfix;
			Graphite2Api.EngineVersion(out major, out minor, out bugfix);
			Assert.AreEqual(1, major, "Major");
			if (major == 1)
				Assert.GreaterOrEqual(minor, 0, "Minor");
		}

		[Test]
		public void MakeFace_WithNullOpGetTableFn_ReturnedFaceIsNull()
		{
			Graphite2Api.GetTableFn returnFace = (IntPtr appFaceHandle, uint name, out int len) =>
			{
				len = 0;
				return IntPtr.Zero;
			};

			IntPtr gr_face = Graphite2Api.MakeFace(IntPtr.Zero,returnFace, 0);

			Assert.AreEqual(IntPtr.Zero, gr_face);
		}

		[Test]
		public void MakeFaceWithSegCache_WithNullOpGetTableFn_ReturnedFaceIsNull()
		{
			Graphite2Api.GetTableFn returnFace = (IntPtr appFaceHandle, uint name, out int len) =>
			{
				len = 0;
				return IntPtr.Zero;
			};

			IntPtr gr_face = Graphite2Api.MakeFaceWithSegCache(IntPtr.Zero,returnFace, 100, 0);

			Assert.AreEqual(IntPtr.Zero, gr_face);
		}

		[Test]
		public void StrToTag_VariousValues_ReturnsExpectedResults()
		{
			Assert.AreEqual(1181049204, Graphite2Api.StrToTag("Feat"), "Feat");
			Assert.AreEqual(0, Graphite2Api.StrToTag(String.Empty), "Empty String");
			// more the 4 chars should be ignored.
			Assert.AreEqual(1181049204, Graphite2Api.StrToTag("FeatOverflow"), "FeatOverflow");
		}

		[Test]
		public void TagToStr_PassFeatEncodedAsId_ReturnsPtrToFeatString()
		{
			IntPtr ptr = Marshal.StringToCoTaskMemAnsi("    ");
			Graphite2Api.TagToStr(1181049204, ptr);
			var str = Marshal.PtrToStringAnsi(ptr);
			Assert.AreEqual("Feat", str);
			Marshal.FreeCoTaskMem(ptr);
		}

		[Test]
		public void FaceFeaturevalForLang_PaduakFontWithEnLang_NonNullFeatureValReturned()
		{
			using(var face = new PaduakDisposableFace())
			{
				uint lang = Graphite2Api.StrToTag("en");
				IntPtr gr_feature_val =  Graphite2Api.FaceFeaturevalForLang(face.Face, lang);
				Assert.AreNotEqual(IntPtr.Zero, gr_feature_val);

				Graphite2Api.FeatureValDestroy(gr_feature_val);
			}
		}

		[Test]
		public void FaceFindFref_FindFreatureForkdotString_ReturnsNonNullFeatureRef()
		{
			using(var face = new PaduakDisposableFace())
			{
				IntPtr gr_feature_ref = Graphite2Api.FaceFindFref(face.Face, 1801744244);
				Assert.AreNotEqual(IntPtr.Zero, gr_feature_ref);
			}
		}

		[Test]
		public void FaceNFref_PaduakFace_ReturnNineOrMoreFeatures()
		{
			using(var face = new PaduakDisposableFace())
			{
				int num = Graphite2Api.FaceNFref(face.Face);
				Assert.GreaterOrEqual(num, 9);
			}
		}

		[Test]
		public void FaceFref_PaduakFace_ReturnsNonNullForEachFeature()
		{
			using(var face = new PaduakDisposableFace())
			{
				int num = Graphite2Api.FaceNFref(face.Face);
				for(ushort i = 0; i < num; ++i)
				{
					IntPtr gr_feature_ref = Graphite2Api.FaceFref(face.Face, i);
					Assert.AreNotEqual(IntPtr.Zero, gr_feature_ref);
				}
			}
		}

		[Test]
		public void FaceNLanguages_Paduak_ReturnsExpectedNumberOfLanguages()
		{
			using(var face = new PaduakDisposableFace())
			{
				Assert.AreEqual(3, Graphite2Api.FaceNLanguages(face.Face));
			}
		}

		[Test]
		public void FaceLangByIndex_IndexZero_ReturnsNonZeroLangId()
		{
			using(var face = new PaduakDisposableFace())
			{
				uint langId = Graphite2Api.FaceLangByIndex(face.Face, 0);
				Assert.AreNotEqual(0, langId);
			}
		}

		[Test]
		public void FaceDestroy_Null_NoExceptionThrown()
		{
			Graphite2Api.FaceDestroy(IntPtr.Zero);
		}

		[Test]
		public void FaceNGlyphs_PaduakFace_ReturnsExpectedNumber()
		{
			using(var face = new PaduakDisposableFace())
			{
				Assert.AreEqual(445, Graphite2Api.FaceNGlyphs(face.Face));
			}
		}

		[Test]
		public void MakeFileFace_UsingPadaukFont_NonNullFaceIsReturned()
		{
			IntPtr gr_face = Graphite2Api.MakeFileFace(PaduakFontLocation, 0);
			Assert.AreNotEqual(IntPtr.Zero, gr_face);

			Graphite2Api.FaceDestroy(gr_face);
		}

		[Test]
		public void MakeFileFaceWithSegCache_UsingPadaukFont_NonNullFaceIsReturned()
		{
			IntPtr gr_face = Graphite2Api.MakeFileFaceWithSegCache(PaduakFontLocation, 100, 0);
			Assert.AreNotEqual(IntPtr.Zero, gr_face);

			Graphite2Api.FaceDestroy(gr_face);
		}

		[Test]
		public void MakeFont_UsingPadaukFont_NonNullFontIsReturned()
		{
			IntPtr gr_face = Graphite2Api.MakeFileFace(PaduakFontLocation, 0);
			IntPtr gr_font = Graphite2Api.MakeFont(20, gr_face);

			Assert.AreNotEqual(IntPtr.Zero, gr_font);

			Graphite2Api.FontDestroy(gr_font);
			Graphite2Api.FaceDestroy(gr_face);
		}

		[Test]
		public void MakeFontWithAdvanceFn_UsingPadaukFont_NonNullFontIsReturned()
		{
			IntPtr gr_face = Graphite2Api.MakeFileFace(PaduakFontLocation, 0);

			Graphite2Api.AdvanceFn advance = (IntPtr appFontHandle, UInt16 glyphid) => 0;

			IntPtr gr_font = Graphite2Api.MakeFontWithAdvanceFn(20, IntPtr.Zero, advance, gr_face);

			Assert.AreNotEqual(IntPtr.Zero, gr_font);

			Graphite2Api.FontDestroy(gr_font);
			Graphite2Api.FaceDestroy(gr_face);
		}

		[Test]
		public void FontDestroy_Null_NoExceptionThrown()
		{
			Graphite2Api.FontDestroy(IntPtr.Zero);
		}

		[Test]
		public void FrefFeatureValue_FirstPaduakFeature_ReturnsZero()
		{
			using (var feature = new PaduakFeatureVal())
			{
				uint lang = Graphite2Api.StrToTag("en");
				IntPtr gr_feature_val =  Graphite2Api.FaceFeaturevalForLang(feature.Face, lang);

				UInt16 val = Graphite2Api.FrefFeatureValue(feature.FeatureRef, gr_feature_val);
				Assert.AreEqual(0, val);

				Graphite2Api.FeatureValDestroy(gr_feature_val);
			}
		}

		[Test]
		public void FrefSetFeatureValue_SettingsThekdotFeature_FailsWithZeroReturnCode()
		{
			using(var face = new PaduakDisposableFace())
			{
				uint lang = Graphite2Api.StrToTag("en");
				IntPtr gr_feature_val =  Graphite2Api.FaceFeaturevalForLang(face.Face, lang);
				IntPtr gr_feature_ref = Graphite2Api.FaceFindFref(face.Face, 1801744244);
				int ret = Graphite2Api.FrefSetFeatureValue(gr_feature_ref, 12, gr_feature_val);
				Assert.AreEqual(0, ret);

				Graphite2Api.FeatureValDestroy(gr_feature_val);
			}
		}

		[Test]
		public void FrefId_FirstPaduakFeature_ReturnsExpectedId()
		{
			using (var feature = new PaduakFeatureVal())
			{
				UInt32 id = Graphite2Api.FrefId(feature.FeatureRef);
				Assert.AreEqual(1801744244, id);
				IntPtr ptr = Marshal.StringToCoTaskMemAnsi("    ");
				Graphite2Api.TagToStr(id, ptr);
				var str = Marshal.PtrToStringAnsi(ptr);
				Assert.AreEqual("kdot", str);
				Marshal.FreeCoTaskMem(ptr);
			}
		}

		[Test]
		public void FrefNValues_FirstPaduakFeature_ReturnsTwo()
		{
			using (var feature = new PaduakFeatureVal())
			{
				int numval = Graphite2Api.FrefNValues(feature.FeatureRef);
				Assert.AreEqual(2, numval);
			}
		}

		[Test]
		public void FrefValue_FirstPaduakFeatureSettingOne_ReturnsOne()
		{
			using (var feature = new PaduakFeatureVal())
			{
				UInt16 val = Graphite2Api.FrefValue(feature.FeatureRef, 1);
				Assert.AreEqual(1, val);
			}
		}

		[Test]
		public void FrefLabel_FirstPaduakFeature_ReturnsExpectedString()
		{
			using (var feature = new PaduakFeatureVal())
			{
				ushort lang = (ushort)Graphite2Api.StrToTag("en");
				UInt32 length;

				IntPtr labelPtr = Graphite2Api.FrefLabel(feature.FeatureRef, ref lang, Encform.Utf8, out length);
				string label = Marshal.PtrToStringAnsi(labelPtr);
				Assert.AreEqual("Khamti style dots", label);

				Graphite2Api.LabelDestroy(labelPtr);
			}
		}

		[Test]
		public void FrefValueLabel_FirstPaduakFeature_ReturnsFalseString()
		{
			using (var feature = new PaduakFeatureVal())
			{
				uint length;
				ushort lang = (ushort)Graphite2Api.StrToTag("en");
				IntPtr labelPtr = Graphite2Api.FrefValueLabel(feature.FeatureRef, 0, ref lang, Encform.Utf8, out length);
				Assert.AreEqual("False", Marshal.PtrToStringAnsi(labelPtr));
				Assert.AreEqual(5, length);

				Graphite2Api.LabelDestroy(labelPtr);
			}
		}

		[Test]
		public void LabelDestroy_Null_DoesNotThrowException()
		{
			Graphite2Api.LabelDestroy(IntPtr.Zero);
		}

		[Test]
		public void FeaturevalClone_CloneAFeature_ReturnADifferentInstance()
		{
			using(var face = new PaduakDisposableFace())
			{
				uint lang = Graphite2Api.StrToTag("en");
				IntPtr one =  Graphite2Api.FaceFeaturevalForLang(face.Face, lang);
				IntPtr two =  Graphite2Api.FeaturevalClone(one);
				Assert.NotNull(one, "one");
				Assert.NotNull(two, "two");
				Assert.AreNotEqual(one, two);

				Graphite2Api.FeatureValDestroy(one);
				Graphite2Api.FeatureValDestroy(two);
			}
		}

		[Test]
		public void FeatureValDestroy_Null_DoesNotThrowException()
		{
			Graphite2Api.FeatureValDestroy(IntPtr.Zero);
		}

		#endregion

		#region Segment Tests

		[Test]
		public void CinfoUnicodeChar_OnTestCinfo_ReturnsExpectecCharacter()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				IntPtr char_info = Graphite2Api.SegCinfo(seg.Seg, (uint)Graphite2Api.SlotBefore(firstSlot));
				Assert.AreEqual(104, Graphite2Api.CinfoUnicodeChar(char_info));
			}
		}

		[Test]
		public void CinfoBreakWeight_OnTestCharInfo_ReturnsExpectedBreakWeight()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				IntPtr char_info = Graphite2Api.SegCinfo(seg.Seg, (uint)Graphite2Api.SlotBefore(firstSlot));
				int bw = Graphite2Api.CinfoBreakWeight(char_info);
				Assert.AreEqual(-30, bw);
			}
		}

		[Test]
		public void CinfoAfter_OnTestCinfo_ReturnsExpectedIndex()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				IntPtr char_info = Graphite2Api.SegCinfo(seg.Seg, (uint)Graphite2Api.SlotBefore(firstSlot));
				Assert.AreEqual(0, Graphite2Api.CinfoAfter(char_info));
			}
		}

		[Test]
		public void CinfoBefore_OnTestCinfo_ReturnsExpectedIndex()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				IntPtr char_info = Graphite2Api.SegCinfo(seg.Seg, (uint)Graphite2Api.SlotBefore(firstSlot));
				Assert.AreEqual(0, Graphite2Api.CinfoBefore(char_info));
			}
		}

		[Test]
		public void CinfoBase_OnTestCinfo_ReturnsExpectedIndex()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				IntPtr char_info = Graphite2Api.SegCinfo(seg.Seg, (uint)Graphite2Api.SlotBefore(firstSlot));
				Assert.AreEqual(0, Graphite2Api.CinfoBase(char_info));
			}
		}

		[Test]
		public void CountUnicodeCharacters_TestString_ReturnsExpectedNumberOfChars()
		{

			IntPtr str = Marshal.StringToCoTaskMemAnsi("hello world");
			IntPtr error = new IntPtr(0);
			int numCodePoints = Graphite2Api.CountUnicodeCharacters(Encform.Utf8, str, IntPtr.Zero, ref error);
			Assert.AreEqual(11, numCodePoints);
			Marshal.FreeCoTaskMem(str);
		}

		[Test]
		public void MakeSeg_WithTestStringAndPaduakFont_ReturnsNonNullSegment()
		{
			using (var font = new PaduakDisposableFont())
			{
				IntPtr str = Marshal.StringToCoTaskMemAnsi("hello world");
				IntPtr error = new IntPtr(0);
				int numCodePoints = Graphite2Api.CountUnicodeCharacters(Encform.Utf8, str, IntPtr.Zero, ref error);
				IntPtr seg = Graphite2Api.MakeSeg(font.Font, font.Face, 0, IntPtr.Zero, Encform.Utf8, str, numCodePoints, 0);
				Assert.AreNotEqual(IntPtr.Zero, seg);

				Graphite2Api.SegDestroy(seg);
				Marshal.FreeCoTaskMem(str);
			}
		}

		[Test]
		public void SegDestroy_NULL_DoesNotThrowException()
		{
			Graphite2Api.SegDestroy(IntPtr.Zero);
		}

		[Test]
		public void SegAdvanceX_OnTestSeg_ReturnsExpectedAdvance()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				Assert.AreEqual(90.234375f, Graphite2Api.SegAdvanceX(seg.Seg));
			}
		}

		[Test]
		public void SegAdvanceY_OnTestSeg_ReturnsExpectedAdvance()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				Assert.AreEqual(0.0f, Graphite2Api.SegAdvanceY(seg.Seg));
			}
		}

		[Test]
		public void SegNCinfo_OnTestSeg_ReturnsExpectedNumber()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				Assert.AreEqual(11, Graphite2Api.SegNCinfo(seg.Seg));

			}
		}

		[Test]
		public void SegCinfo_OnATestSegAndSlot_ReturnsNonNullCharInfoPtr()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				IntPtr char_info = Graphite2Api.SegCinfo(seg.Seg, (uint)Graphite2Api.SlotBefore(firstSlot));
				Assert.AreNotEqual(IntPtr.Zero, char_info);
			}
		}

		[Test]
		public void SegNSlots_OnTestSeg_ReturnsExpectedNumberOfSlots()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				Assert.AreEqual(11, Graphite2Api.SegNSlots(seg.Seg));
			}
		}

		[Test]
		public void SegFirstSlot_OnPaduakTestSegment_ReturnsNonNullSlot()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr slot = Graphite2Api.SegFirstSlot(seg.Seg);
				Assert.AreNotEqual(IntPtr.Zero, slot);
			}
		}

		[Test]
		public void SegLastSlot_OnPaduakTestSegment_ReturnsNonNullSlot()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr slot = Graphite2Api.SegLastSlot(seg.Seg);
				Assert.AreNotEqual(IntPtr.Zero, slot);
			}
		}

		[Test]
		public void SegJustify_OnATestSegment_DoesNotThrowException()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr slot = Graphite2Api.SegFirstSlot(seg.Seg);
				Graphite2Api.SegJustify(seg.Seg, slot, seg.Font, 100, 0, IntPtr.Zero, IntPtr.Zero);
			}
		}

		[Test]
		public void SlotNextInSegment_OnFirstSlotInPaduakTestSegment_ReturnsNonNullSlot()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr slot = Graphite2Api.SegFirstSlot(seg.Seg);
				Assert.AreNotEqual(IntPtr.Zero, Graphite2Api.SlotNextInSegment(slot));
			}
		}

		[Test]
		public void SlotPrevInSegment_OnSecondSlotInPaduakTestSegment_ReturnsNonNullSlot()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				IntPtr secondSlot = Graphite2Api.SlotNextInSegment(firstSlot);

				Assert.AreNotEqual(IntPtr.Zero, Graphite2Api.SlotPrevInSegment(secondSlot), "returned null slot");
				Assert.AreEqual(firstSlot, Graphite2Api.SlotPrevInSegment(secondSlot), "should equal first slot");
			}
		}

		[Test]
		public void SlotAttachedTo_TestSegFirstSlot_ReturnsNullSlotPtr()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				IntPtr parentSlot = Graphite2Api.SlotAttachedTo(firstSlot);
				Assert.AreEqual(IntPtr.Zero, parentSlot);
			}
		}

		[Test]
		public void SlotFirstAttachment_TestSegFirstSlot_ReturnsNullSlotPtr()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				IntPtr slot = Graphite2Api.SlotFirstAttachment(firstSlot);
				Assert.AreEqual(IntPtr.Zero, slot);
			}
		}

		[Test]
		public void SlotNextSiblingAttachment_TestSegFirstSlot_ReturnsNonNullSlotPtr()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				IntPtr slot = Graphite2Api.SlotNextSiblingAttachment(firstSlot);
				Assert.AreNotEqual(IntPtr.Zero, slot);
			}
		}

		[Test]
		public void SlotGid_TestSegFirstSlot_ReturnsExpectedGid()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				Assert.AreEqual(75, Graphite2Api.SlotGid(firstSlot));
			}
		}

		[Test]
		public void SlotOriginX_TestSegFirstSlot_ReturnsExpectedOffset()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				Assert.AreEqual(0, Graphite2Api.SlotOriginX(firstSlot));
			}
		}

		[Test]
		public void SlotOriginY_TestSegFirstSlot_ReturnsExpectedOffset()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				Assert.AreEqual(0, Graphite2Api.SlotOriginY(firstSlot));
			}
		}

		[Test]
		public void SlotAdvanceX_OnTestSeg_ReturnsExpecedAdvance()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr slot = Graphite2Api.SegFirstSlot(seg.Seg);
				Assert.AreEqual(9.9609375f, Graphite2Api.SlotAdvanceX(slot, seg.Face, seg.Font));
			}
		}

		[Test]
		public void SlotAdvanceY_OnTestSeg_ReturnsExpecedAdvance()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr slot = Graphite2Api.SegFirstSlot(seg.Seg);
				Assert.AreEqual(0.0f, Graphite2Api.SlotAdvanceY(slot, seg.Face, seg.Font));
			}
		}

		[Test]
		public void SlotBefore_OnTestSlot_ReturnsExpectedIndex()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				Assert.AreEqual(0, (uint)Graphite2Api.SlotBefore(firstSlot));
			}
		}

		[Test]
		public void SlotAfter_OnTestSlot_ReturnsExpectedIndex()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				Assert.AreEqual(0, (uint)Graphite2Api.SlotAfter(firstSlot));
			}
		}

		[Test]
		public void SlotIndex_OnTestSlot_ReturnsExpectedIndex()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegFirstSlot(seg.Seg);
				Assert.AreEqual(0, (uint)Graphite2Api.SlotIndex(firstSlot));
			}
		}

		[Test]
		public void SlotAttr_OnTestSegLastSlot_ReturnsExpectedAttributeValue()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegLastSlot(seg.Seg);
				Assert.AreEqual(526, Graphite2Api.SlotAttr(firstSlot, seg.Seg, 0, 0));
			}
		}

		[Test]
		public void SlotCanInsertBefore_OnTestSegLastSlot_ReturnsTrue()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegLastSlot(seg.Seg);
				Assert.AreEqual(1, Graphite2Api.SlotCanInsertBefore(firstSlot));
			}
		}

		[Test]
		public void SlotOriginal_OnTestSegLastSlot_ReturnsExpectedCharInfoValue()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegLastSlot(seg.Seg);
				Assert.AreEqual(10, Graphite2Api.SlotOriginal(firstSlot));
			}
		}

		[Test]
		public void SlotLinebreakBefore_OnATestSlot_DoesNotThrowException()
		{
			using(var seg = new PaduakDisposableTestSegment("hello world"))
			{
				IntPtr firstSlot = Graphite2Api.SegLastSlot(seg.Seg);
				Graphite2Api.SlotLinebreakBefore(firstSlot);
			}
		}

		#endregion

		#region Log Tests

		[Test]
		public void StartLogging_WithTestFaceAndValidLogFileLocation_FileShouldBeCreated()
		{
			int major, minor, bugfix;
			Graphite2Api.EngineVersion(out major, out minor, out bugfix);

			if (major <= 1 && minor < 2)
				Assert.Ignore("Need newer engine to support logging");

			using(var face = new PaduakDisposableFace())
			{
				string filename = Path.GetTempPath() + Guid.NewGuid().ToString();
				Assert.IsTrue(Graphite2Api.StartLogging(face.Face, filename));
				Assert.IsTrue(File.Exists(filename));
				Graphite2Api.StopLogging(face.Face);
				File.Delete(filename);
			}

		}

		#endregion

		#region test helpers

		internal class PaduakDisposableFace : IDisposable
		{
			public IntPtr Face;
			const string PaduakFontLocation = Graphite2ApiTests.PaduakFontLocation;

			public PaduakDisposableFace()
			{
				Face = Graphite2Api.MakeFileFace(PaduakFontLocation, 0);
			}

			#region IDisposable implementation
			public void Dispose()
			{
				Graphite2Api.FaceDestroy(Face);
			}
			#endregion
		}

		internal class PaduakDisposableFont : IDisposable
		{
			public IntPtr Font;
			PaduakDisposableFace _face;

			public IntPtr Face
			{
				get { return _face.Face; }
			}

			public PaduakDisposableFont()
			{
				_face = new PaduakDisposableFace();
				Font = Graphite2Api.MakeFont(20, _face.Face);
			}

			#region IDisposable implementation
			public void Dispose()
			{
				Graphite2Api.FontDestroy(Font);
				_face.Dispose();
			}
			#endregion
		}

		internal class PaduakFeatureVal : IDisposable
		{
			PaduakDisposableFace _face;
			public IntPtr FeatureRef;

			public IntPtr Face
			{
				get { return _face.Face; }
			}

			public PaduakFeatureVal() : this(0)
			{

			}

			public PaduakFeatureVal(ushort n)
			{
				_face = new PaduakDisposableFace();
				FeatureRef = Graphite2Api.FaceFref(_face.Face, n);
			}

			#region IDisposable implementation
			public void Dispose()
			{
				_face.Dispose();
			}
			#endregion
		}

		internal class PaduakDisposableTestSegment : IDisposable
		{
			public IntPtr Seg;
			IntPtr _testData;
			PaduakDisposableFont _font;

			public IntPtr Font
			{
				get { return _font.Font; }
			}

			public IntPtr Face
			{
				get { return _font.Face; }
			}

			public PaduakDisposableTestSegment(string testData)
			{
				_font = new PaduakDisposableFont();
				_testData = Marshal.StringToCoTaskMemAnsi(testData);
				IntPtr error = new IntPtr(0);
				int numCodePoints = Graphite2Api.CountUnicodeCharacters(Encform.Utf8, _testData, IntPtr.Zero, ref error);
				Seg = Graphite2Api.MakeSeg(_font.Font, _font.Face, 0, IntPtr.Zero, Encform.Utf8, _testData, numCodePoints, 0);
			}

			#region IDisposable implementation
			public void Dispose()
			{
				Graphite2Api.SegDestroy(Seg);
				Marshal.FreeCoTaskMem(_testData);
				_font.Dispose();
			}
			#endregion

		}

		#endregion
	}
}
