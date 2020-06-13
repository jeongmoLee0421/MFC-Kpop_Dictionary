
// KpopDictionaryDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "KpopDictionary.h"
#include "KpopDictionaryDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDatabase m_db; // db연결할 객체
CRecordset* m_pRs; // db에서 받아온 결과를 담을 객체
CFont m_font; // 프로그램 이름에 사용할 폰트

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CKpopDictionaryDlg 대화 상자



CKpopDictionaryDlg::CKpopDictionaryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KPOPDICTIONARY_DIALOG, pParent)
	, m_strSearchName(_T(""))
	, m_strSingerName(_T(""))
	, m_strSongName(_T(""))
	, m_strSongLyric(_T(""))
	, m_strSongCompose(_T(""))
	, m_strSongGenre(_T(""))
	, m_strSongRelease(_T(""))
	, chk(FALSE) // TRUE면 리스트출력함수에서 넘어온것이고 FALSE면 아이템선택에서 넘어온 것
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKpopDictionaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SINGER_INFO, m_listSingerInfo);
	DDX_Control(pDX, IDC_LIST_SINGER_SONG, m_listSingerSong);
	DDX_Text(pDX, IDC_EDIT_SEARCH_NAME, m_strSearchName);
	DDX_Text(pDX, IDC_EDIT_SINGER_NAME, m_strSingerName);
	DDX_Text(pDX, IDC_EDIT_SONG_NAME, m_strSongName);
	DDX_Text(pDX, IDC_EDIT_SONG_LYRIC, m_strSongLyric);
	DDX_Text(pDX, IDC_EDIT_SONG_COMPOSE, m_strSongCompose);
	DDX_Text(pDX, IDC_EDIT_SONG_GENRE, m_strSongGenre);
	DDX_Text(pDX, IDC_EDIT_SONG_RELEASE, m_strSongRelease);
}

BEGIN_MESSAGE_MAP(CKpopDictionaryDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_INQUIRE, &CKpopDictionaryDlg::OnClickedButtonInquire)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CKpopDictionaryDlg::OnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CKpopDictionaryDlg::OnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CKpopDictionaryDlg::OnClickedButtonAdd)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SINGER_SONG, &CKpopDictionaryDlg::OnItemchangedListSingerSong)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CKpopDictionaryDlg::OnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CKpopDictionaryDlg::OnClickedButtonDelete)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CKpopDictionaryDlg 메시지 처리기

BOOL CKpopDictionaryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.


	// 가수 리스트 초기화
	LPWSTR singer_info[6] = { _T("순번"), _T("이름"), _T("나이") , _T("출생") , _T("소속사") , _T("데뷔 년도") };
	int infoWidth[6] = { 40,120,40,120,250,100 };

	for (int i = 0; i < 6; i++) {
		m_listSingerInfo.InsertColumn(i, singer_info[i], LVCFMT_CENTER, infoWidth[i]);
	}
	m_listSingerInfo.SetExtendedStyle(m_listSingerInfo.GetExtendedStyle() | LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES);


	// 곡 리스트 초기화
	LPWSTR singer_song[7] = { _T("순번"), _T("이름"),_T("곡명"),_T("작사") ,_T("작곡") ,_T("장르") ,_T("발매일") };
	int songWidth[7] = { 40,100,120,100,100,120,80 };

	for (int i = 0; i < 7; i++) {
		m_listSingerSong.InsertColumn(i, singer_song[i], LVCFMT_CENTER, songWidth[i]);
	}
	m_listSingerSong.SetExtendedStyle(m_listSingerSong.GetExtendedStyle() | LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES);


	// 설정한 ODBC와 연결
	try {
		BOOL bOpen = m_db.OpenEx(_T("DSN=kpop_dictionary_odbc;SERVER=127.0.0.1;PORT=3306;UID=root;PWD=root;DATABASE=kpop_dictionary"), CDatabase::noOdbcDialog);
		if (bOpen)
			m_pRs = new CRecordset(&m_db);
	}
	catch (CException * e) {
		e->ReportError();
	}

	// 폰트 객체 생성
	m_font.CreateFont(30, 0, 0, 0, FW_HEAVY, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, _T("굴림체"));

	//해당 스태틱에 적용
	GetDlgItem(IDC_STATIC_SINGER)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC_INFO)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC_SYSTEM)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC_VERSION)->SetFont(&m_font);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CKpopDictionaryDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CKpopDictionaryDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CKpopDictionaryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 가수 목록 조회
void CKpopDictionaryDlg::OnClickedButtonInquire()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_listSingerInfo.DeleteAllItems(); // 중복 출력 방지를 위해

	int nCount = 0;
	CString strCount;
	try {
		CString m_strSingerId(_T("")), m_strSingerName(_T("")), m_strSingerAge(_T("")),
			m_strSingerBirth(_T("")), m_strSingerAgency(_T("")), m_strSingerDebut(_T(""));
		BOOL bOpen = m_pRs->Open(CRecordset::snapshot, _T("SELECT * FROM SINGER_INFO"));
		if (bOpen) {
			int iRow = 1; // 1행부터 시작
			BOOL blsEOF = m_pRs->IsEOF();
			DWORD dwSize = m_pRs->GetRowsetSize(); // 행의 크기를 가져온다.
			if (!blsEOF) {
				for (m_pRs->MoveFirst(); !m_pRs->IsEOF(); m_pRs->MoveNext()) {
					int iFiledCnt = m_pRs->GetODBCFieldCount();
					for (int iCol = 0; iCol < iFiledCnt; iCol++) { // 0번 열부터 읽어 나간다.
						CString sItem;
						m_pRs->SetAbsolutePosition(iRow); // 행의 위치
						m_pRs->GetFieldValue(iCol, sItem); // 해당 필드의 값을 읽어서 sItem에 저장
						if (iCol == 0) {
							m_strSingerId = sItem;
						}
						else if (iCol == 1) {
							m_strSingerName = sItem;
						}
						else if (iCol == 2) {
							m_strSingerAge = sItem;
						}
						else if (iCol == 3) {
							m_strSingerBirth = sItem;
						}
						else if (iCol == 4) {
							m_strSingerAgency = sItem;
						}
						else if (iCol == 5) {
							m_strSingerDebut = sItem;
						}
					}
					// db에서 읽어온 값을 리스트에 출력
					strCount.Format(_T("%d"), nCount + 1);
					m_listSingerInfo.InsertItem(nCount, strCount);
					m_listSingerInfo.SetItem(nCount, 0, LVIF_TEXT, m_strSingerId, 0, 0, 0, 0);
					m_listSingerInfo.SetItem(nCount, 1, LVIF_TEXT, m_strSingerName, 0, 0, 0, 0);
					m_listSingerInfo.SetItem(nCount, 2, LVIF_TEXT, m_strSingerAge, 0, 0, 0, 0);
					m_listSingerInfo.SetItem(nCount, 3, LVIF_TEXT, m_strSingerBirth, 0, 0, 0, 0);
					m_listSingerInfo.SetItem(nCount, 4, LVIF_TEXT, m_strSingerAgency, 0, 0, 0, 0);
					m_listSingerInfo.SetItem(nCount, 5, LVIF_TEXT, m_strSingerDebut, 0, 0, 0, 0);
					iRow++; // 다음 db 행으로
					nCount++; // 다음 list 인덱스로
				}
			}
		}
	}
	catch (CException* e) {
		e->ReportError();
	}
	m_pRs->Close();
}


// 특정 가수 노래 검색
void CKpopDictionaryDlg::OnClickedButtonSearch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_listSingerSong.DeleteAllItems();

	int nCount = 0;
	CString strCount;
	UpdateData(TRUE); // 컨트롤 값을 변수로 가져온다.
	CString strQuery;
	CString m_strSongId, m_strSingerName, m_strSongName, m_strSongLyric,
		m_strSongCompose, m_strSongGenre, m_strSongRelease;

	// 에딧 컨트롤로부터 넘어온 변수를 이용해서 동적으로 쿼리문을 만든다.
	strQuery.Format(_T("SELECT * FROM SINGER_SONG WHERE SINGER_NAME='%s'"), m_strSearchName);

	try {
		BOOL bOpen = m_pRs->Open(CRecordset::snapshot, strQuery);
		if (bOpen) {
			int iRow = 1;
			BOOL blsEOF = m_pRs->IsEOF();
			DWORD dwSize = m_pRs->GetRowsetSize();
			if (blsEOF) {
				AfxMessageBox(_T("가수이름을 다시 입력해주시기 바랍니다."));
			}
			if (!blsEOF) { // 올바르지 않은 데이터가 넘어오면 이 부분에서 막혀서 Close()로 넘어감(빈칸 또는 없는 가수)
				for (m_pRs->MoveFirst(); !m_pRs->IsEOF(); m_pRs->MoveNext()) {
					int iFieldCnt = m_pRs->GetODBCFieldCount(); // 해당 필드 갯수를 가져온다.
					for (int iCol = 0; iCol < iFieldCnt; iCol++) {
						CString sItem;
						m_pRs->SetAbsolutePosition(iRow);
						m_pRs->GetFieldValue(iCol,sItem);
						// db에서 가져온 열 값을 각 변수에 저장
						if (iCol == 0) {
							m_strSongId = sItem;
						}
						else if (iCol == 1) {
							m_strSingerName = sItem;
						}
						else if (iCol == 2) {
							m_strSongName = sItem;
						}
						else if (iCol == 3) {
							m_strSongLyric = sItem;
						}
						else if (iCol == 4) {
							m_strSongCompose = sItem;
						}
						else if (iCol == 5) {
							m_strSongGenre = sItem;
						}
						else if (iCol == 6) {
							m_strSongRelease = sItem;
						}
					}
					// DB로부터 넘어온 데이터들이 변수들에 저장이 되면 해당 변수들로 리스트를 채운다.
					strCount.Format(_T("%d"), nCount + 1);
					chk = TRUE; // 이름 중복 출력 방지
					m_listSingerSong.InsertItem(nCount, strCount);
					m_listSingerSong.SetItem(nCount, 0, LVIF_TEXT, m_strSongId, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 1, LVIF_TEXT, m_strSingerName, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 2, LVIF_TEXT, m_strSongName, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 3, LVIF_TEXT, m_strSongLyric, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 4, LVIF_TEXT, m_strSongCompose, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 5, LVIF_TEXT, m_strSongGenre, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 6, LVIF_TEXT, m_strSongRelease, 0, 0, 0, 0);
					iRow++;
					nCount++;
				}
			}
		}
		m_strSearchName.Empty(); // 해당 변수를 비운다.
		UpdateData(FALSE); // 비운 변수 값을 컨트롤로 넘겨서 에딧컨트롤을 초기화시킨다.
	}
	catch (CException * e) {
		e->ReportError();
	}
	m_pRs->Close();
}


// 리셋, 추가, 수정, 삭제시 리스트를 다시 출력하는 함수
void CKpopDictionaryDlg::DisplaySingerSongList()
{
	// TODO: 여기에 구현 코드 추가.
	int nCount = 0;
	CString strCount;
	try {
		CString m_strSongId, m_strSingerName, m_strSongName, m_strSongLyric,
			m_strSongCompose, m_strSongGenre, m_strSongRelease;
		BOOL bOpen = m_pRs->Open(CRecordset::snapshot, _T("SELECT * FROM SINGER_SONG"));

		if (bOpen) {
			int iRow = 1;
			BOOL blsEOF = m_pRs->IsEOF();
			DWORD dwSize = m_pRs->GetRowsetSize();
			if (!blsEOF) {
				for (m_pRs->MoveFirst(); !m_pRs->IsEOF(); m_pRs->MoveNext()) {
					int iFieldCnt = m_pRs->GetODBCFieldCount();
					for (int iCol = 0; iCol < iFieldCnt; iCol++) {
						CString sItem;
						m_pRs->SetAbsolutePosition(iRow);
						m_pRs->GetFieldValue(iCol, sItem);
						if (iCol == 0) {
							m_strSongId = sItem;
						}
						if (iCol == 1) {
							m_strSingerName = sItem;
						}
						else if (iCol == 2) {
							m_strSongName = sItem;
						}
						else if (iCol == 3) {
							m_strSongLyric = sItem;
						}
						else if (iCol == 4) {
							m_strSongCompose = sItem;
						}
						else if (iCol == 5) {
							m_strSongGenre = sItem;
						}
						else if (iCol == 6) {
							m_strSongRelease = sItem;
						}
					}
					strCount.Format(_T("%d"), nCount + 1);
					chk = TRUE; // 이름 중복 출력 방지
					m_listSingerSong.InsertItem(nCount, strCount);
					m_listSingerSong.SetItem(nCount, 0, LVIF_TEXT, m_strSongId, 0, 0, 0, 0); // 이 부분에서 메시지 처리 과정 중간에 LVN_ITEMCHANGE를 한번 처리한다.
					m_listSingerSong.SetItem(nCount, 1, LVIF_TEXT, m_strSingerName, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 2, LVIF_TEXT, m_strSongName, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 3, LVIF_TEXT, m_strSongLyric, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 4, LVIF_TEXT, m_strSongCompose, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 5, LVIF_TEXT, m_strSongGenre, 0, 0, 0, 0);
					m_listSingerSong.SetItem(nCount, 6, LVIF_TEXT, m_strSongRelease, 0, 0, 0, 0);
					iRow++;
					nCount++;
				}
			}
		}
	}
	catch (CException * e) {
		e->ReportError();
	}

	m_pRs->Close();
}


// 노래 리스트 초기화
void CKpopDictionaryDlg::OnClickedButtonReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_listSingerSong.DeleteAllItems(); // 리스트를 지우고
	DisplaySingerSongList(); // 다시출력
}


// 컨트롤 초기화
void CKpopDictionaryDlg::InitEdit()
{
	// TODO: 여기에 구현 코드 추가.

	// 변수값들을 전부 비우고
	m_strSingerName.Empty();
	m_strSongName.Empty();
	m_strSongLyric.Empty();
	m_strSongCompose.Empty();
	m_strSongGenre.Empty();
	m_strSongRelease.Empty();

	// 변수값을 컨트롤로 넘겨 초기화 시킨다.
	UpdateData(FALSE);
}


// 노래 추가하기
void CKpopDictionaryDlg::OnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_listSingerSong.DeleteAllItems();
	UpdateData(TRUE);
	CString m_strQuery;

	if (!m_strSingerName.IsEmpty() && !m_strSongName.IsEmpty() && !m_strSongLyric.IsEmpty() &&
		!m_strSongCompose.IsEmpty() && !m_strSongGenre.IsEmpty() && !m_strSongRelease.IsEmpty()) { // 모든 에딧컨트롤에 입력되었는지 확인

		// 해당 변수들을 가지고 동적으로 쿼리문 생성
		m_strQuery.Format(_T("INSERT INTO SINGER_SONG (singer_name, song_name, song_lyric, song_compose, song_genre, song_release)VALUES('%s', '%s', '%s', '%s', '%s', '%s')"),
			m_strSingerName, m_strSongName, m_strSongLyric, m_strSongCompose, m_strSongGenre, m_strSongRelease);

		m_db.BeginTrans(); // 트랜잭션 시작

		try {
			m_db.ExecuteSQL(m_strQuery);
		}
		catch (CException * e) {
			e->ReportError();
		}

		m_db.CommitTrans(); // 트랜잭션이 성공적으로 끝남
	}
	else {
		AfxMessageBox(_T("모든 항목을 입력해주시기 바랍니다"));
	}

	DisplaySingerSongList(); // 리스트 다시 그리기
	InitEdit(); // 컨트롤 초기화
}


// 노래 리스트에서 어떤 아이템이 선택되었는지
void CKpopDictionaryDlg::OnItemchangedListSingerSong(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (chk == TRUE) { // 검색과 리셋버튼을 누르면 메시지 처리과정에 SetItem메소드에서 LVN_ITEMCHANGE로 넘어와서 가수 이름이 출력되게 된다.
		chk = FALSE; // 해당 메시지 처리를 방지하고 나서 아이템 클릭시에는 정상적으로 동작하기 위해서 FALSE로 바꿔준다.
		return; // TRUE라면 즉 리스트 출력함수에서 넘어온것이면 return 시키고 FALSE라면 즉 아이템클릭으로 넘어온 것이면 그대로 진행한다.
	}

	m_nSelectedSong = pNMLV->iItem; // 어떤 항목이 눌렸는지
	CString m_strSongId;

	// 해당 아이템의 텍스트를 가져와 변수에 대입
	m_strSongId = m_listSingerSong.GetItemText(m_nSelectedSong, 0);
	m_nSongId = _ttoi(m_strSongId); // CString을 int로 변환
	m_strSingerName = m_listSingerSong.GetItemText(m_nSelectedSong, 1);
	m_strSongName = m_listSingerSong.GetItemText(m_nSelectedSong, 2);
	m_strSongLyric = m_listSingerSong.GetItemText(m_nSelectedSong, 3);
	m_strSongCompose = m_listSingerSong.GetItemText(m_nSelectedSong, 4);
	m_strSongGenre = m_listSingerSong.GetItemText(m_nSelectedSong, 5);
	m_strSongRelease = m_listSingerSong.GetItemText(m_nSelectedSong, 6);

	UpdateData(FALSE); // 변수의 값을 컨트롤로 넘김
	*pResult = 0;
}

// 노래 목록 수정
void CKpopDictionaryDlg::OnClickedButtonModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_listSingerSong.DeleteAllItems();
	UpdateData(TRUE);
	CString m_strQuery;

	if (!m_strSingerName.IsEmpty() && !m_strSongName.IsEmpty() && !m_strSongLyric.IsEmpty() &&
		!m_strSongCompose.IsEmpty() && !m_strSongGenre.IsEmpty() && !m_strSongRelease.IsEmpty()) {
		m_strQuery.Format(_T("UPDATE SINGER_SONG SET singer_name='%s', song_name='%s', song_lyric='%s', song_compose='%s', song_genre='%s', song_release='%s' WHERE song_id=%d"),
			m_strSingerName, m_strSongName, m_strSongLyric, m_strSongCompose, m_strSongGenre, m_strSongRelease, m_nSongId);

		m_db.BeginTrans();

		try {
			m_db.ExecuteSQL(m_strQuery);
		}
		catch (CException * e) {
			e->ReportError();
		}

		m_db.CommitTrans();
	}
	else {
		AfxMessageBox(_T("모든 항목을 입력해주시기 바랍니다."));
	}

	DisplaySingerSongList();
	InitEdit();
}

// 노래 삭제
void CKpopDictionaryDlg::OnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_listSingerSong.DeleteAllItems();
	CString m_strQuery;

	if (m_nSelectedSong >= 0) { // 어느 아이템이 눌렸다면
		m_strQuery.Format(_T("DELETE FROM SINGER_SONG WHERE song_id=%d"),
			m_nSongId);

		m_db.BeginTrans();

		try {
			m_db.ExecuteSQL(m_strQuery);
		}
		catch (CException * e) {
			e->ReportError();
		}

		m_db.CommitTrans();
	}
	else {
		AfxMessageBox(_T("항목을 선택하지 않았습니다."));
	}

	DisplaySingerSongList();
}


void CKpopDictionaryDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	// CDatabase와 CRecordset 종료
	if (m_db.IsOpen())
		m_db.Close();
	if (m_pRs->IsOpen())
		m_pRs->Close();

	// 사용 완료한 폰트 객체 삭제
	m_font.DeleteObject();
}


// 컨트롤이 화면에 출력되기 전에 Dialog에게 WM_CTLCOLOR 메시지를 보낸다. 이때 중간에 가로채서 색을 지정해주면 된다.
HBRUSH CKpopDictionaryDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_SINGER) {
		pDC->SetTextColor(RGB(153, 000, 153));
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_INFO) {
		pDC->SetTextColor(RGB(153, 000, 153));
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_SYSTEM) {
		pDC->SetTextColor(RGB(153 , 000, 153));
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_VERSION) {
		pDC->SetTextColor(RGB(051, 255, 102));
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}
