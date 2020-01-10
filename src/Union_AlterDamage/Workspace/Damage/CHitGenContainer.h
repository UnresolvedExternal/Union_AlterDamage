#include <unordered_map>
#include <memory>
#include "Workspace\Utility\CBinSeqGen.h"

namespace NAMESPACE
{
	class CHitGenContainer
	{
	private:
		static std::unique_ptr<int[]> buffer;
		std::unordered_map<int, CBinSeqGen> hitGens;

	public:
		CHitGenContainer();
		CBinSeqGen& operator[](int index);
		void Clear();
		void Archive(zCArchiver& arc);
		void Unarchive(zCArchiver& arc);
	};
}