using System;
using System.IO;

namespace NGraphite
{
	public class Face : IDisposable
	{
		IntPtr _face;

		/// <summary>
		/// Initializes a new instance of the <see cref="NGraphite.Face"/> class.
		/// </summary>
		/// <param name='filename'>
		/// fullpath to a graphite enabled font.
		/// </param>
		public Face(string filename, FaceOptions options)
		{
			if (!File.Exists(filename))
				throw new ArgumentException("filename");

			_face = Graphite2Api.MakeFileFace(filename, options);

			if (_face == IntPtr.Zero)
				throw new ArgumentException("filename is not a graphite font");
		}

		public Face(string filename, uint segCacheMaxSize, FaceOptions options)
		{
			if (!File.Exists(filename))
				throw new ArgumentException("filename");

			_face = Graphite2Api.MakeFileFaceWithSegCache(filename, segCacheMaxSize, options);

			if (_face == IntPtr.Zero)
				throw new ArgumentException("filename is not a graphite font");
		}

		/// <summary>
		/// Determins if filename is a graphite font or not.
		/// </summary>
		public static bool IsGraphiteFont(string filename)
		{
			if (!File.Exists(filename))
				return false;

			IntPtr face = Graphite2Api.MakeFileFace(filename, FaceOptions.face_default);
			if (face == IntPtr.Zero)
				return false;

			Graphite2Api.FaceDestroy(face);
			return true;
		}

		internal IntPtr FacePtr
		{
			get { return _face; }
		}

		public Featureval FeaturevalForLang(string lang)
		{
			IntPtr ptr = Graphite2Api.FaceFeaturevalForLang(_face, Graphite2Api.StrToTag(lang));
			if (ptr == IntPtr.Zero)
				return null;
			return new Featureval(ptr);
		}

		public FeatureRef FindFref(Int32 featid)
		{
			IntPtr ptr = Graphite2Api.FaceFindFref(_face, featid);
			if (ptr == IntPtr.Zero)
				return null;
			return new FeatureRef(ptr);
		}

		public UInt16 NFref()
		{
			return Graphite2Api.FaceNFref(_face);
		}

		public FeatureRef Fref(UInt16 i)
		{
			IntPtr ptr = Graphite2Api.FaceFref(_face, i);
			if (ptr == IntPtr.Zero)
				return null;
			return new FeatureRef(ptr);
		}

		public ushort NLanguages()
		{
			return Graphite2Api.FaceNLanguages(_face);
		}

		public UInt32 LangByIndex(UInt16 i)
		{
			return Graphite2Api.FaceLangByIndex(_face, i);
		}

		public short NGlyphs()
		{
			return Graphite2Api.FaceNGlyphs(_face);
		}

		public bool StartLogging(string filename)
		{
			int major, minor, bugfix;
			Graphite2Api.EngineVersion(out major, out minor, out bugfix);

			if (major <= 1 && minor < 2)
				throw new NotSupportedException("A newer graphite engine is needed to support logging");

			return Graphite2Api.StartLogging(_face, filename);
		}

		public void StopLogging()
		{
			int major, minor, bugfix;
			Graphite2Api.EngineVersion(out major, out minor, out bugfix);

			if (major <= 1 && minor < 2)
				throw new NotSupportedException("A newer graphite engine is needed to support logging");

			Graphite2Api.StopLogging(_face);
		}

		#region IDisposable implementation
		public void Dispose()
		{
			Graphite2Api.FaceDestroy(_face);
		}
		#endregion
	}
}
