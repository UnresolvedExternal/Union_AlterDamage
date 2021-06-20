namespace NAMESPACE
{
	class CGridView : public zCView
	{
	private:
		void Init()
		{
			if (screen)
				this->SetFont(screen->GetFontName());
			
			selection.index = -1;
			selection.color = zCOLOR(255, 255, 0);
		}

		int GetColumnDistance()
		{
			return anx(font->GetLetterDistance() * 2);
		}

	public:
		struct TSelection
		{
			int index;
			zCOLOR color;
		};

		TSelection selection;
		Array<string> entries;

		CGridView() : zCView() { Init(); }
		CGridView(int x1, int y1, int x2, int y2) : zCView(x1, y1, x2, y2) { Init(); }

		bool CheckSelection() const
		{
			return selection.index >= 0 && selection.index < (int)entries.GetNum();
		}

		const string& GetSelected() const
		{
			return entries[selection.index];
		}

		void Draw(const Array<string>& entries)
		{
			this->entries.Clear();
			zCView::ClrPrintwin();

			if (!entries.GetNum() || !ondesk || !FontY())
				return;

			int rows = std::max(1, 8192 / FontY());
			int columns = 1;

			for (int column = 0, width = 0; rows * column < (int)entries.GetNum(); column++)
			{
				int maxWidth = 0;
				for (int i = column * rows; i < std::min((int)entries.GetNum(), column * rows + rows); i++)
				{
					zSTRING text = entries[i].GetVector();
					maxWidth = std::max(maxWidth, FontSize(text));
				}

				width += maxWidth;
				if (column)
					width += GetColumnDistance();

				if (width < 8192)
					columns = column + 1;
				else
					break;
			}

			for (int i = 0, x = 0, width = 0; i < (int)entries.GetNum(); i++)
			{
				int row = i % rows;
				int column = i / rows;

				if (column >= columns)
					break;

				if (!row)
				{
					x += width;
					width = 0;
				}

				zSTRING text = entries[i].GetVector();
				width = std::max(width, FontSize(text) + GetColumnDistance());
				zCView::PrintTimed(x, row * FontY(), text, -2.0f, (i == selection.index) ? &selection.color : &color);
				this->entries.Insert(entries[i]);
			}
		}

		void Redraw()
		{
			Array<string> entries;
			for (const string& e : this->entries)
				entries.InsertEnd(e);
			Draw(entries);
		}
	};
}