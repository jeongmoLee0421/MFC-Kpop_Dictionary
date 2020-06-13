
// KpopDictionaryDlg.h: 헤더 파일
//

#pragma once
#include <afxdb.h> // ODBC 기반 데이터베이스 개발

// CKpopDictionaryDlg 대화 상자
class CKpopDictionaryDlg : public CDialogEx
{
// 생성입니다.
public:
	CKpopDictionaryDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KPOPDICTIONARY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listSingerInfo;
	afx_msg void OnClickedButtonInquire();
	CListCtrl m_listSingerSong;
	CString m_strSearchName;
	afx_msg void OnClickedButtonSearch();
	afx_msg void OnClickedButtonReset();
	void DisplaySingerSongList();
	CString m_strSingerName;
	CString m_strSongName;
	CString m_strSongLyric;
	CString m_strSongCompose;
	CString m_strSongGenre;
	CString m_strSongRelease;
	void InitEdit();
	afx_msg void OnClickedButtonAdd();
	afx_msg void OnItemchangedListSingerSong(NMHDR* pNMHDR, LRESULT* pResult);
	int m_nSelectedSong;
	bool chk;
	afx_msg void OnClickedButtonModify();
	int m_nSongId; // CString 형으로 받아온 노래ID값을 int로 변환하기 위한 변수(_ttoi()) -> UPDATE문에 조건으로 활용
	afx_msg void OnClickedButtonDelete();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
