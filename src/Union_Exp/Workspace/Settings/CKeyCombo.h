#include <sstream>
#include <chrono>
#include <optional>

namespace NAMESPACE
{
	class CKeyCombo
	{
	private:
		std::vector<std::vector<int>> combos;
		mutable std::optional<std::chrono::high_resolution_clock::time_point> lastPressedTime;

		void Build(const std::vector<std::vector<int>>& combos)
		{
			this->combos.clear();

			for (const std::vector<int>& combo : combos)
			{
				std::vector<int> keys;
				keys.reserve(combos.size());

				std::copy(std::begin(combo), std::end(combo), std::back_inserter(keys));
				std::sort(std::begin(keys), std::end(keys));
				keys.erase(std::unique(std::begin(keys), std::end(keys)), std::end(keys));

				if (!keys.empty())
					this->combos.push_back(std::move(keys));
			}

			std::sort(std::begin(this->combos), std::end(this->combos), [](const std::vector<int>& x, const std::vector<int>& y) 
				{ 
					return x.size() < y.size(); 
				});

			for (int i = 0; i < (int)this->combos.size() - 1; i++)
				for (int k = i + 1; k < (int)this->combos.size(); k++)
				{
					const auto& x = this->combos[i];
					const auto& y = this->combos[k];

					if (x.size() != y.size())
						continue;

					bool matches = true;

					for (size_t n = 0; n < x.size() && matches; n++)
						if (x[n] != y[n])
							matches = false;

					if (!matches)
						continue;

					this->combos.erase(std::begin(this->combos) + k);
					k -= 1;
				}
		}

		static bool IsPressed(const std::vector<int>& combo)
		{
			for (int key : combo)
			{
				bool pressed;

				switch (key)
				{
				case MOUSE_BUTTONLEFT: pressed = zinput->GetMouseButtonPressedLeft(); break;
				case MOUSE_BUTTONMID: pressed = zinput->GetMouseButtonPressedMid(); break;
				case MOUSE_BUTTONRIGHT: pressed = zinput->GetMouseButtonPressedRight(); break;
				default: pressed = zKeyPressed(key); break;
				}

				if (!pressed)
					return false;
			}

			return true;
		}

		static bool IsAllModifiers(const std::vector<int>& combo)
		{
			for (int key : combo)
				if (!CKeyHelper::GetInstance().IsModifier(key))
					return false;

			return true;
		}

	public:
		enum class TModifiersResetMode
		{
			ResetIfAll = 0,
			Reset,
			Never
		};

		friend std::ostream& operator<<(std::ostream&, const CKeyCombo&);
		friend std::istream& operator>>(std::istream&, CKeyCombo&);

		CKeyCombo()
		{

		}

		CKeyCombo(const std::vector<std::vector<int>>& combos)
		{
			Build(combos);
		}

		CKeyCombo(const std::initializer_list<std::initializer_list<int>>& combos)
		{
			std::vector<std::vector<int>> vCombos;
			vCombos.reserve(combos.size());

			for (const auto& combo : combos)
				vCombos.push_back(std::vector<int>(combo.begin(), combo.end()));

			Build(vCombos);
		}

		CKeyCombo(const CKeyCombo& right)
		{
			Build(right.combos);
			lastPressedTime = {};
		}

		CKeyCombo(CKeyCombo&& right) = default;

		CKeyCombo& operator=(const CKeyCombo& right)
		{
			Build(right.combos);
			lastPressedTime = {};
			return *this;
		}

		CKeyCombo& operator=(CKeyCombo&& right) = default;

		bool operator==(const CKeyCombo& right) const
		{
			if (combos.size() != right.combos.size())
				return false;

			for (size_t i = 0; i < combos.size(); i++)
			{
				if (combos[i].size() != right.combos[i].size())
					return false;

				for (size_t k = 0; k < combos[i].size(); k++)
					if (combos[i][k] != right.combos[i][k])
						return false;
			}

			return true;
		}

		bool operator!=(const CKeyCombo& right) const
		{
			return !(*this == right);
		}

		explicit operator bool() const
		{
			return combos.size();
		}

		bool GetPressed() const
		{
			if (combos.empty())
				return false;

			for (const auto& combo : combos)
				if (IsPressed(combo))
					return true;

			return false;
		}

		bool GetPressed(const int& throttleMs) const
		{
			if (!GetPressed())
			{
				lastPressedTime = {};
				return false;
			}

			auto now = std::chrono::high_resolution_clock::now();

			if (lastPressedTime)
			{
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - *lastPressedTime);

				if (duration.count() >= throttleMs)
				{
					lastPressedTime = now;
					return true;
				}

				return false;
			}

			lastPressedTime = now;
			return true;
		}

		void ResetTimer() const
		{
			lastPressedTime = {};
		}

		bool GetToggled(const TModifiersResetMode& mode = TModifiersResetMode::ResetIfAll) const
		{
			if (combos.empty())
				return false;

			for (const auto& combo : combos)
				if (IsPressed(combo))
				{
					const bool win32 = dynamic_cast<zCInput_Win32*>(zinput);
					zCArray<int>& keyBuffer = *reinterpret_cast<zCArray<int>*>(ZENDEF(0x0086D2DC, 0x008B2E00, 0x008C36F0, 0x008D1D50));

					const bool resetModifiers = mode == TModifiersResetMode::Reset || mode == TModifiersResetMode::ResetIfAll && IsAllModifiers(combo);

					for (int key : combo)
					{
						if (!resetModifiers && CKeyHelper::GetInstance().IsModifier(key))
							continue;

						bool mouse = true;

						switch (key)
						{
						case MOUSE_BUTTONLEFT: zinput->GetMouseButtonToggledLeft(); break;
						case MOUSE_BUTTONMID: zinput->GetMouseButtonToggledMid(); break;
						case MOUSE_BUTTONRIGHT: zinput->GetMouseButtonToggledRight(); break;
						default: zinput->SetKey(key, false); mouse = false; break;
						}

						if (ZENDEF(mouse, false, false, false))
							continue;
						
						if (win32)
						{
							for (int i = keyBuffer.GetNum() - 1; i >= 0; i--)
								if (keyBuffer[i] == key)
									keyBuffer.RemoveOrderIndex(i);
						}
					}

					return true;
				}

			return false;
		}
	};

	std::ostream& operator<<(std::ostream& out, const CKeyCombo& keyCombo)
	{
		if (!keyCombo.combos.size())
			return out << "#";

		for (size_t i = 0; i < keyCombo.combos.size(); i++)
		{
			if (i)
				out << ", ";

			for (size_t k = 0; k < keyCombo.combos[i].size(); k++)
			{
				if (k)
					out << " + ";
				
				out << CKeyHelper::GetInstance().KeyToName(keyCombo.combos[i][k]).GetVector();
			}
		}

		return out;
	}

	std::istream& operator>>(std::istream& in, CKeyCombo& combo)
	{
		std::string text;
		std::getline(in, text);

		std::vector<std::vector<int>> combos;

		if (text.find('#') < text.length())
		{
			combo.Build(combos);
			return in;
		}

		text = "," + text + ",";
		string keyName;

		for (char ch : text)
		{
			if (isspace(ch))
				continue;

			if (ch == '+' || ch == ',')
			{
				if (int key = CKeyHelper::GetInstance().NameToKey(keyName))
					combos.back().push_back(key);

				keyName.Clear();

				if (ch == ',')
					combos.push_back(std::vector<int>());

				continue;
			}

			keyName += ch;
		}

		combo.Build(combos);
		return in;
	}
}
