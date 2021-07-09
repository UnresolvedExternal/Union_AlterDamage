namespace NAMESPACE
{
	class ActiveOptionBase
	{
	private:
		static std::vector<ActiveOptionBase*> options;

		zCOption* const& option;
		zCOptionEntry* entry;
		const string entryName;
		const string sectionName;
		const string defaultValue;

		static bool32 ChangeCallback(zCOptionEntry& e)
		{
			for (ActiveOptionBase* option : options)
				if (&e == option->entry)
					option->HandleValueChanged(e.varValueTemp);

			return false;
		}

		void Subscribe()
		{
			zCOptionSection* section = option->GetSectionByName(sectionName, true);
			entry = option->GetEntryByName(section, entryName, true);
			entry->ccbList.InsertEnd(&ActiveOptionBase::ChangeCallback);
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

		ActiveOptionBase(zCOption* const& option, const string& sectionName, const string& entryName, const string& defaultValue) :
			option{ option },
			entry{ nullptr },
			sectionName{ sectionName },
			entryName{ entryName },
			defaultValue{ defaultValue }
		{
			options.push_back(this);
		}

		ActiveOptionBase(ActiveOptionBase&& right) :
			option{ std::move(right.option) },
			entry{ std::move(right.entry) },
			entryName{ std::move(right.entryName) },
			sectionName{ std::move(right.sectionName) },
			defaultValue{ std::move(right.defaultValue) }
		{
			auto it = std::find(begin(options), end(options), &right);
			*it = this;
		}

		virtual ~ActiveOptionBase()
		{
			auto it = std::find(begin(options), end(options), this);

			if (it != end(options))
				options.erase(it);
		}

	public:
		static void LoadAll()
		{
			for (ActiveOptionBase* option : options)
				option->Subscribe();
		}
	};

	// define static before the first use to get rid on destruction order
	std::vector<ActiveOptionBase*> ActiveOptionBase::options;

	template <class T>
	class ActiveOption : ActiveOptionBase
	{
	private:
		T value;

	protected:
		void SetValue(const T& newValue, bool propagate)
		{
			if (newValue == value)
				return;

			ChangeEntry(StdToString<T>(newValue), propagate);
			
			if (propagate)
				return;
		
			value = newValue;
			onChange();
		}

		virtual void HandleValueChanged(const string& strValue) override
		{
			// called when ZenGin changes the options
			// we are in propagation already
			SetValue(StdFromString<T>(strValue), false);
		}

	public:
		ActiveOption(zCOption* const& option, const string& sectionName, const string& entryName, const T& defaultValue) :
			ActiveOptionBase{ option, sectionName, entryName, StdToString<T>(defaultValue) },
			value{}
		{

		}

		ActiveOption(const ActiveOption& right) = delete;

		ActiveOption(ActiveOption&& right) :
			ActiveOptionBase{ std::move(right) },
			value{ std::move(right.value) },
			onChange{ std::move(right.onChange) }
		{

		}

		// must not be called before options load
		template <class TOther>
		ActiveOption& operator=(const TOther& value)
		{
			T newValue = value;

			// propagate the new value to ZenGin
			SetValue(newValue, true);
			return *this;
		}

		operator T() const { return value; }
		const T* operator->() const	{ return &value; }
		const T& operator*() const { return value; }

		DelegateList<void()> onChange;
	};

	template <class T>
	ActiveOption<T> CreateOption(zCOption* const& option, const char* sectionName, const char* entryName, const T& defaultValue)
	{
		return ActiveOption<T>(option, sectionName, entryName, defaultValue);
	}

#define ZOPTION(name, value) auto name = CreateOption(zoptions, (A PROJECT_NAME).GetVector(), #name, value)
}