using System;

namespace NGraphite
{
	public class Featureval : IDisposable
	{
		IntPtr _featureval;
		
		internal Featureval(IntPtr featureval)
		{
			_featureval = featureval;
		}
		
		internal IntPtr FeatureValPtr
		{
			get { return _featureval; }	
		}
		
		public Featureval Clone()
		{
			return new Featureval(Graphite2Api.FeaturevalClone(_featureval));
		}

		#region IDisposable implementation
		public void Dispose()
		{
			Graphite2Api.FeatureValDestroy(_featureval);
		}
		#endregion
	}
}

