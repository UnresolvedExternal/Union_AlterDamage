namespace NAMESPACE
{
	class CSingleOptionBase
	{
	private:
		static std::vector<CSingleOptionBase*> options;

		zCOption* const& option;
		zCOptionEntry* entry;
		const string entryName;
		const string sectionName;
		const string defaultValue;

		static bool32 ChangeCallback(zCOptionEntry& e)
		{
			for (CSingleOptionBase* option : options)
				if (&e == option->entry)
					option->HandleValueChanged(e.varValueTemp);

			return false;
		}

		void Subscribe()
		{
			zCOptionSection* section = option->GetSectionByName(sectionName, true);
			entry = option->GetEntryByName(section, entryName, true);
			entry->ccbList.InsertEnd(&CSingleOptionBase::ChangeCallback);
			ChangeEntry(entry->varValueTemp.Length() ? entry->varValueTemp : defaultValue, true);
		}

		void Propagate()
		{
			for (int i = 0; i < entry->ccbList.GetNum(); i++)
				entry->ccbList[i](*entry);
		}

	protected:
		virtual void HandleValueChanged(const string& newValue) = 0;

		void ChangeEntry(const string& newValue, bool propagate)
		{
			entry->varValue = entry->varValueTemp = newValue.GetVector();
			
			if (propagate)
				Propagate();
		}

		CSingleOptionBase(zCOption* const& option, const string& sectionName, const string& entryName, const string& defaultValue) :
			option(option),
			sectionName(sectionName),
			entryName(entryName),
			defaultValue(defaultValue)
		{
			options.push_back(this);
		}

		CSingleOptionBase(CSingleOptionBase&& right) :
			option(right.option),
			entry(right.entry),
			entryName(right.entryName),
			sectionName(right.sectionName),
			defaultValue(right.defaultValue)
		{
			auto it = std::find(begin(options), end(options), &right);
			*it = this;
		}

		virtual ~CSingleOptionBase()
		{
			auto it = std::find(begin(options), end(options), this);
			if (it != end(options))
				options.erase(it);
		}

	public:
		static void LoadAll()
		{
			for (CSingleOptionBase* option : options)
				option->Subscribe();
		}
	};

	// define static before the first use to get rid on destruction order
	std::vector<CSingleOptionBase*> CSingleOptionBase::options;

	template <class T>
	class CSingleOption : CSingleOptionBase
	{
	private:
		T value;

	protected:
		void SetValue(const T& newValue, bool propagate)
		{
			if (newValue == value)
				return;

			ChangeEntry(StdToString<T>(newValue), propagate);
			
			if (!propagate)
			{
				value = newValue;
				onChange(*this);
			}
		}

		virtual void HandleValueChanged(const string& strValue) override
		{
			// called when Zengine changes the options
			// we are in propagation already
			SetValue(StdFromString<T>(strValue), false);
		}

	public:
		CSingleOption(zCOption* const& option, const string& sectionName, const string& entryName, const T& defaultValue) :
			CSingleOptionBase(option, sectionName, entryName, StdToString<T>(defaultValue)),
			value()
		{

		}

		CSingleOption(const CSingleOption& right) = delete;

		CSingleOption(CSingleOption&& right) :
			CSingleOptionBase(right),
			value(right),
			onChange(right.onChange)
		{

		}

		// must not be called before options load
		template <class TOther>
		CSingleOption& operator=(const TOther& value)
		{
			T newValue = value;

			// propagate new value to Zengine
			SetValue(newValue, true);
			return *this;
		}

		operator T() const
		{
			return value;
		}

		const T* operator->() const
		{
			return &value;
		}

		const T& operator*() const
		{
			return value;
		}

		CDelegateContainer<void(const CSingleOption&)> onChange;
	};

	template <class T>
	CSingleOption<T> CreateOption(zCOption* const& option, const char* sectionName, const char* entryName, const T& defaultValue)
	{
		return CSingleOption<T>(option, sectionName, entryName, defaultValue);
	}

#define ZOPTION(name, value) auto name = CreateOption(zoptions, PLUGIN_NAME, #name, value)
}