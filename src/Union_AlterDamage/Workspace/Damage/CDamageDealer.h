#include <memory>

namespace NAMESPACE
{
	class CDamageDealer
	{
	private:
		CHitGenContainer* const hitGen;

		bool IsOneHanded(oCItem* weapon);
		bool IsTwoHanded(oCItem* weapon);
		bool IsMelee(oCItem* weapon);
		int GetBestScalingAttribute(oCItem* weapon);
		void GetIsMeleeAndTalent(const TDamageInfo& info, bool& isMelee, int& talent);
		int GetComboHit(const TDamageInfo& info);

		oEDamageType IndexToDamage(const oEDamageIndex& index);
		bool IsCorrectSpellData(const TDamageInfo& info);
		void SetWeaponModeByWeapon(TDamageInfo& info);
		void SetRawDamageByWeapon(TDamageInfo& info);
		float GetHitChance(oCNpc* npc, int talent);

		void InitInfo(oCNpc* target, const oCNpc::oSDamageDescriptor& desc, TDamageInfo& info);
		void SetWeaponMode(TDamageInfo& info);
		void SetRawDamage(TDamageInfo& info);
		//void HandleIfMiss(TDamageInfo& info);
		void SetEffectiveDamage(TDamageInfo& info);
		void SetTotalDamage(TDamageInfo& info);
		void SetRealDamage(TDamageInfo& info);
		void ApplyDamage(const TDamageInfo& info);
		void AddDots(std::shared_ptr<TDamageInfo> info);
		void UpdateDescriptor(const TDamageInfo& info, oCNpc::oSDamageDescriptor& desc);

	public:
		CDamageDealer(CHitGenContainer* hitGen);
		std::shared_ptr<TDamageInfo> DoDamage(oCNpc* target, oCNpc::oSDamageDescriptor& desc);
	};
}
