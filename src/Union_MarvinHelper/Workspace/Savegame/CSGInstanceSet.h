#include <unordered_map>
#include <unordered_set>

namespace NAMESPACE
{
	class CSGInstanceSet : public CSGGlobal
	{
	private:
		std::unordered_set<int> instances;

	protected:
		virtual void Archive(zCArchiver& arc) override;
		virtual void Unarchive(zCArchiver& arc) override;

	public:
		CSGInstanceSet(const string& name);
		virtual void Clear() override;
		bool Contains(int instance) const;
		bool TryAdd(int instance);
		bool TryRemove(int instance);
		int Size() const;

		std::unordered_set<int>::const_iterator begin() const;
		std::unordered_set<int>::const_iterator end() const;
	};
}