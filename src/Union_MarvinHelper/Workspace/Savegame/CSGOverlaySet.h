namespace NAMESPACE
{
	struct TOverlay
	{
		zSTRING name;
		float time;
	};

	class CSGOverlaySet : public CSGGlobal
	{
	private:
		Array<TOverlay> overlays;

	public:
		CSGOverlaySet(const string& name);
		virtual void Clear() override;
		virtual void Archive(zCArchiver& arc) override;
		virtual void Unarchive(zCArchiver& arc) override;
		void Update();
		void Apply();
	};
}