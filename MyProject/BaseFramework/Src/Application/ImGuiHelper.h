#pragma once
// ==============================================
// ログウィンドウクラス
// ==============================================

class CustomImGui
{
public:
	
	template<class... Args>		//文字列に変換
	void AddLog(const char* fmt, Args... args)
	{
		// 改行
		std::string str = fmt;
		str += "\n";

		//表示
		m_ITBuf.appendf(str.c_str(), args...);

		//スクロール
		m_Scroll = true;
	}

	// ウィンドウ描画
	void ImGuiUpdate(const char* title)
	{
		if (ImGui::Begin(title))
		{
			ImGui::TextUnformatted(m_ITBuf.begin());
			if (m_Scroll)
			{
				ImGui::SetScrollHere(1.0f);
				m_Scroll = false;
			}
		}
		ImGui::End();
	}

	// ログをクリア
	void Clear() { m_ITBuf.clear(); }

private:
	ImGuiTextBuffer	 m_ITBuf;
	bool			 m_Scroll = false;
};