namespace NAMESPACE
{
	class CLogger
	{
	public:
		template <class T>
		static void Log(const T& obj)
		{
			cmd << TGlobals::pluginSettings.pluginName << ": " << ToString(obj) << endl;
		}

		template <class T>
		static void LogError(const T& obj)
		{
			Message::Error(ToString(obj), TGlobals::pluginSettings.pluginName + " error");
		}
	};
}