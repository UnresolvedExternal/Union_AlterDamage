#include "CKarma.h"
#include <memory>

namespace NAMESPACE
{
	class CDamageDealer
	{
	private:
		std::shared_ptr<CKarma> karma;

		bool IsOneHanded(oCItem* weapon);
		bool IsTwoHanded(oCItem* weapon);
		bool IsMelee(oCItem* weapon);

		oEDamageType IndexToDamage(const oEDamageIndex& index);
		bool IsCorrectSpellData(const TDamageInfo& info);
		void SetWeaponModeByWeapon(TDamageInfo& info);
		void SetRawDamageByWeapon(TDamageInfo& info, float multiplier);

		void InitInfo(oCNpc* target, const oCNpc::oSDamageDescriptor& desc, TDamageInfo& info);
		void SetWeaponMode(TDamageInfo& info);
		void SetFullRawDamage(TDamageInfo& info);
		void HandleIfMiss(TDamageInfo& info);
		void SetEffectiveDamage(TDamageInfo& info);
		void SetTotalDamage(TDamageInfo& info);
		void SetRealDamage(TDamageInfo& info);
		void ApplyDamage(const TDamageInfo& info);
		void AddDots(std::shared_ptr<TDamageInfo> info);
		void UpdateDescriptor(const TDamageInfo& info, oCNpc::oSDamageDescriptor& desc);

	public:
		CDamageDealer(const std::shared_ptr<CKarma>& karma);
		std::shared_ptr<TDamageInfo> DoDamage(oCNpc* target, oCNpc::oSDamageDescriptor& desc);
	};
}