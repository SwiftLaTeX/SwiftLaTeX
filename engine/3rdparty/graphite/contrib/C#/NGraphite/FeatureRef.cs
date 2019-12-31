using System;
using System.Runtime.InteropServices;

namespace NGraphite
{
	public class FeatureRef
	{
		IntPtr _featureRef;

		internal FeatureRef(IntPtr featureRef)
		{
			_featureRef = featureRef;
		}

		public UInt32 Id
		{
			get
			{
				return Graphite2Api.FrefId(_featureRef);
			}
		}

		public string Label(string langid, Encform utf)
		{
			UInt32 length;
			ushort langId = (ushort)Graphite2Api.StrToTag(langid);
			IntPtr labelPtr = Graphite2Api.FrefLabel(_featureRef, ref langId, utf, out length);
			if (labelPtr == IntPtr.Zero)
				return String.Empty;
			string retLabel = ConvertGraphiteLabelToString(labelPtr, utf, length);
			Graphite2Api.LabelDestroy(labelPtr);
			return retLabel;
		}

		/// <summary>
		/// As Id as a string.
		/// </summary>
		public string IdLabel
		{
			get
			{
				IntPtr ptr = Marshal.StringToCoTaskMemAnsi("    ");
				Graphite2Api.TagToStr(Id, ptr);
				var str = Marshal.PtrToStringAnsi(ptr);
				Marshal.FreeCoTaskMem(ptr);
				return str;
			}
		}

		public string ValueLabel(UInt16 settingsno, string langid, Encform utf)
		{
			UInt32 length;
			ushort langId = (ushort)Graphite2Api.StrToTag(langid);
			IntPtr labelPtr = Graphite2Api.FrefValueLabel(_featureRef, settingsno, ref langId, utf, out length);
			if (labelPtr == IntPtr.Zero)
				return String.Empty;
			string retLabel = ConvertGraphiteLabelToString(labelPtr, utf, length);
			Graphite2Api.LabelDestroy(labelPtr);
			return retLabel;
		}

		public UInt16 NValues()
		{
			return Graphite2Api.FrefNValues(_featureRef);
		}

		public UInt16 Value(UInt16 settingno)
		{
			return Graphite2Api.FrefValue(_featureRef, settingno);
		}

		public UInt16 FeatureValue(Featureval feats)
		{
			return Graphite2Api.FrefFeatureValue(_featureRef, feats.FeatureValPtr);
		}

		public int SetFeatureValue(UInt16 val, Featureval dest)
		{
			return Graphite2Api.FrefSetFeatureValue(_featureRef, val, dest.FeatureValPtr);
		}

		#region helper methods

		protected string ConvertGraphiteLabelToString(IntPtr labelPtr, Encform utf, UInt32 length)
		{
			switch(utf)
			{
				case Encform.Utf8:
					return Marshal.PtrToStringAnsi(labelPtr, (int)length);
				case Encform.Utf16:
					return Marshal.PtrToStringUni(labelPtr, (int)length);
				case Encform.utf32:
					// I am assuming that PtrToStringUni only works for utf-16
					throw new NotImplementedException("Getting labels as utf32 not supportted");
				default:
					throw new NotImplementedException("Unknown EncForm");
			}
		}

		#endregion
	}
}
