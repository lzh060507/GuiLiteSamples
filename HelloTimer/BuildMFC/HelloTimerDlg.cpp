
// HelloTimerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HelloTimer.h"
#include "HelloTimerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CHelloTimerDlg dialog
CHelloTimerDlg* CHelloTimerDlg::m_the_dialog = NULL;
CRITICAL_SECTION CHelloTimerDlg::m_criSection;

CHelloTimerDlg::CHelloTimerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HelloTimer_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_the_dialog = this;
	InitializeCriticalSection(&m_criSection);
}

void CHelloTimerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHelloTimerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CHelloTimerDlg message handlers

BOOL CHelloTimerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	AfxBeginThread(CHelloTimerDlg::ThreadHelloTimer, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHelloTimerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHelloTimerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHelloTimerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CHelloTimerDlg::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

#define DISPLAY_WIDTH	240
#define DISPLAY_HEIGHT	320

struct EXTERNAL_GFX_OP
{
	void(*draw_pixel)(int x, int y, unsigned int rgb);
	void(*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
} my_gfx_op;

void CHelloTimerDlg::draw_pixel(int x, int y, unsigned int rgb)
{
	EnterCriticalSection(&m_criSection);
	static CDC* pDC;
	if (NULL == pDC)
	{
		pDC = m_the_dialog->GetDC();
	}
	pDC->SetPixel(x, y, RGB(((((unsigned int)(rgb)) >> 16) & 0xFF), ((((unsigned int)(rgb)) >> 8) & 0xFF), (((unsigned int)(rgb)) & 0xFF)));
	LeaveCriticalSection(&m_criSection);
}

void CHelloTimerDlg::fill_rect(int x0, int y0, int x1, int y1, unsigned int rgb)
{
	EnterCriticalSection(&m_criSection);
	static CDC* pDC;
	if (NULL == pDC)
	{
		pDC = m_the_dialog->GetDC();
	}
	CBrush brush;
	brush.CreateSolidBrush(RGB(((((unsigned int)(rgb)) >> 16) & 0xFF), ((((unsigned int)(rgb)) >> 8) & 0xFF), (((unsigned int)(rgb)) & 0xFF)));
	CRect rect(x0, y0, x1 + 1, y1 + 1);
	pDC->FillRect(&rect, &brush);
	LeaveCriticalSection(&m_criSection);
}

UINT CHelloTimerDlg::ThreadHelloTimer(LPVOID pParam)
{
	Sleep(500);
	my_gfx_op.draw_pixel = draw_pixel;
	my_gfx_op.fill_rect = fill_rect;
	startHelloTimer(NULL, DISPLAY_WIDTH, DISPLAY_HEIGHT, 2, &my_gfx_op);
	return 0;
}

void CHelloTimerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CHelloTimerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
}
