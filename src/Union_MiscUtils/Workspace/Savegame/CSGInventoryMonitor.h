#include <unordered_map>

namespace NAMESPACE
{
	enum class TItemState
	{
		Old = 0,
		New,
		Add
	};

	class CSGInventoryMonitor : public CSGGlobal
	{
	private:
		std::unordered_map<int, int> savedInv;
		std::unordered_map<oCItem*, TItemState> states;

		void SaveInventory(oCNpcInventory& inv, std::unordered_map<int, int>& dest);

	protected:
		virtual void Archive(zCArchiver& arc) override;
		virtual void Unarchive(zCArchiver& arc) override;

	public:
		CSGInventoryMonitor(const string& name);
		virtual void Clear() override;
		void Update(oCNpcInventory& inv);
		void BuildStates(oCNpcInventory& inv);
		TItemState GetState(oCItem* item);
	};
}