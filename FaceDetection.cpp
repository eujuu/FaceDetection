#include "stdafx.h"
#ifndef SHARED_HANDLERS
#include "Sub_Project.h"
#endif

#include "Sub_ProjectDoc.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#define MAX_LABEL 10000
#endif

// CSub_ProjectDoc
IMPLEMENT_DYNCREATE(CSub_ProjectDoc, CDocument)
BEGIN_MESSAGE_MAP(CSub_ProjectDoc, CDocument)
END_MESSAGE_MAP()


// CSub_ProjectDoc 생성/소멸
CSub_ProjectDoc::CSub_ProjectDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
}
CSub_ProjectDoc::~CSub_ProjectDoc()
{
}
BOOL CSub_ProjectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	::OpenClipboard(NULL);

	if (!IsClipboardFormatAvailable(CF_DIB)) 
		return FALSE;
	HGLOBAL m_hImage = ::GetClipboardData(CF_DIB);

	::CloseClipboard();

	LPSTR pDIB = (LPSTR) ::GlobalLock((HGLOBAL)m_hImage);
	memcpy(&dibHi, pDIB, sizeof(BITMAPINFOHEADER));
	height = dibHi.biHeight; width = dibHi.biWidth;
	int rwsize = WIDTHBYTES(dibHi.biBitCount*width);
	DWORD dwBitsSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + rwsize*height * sizeof(char); //
	
	m_InImg = new unsigned char[dibHi.biSizeImage];
	m_OutImg = new unsigned char[dibHi.biSizeImage];

	if (dibHi.biBitCount == 8)
	{
		memcpy(palRGB, pDIB + sizeof(BITMAPINFOHEADER), sizeof(RGBQUAD) * 256);
		memcpy(m_InImg, pDIB + dwBitsSize - dibHi.biSizeImage, dibHi.biSizeImage);
	}
	else 
		memcpy(m_InImg, pDIB + sizeof(BITMAPINFOHEADER), dibHi.biSizeImage);
	// BITMAP Filer Header파라메타의 설정
	dibHf.bfType = 0x4d42; // 'BM'
	dibHf.bfSize = dwBitsSize + sizeof(BITMAPFILEHEADER); // 전체파일 크기
	if (dibHi.biBitCount == 24) 
		dibHf.bfSize -= sizeof(RGBQUAD) * 256; // no pallette
	dibHf.bfOffBits = dibHf.bfSize - rwsize*height * sizeof(char);
	dibHf.bfReserved1 = dibHf.bfReserved2 = 0;
	return TRUE;
}
// CSub_ProjectDoc serialization
void CSub_ProjectDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}
#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CSub_ProjectDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CSub_ProjectDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CSub_ProjectDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS
// CSub_ProjectDoc 진단
#ifdef _DEBUG
void CSub_ProjectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSub_ProjectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
// CSub_ProjectDoc 명령
BOOL CSub_ProjectDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	int ImgSize;
	int i, j, index;
	int rwsize = WIDTHBYTES(dibHi.biBitCount*width);
	CFile hFile;
	//----- Prof. Kim on 2011. 03. 17 ----------//
	if (!CDocument::OnOpenDocument(lpszPathName)) 
		return FALSE;

	
	hFile.Open(lpszPathName, CFile::modeRead | CFile::typeBinary);
	hFile.Read(&dibHf, sizeof(BITMAPFILEHEADER)); // 파일 헤드를 읽음
	if (dibHf.bfType != 0x4D42) { 
		AfxMessageBox(L"Not BMP file!!"); 
		return FALSE; 
	}

	//이 파일이 BMP파일인지 검사. 0x4d42=='BM'
	hFile.Read(&dibHi, sizeof(BITMAPINFOHEADER)); //"영상정보의 Header"를 읽는다.

	if (dibHi.biBitCount != 8 && dibHi.biBitCount != 24)
	{
		AfxMessageBox(L"Gray/True Color Possible!!"); return FALSE;
	}
	if (dibHi.biBitCount == 8) 
		hFile.Read(palRGB, sizeof(RGBQUAD) * 256);
	
	if (dibHi.biBitCount == 8) 
		ImgSize = hFile.GetLength() - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER) - 256 * sizeof(RGBQUAD);
	else if (dibHi.biBitCount == 24)
		ImgSize = hFile.GetLength() - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);

	m_InImg = new unsigned char[ImgSize]; //dibHi.biSizeImage
	m_OutImg = new unsigned char[ImgSize];
	m_COrigin = new unsigned char[ImgSize];

	hFile.Read(m_InImg, ImgSize);
	hFile.Close();

	height = dibHi.biHeight; width = dibHi.biWidth;

	if (dibHi.biBitCount == 24) 
		return TRUE;
	// 영상데이터 대입

	for (i = 0; i<height; i++) {
		index = i*rwsize;
		for (j = 0; j<width; j++)
			m_InImg[index + j] = (unsigned char)palRGB[(int)m_InImg[index + j]].rgbBlue;
	}
	ImgType = 0;
	if (dibHi.biBitCount == 8) ImgType = 1;
	return TRUE;
}
void CSub_ProjectDoc::CopyClipboard(BYTE* m_CpyImg, int height, int width, int biBitCount)
{
	// 클립보드에 복사하기 위한 파일의 길이를 구함
	int rwsize = WIDTHBYTES(biBitCount*width);

	DWORD dwBitsSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 +  rwsize*height * sizeof(char); //
	// 메모리 할당(파일헤드만 제외시킨 길이)
	HGLOBAL m_hImage = (HGLOBAL)::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	LPSTR pDIB = (LPSTR) ::GlobalLock((HGLOBAL)m_hImage);
	// 데이타복사
	BITMAPINFOHEADER dibCpyHi;

	memcpy(&dibCpyHi, &dibHi, sizeof(BITMAPINFOHEADER));

	dibCpyHi.biBitCount = biBitCount;
	dibCpyHi.biHeight = height;
	dibCpyHi.biWidth = width;
	dibCpyHi.biSizeImage = height*rwsize;

	if (biBitCount == 8) 
		dibCpyHi.biClrUsed = dibCpyHi.biClrImportant = 256;

	memcpy(pDIB, &dibCpyHi, sizeof(BITMAPINFOHEADER));

	if (biBitCount == 8)
	{
		memcpy(pDIB + sizeof(BITMAPINFOHEADER), palRGB, sizeof(RGBQUAD) * 256);
		memcpy(pDIB + dwBitsSize - dibCpyHi.biSizeImage, m_CpyImg, dibCpyHi.biSizeImage);
	}
	else 
		memcpy(pDIB + sizeof(BITMAPINFOHEADER), m_CpyImg, dibCpyHi.biSizeImage);
	// 클립보드 복사
	::OpenClipboard(NULL);
	::SetClipboardData(CF_DIB, m_hImage);
	::CloseClipboard();
	::GlobalUnlock((HGLOBAL)m_hImage);
	GlobalFree(m_hImage);
}


double** CSub_ProjectDoc::OnMaskProcess(unsigned char * Target, double Mask[3][3])
{
	//회선처리가 일어나는 함수
	// TODO: 여기에 구현 코드 추가.
	int i, j, n, m;
	double ** tempInputImage, **tempOutputImage, S = 0.0;
	m_height = dibHi.biHeight;
	m_width = dibHi.biWidth;
	tempInputImage = Image2DMem(m_height + 2, m_width + 2);
	//입력값을 위한 메모리 할당
	tempOutputImage = Image2DMem(m_height, m_width);
	//출력값을 위한 메모리 할당

	//1차원 입력 영상 m_InputImage[] 의 값을 2차원 배열 tempInputImage[][]에 할당
	for (i = 0; i < m_height; i++) {
		for (j = 0; j < m_width; j++) {
			tempInputImage[i + 1][j + 1] = (double)Target[i * m_width + j];
		}
	}

	//회선 연산

	for (i = 1; i < m_height + 1; i++) {
		for (j = 1; j < m_width + 1; j++) {
			for (n = -1; n < 2; n++) {
				for (m = -1; m < 2; m++) {
					S += Mask[n + 1][m + 1] * tempInputImage[i + n][j + m]; //unsigned char 쓰지 말기...
				}
			}
			tempOutputImage[i - 1][j - 1] = S;
			S = 0.0;
		}
	}

	return tempOutputImage; //결과 값 반환
}


double** CSub_ProjectDoc::Image2DMem(int height, int width)
{
	//2차원 메모리 할당을 위한 함수

	double** temp;
	int i, j;
	temp = new double *[height ];
	for (i = 0; i<height ; i++) {
		temp[i] = new double[width ];
	}
	for (i = 0; i<height ; i++) {
		for (j = 0; j<width ; j++) {
			temp[i][j] = 0.0; //0 삽입
		}
	} // 할당된2차원메모리를초기화
	return temp;

}

void CSub_ProjectDoc::OnSwap(double *a, double *b)
{ // 데이터교환함수
	double temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void CSub_ProjectDoc::OnBubleSort(double * A, int MAX)
{ // 데이터의정렬을처리하는함수
	int i, j;
	for (i = 0; i<MAX; i++) {
		for (j = 0; j<MAX - 1; j++) {
			if (A[j] > A[j + 1]) {
				OnSwap(&A[j], &A[j + 1]);
			}
		}
	}
}


unsigned char * CSub_ProjectDoc::OnMedianSub(unsigned char * m_Image)
{
	int i, j, n, m, M = 7, index = 0; // M = 서브샘플링비율
	double *Mask, Value;
	Mask = new double[M*M]; // 마스크의크기결정
	m_Re_height = (m_height);
	m_Re_width = (m_width);
	m_Re_size = m_Re_height* m_Re_width;
	m_filterImage = new unsigned char[m_Re_size];
	m_tempImage = Image2DMem(m_height + 6, m_width + 6);
	for (i = 0; i<m_height; i++) {
		for (j = 0; j<m_width; j++) {
			m_tempImage[i][j] = (double)m_Image[i*m_width + j]; //원 영상을 이차원 배열에 넣기
		}
	}
	for (i = 0; i<m_height; i++) {
		for (j = 0; j<m_width; j++) {
			for (n = 0; n<M; n++) {
				for (m = 0; m<M; m++) {
					Mask[n*M + m] = m_tempImage[i + n][j + m];
					// 입력영상을블록으로잘라마스크배열에저장
				}
			}
			OnBubleSort(Mask, M*M); // 마스크에저장된값을정렬
			Value = Mask[(int)(M*M / 2)]; // 정렬된값중가운데값을선택
			m_filterImage[index] = (unsigned char)Value;
			// 가운데값을출력
			index++;
		}
	}
	return m_filterImage;
}

void CSub_ProjectDoc::OnFace()
{
	int i, j = 0, num_C, pixR, pixG, pixB, k = 0, w = 0;

	double pixY, pixCR, pixCB;
	int First = 0, Last = 0;
	int count = 0;
	double DifMask[3][3] = { { 1., 1., 1. } ,{ 0. ,0., 0. } ,{ -1.,-1.,-1. } };
	double sobel[3][3] = { { -1., -2., -1. } ,{ 0. ,0., 0. } ,{ 1.,2.,1. } };
	double sobel2[3][3] = { { 1., 0., -1. } ,{ 2. ,0., -2. } ,{ 1.,0.,-1. } };
	double Gau[3][3] = { { 1 / 16., 1 / 8., 1 / 16. } ,{ 1 / 8. ,1 / 4., 1 / 8. } ,{ 1 / 16,1 / 8.,1 / 16. } };
	if (dibHi.biBitCount == 24)
		num_C = 3;
	else if (dibHi.biBitCount == 8)
		num_C = 1;
	m_height = dibHi.biHeight;
	m_width = dibHi.biWidth;
	m_size = m_height * m_width;
	//위치 저장할 배열
	pos = new double*[m_height];
	for (i = 0; i < m_height; i++) {
		pos[i] = new double[m_width];
	}
	double **out_p = new double*[m_height];
	for (i = 0; i < m_height; i++) {
		out_p[i] = new double[m_width];
	}
	for (i = 0; i < m_height; i++) {
		for (j = 0; j < m_width; j++) {
			out_p[i][j] = 0;
		}
	}
	if (dibHi.biBitCount == 24) {
		m_OutputImage = new BYTE[m_size * 3];
		mid_Image = new unsigned char[m_size];
		last_Image = new unsigned char[m_size];


		for (i = 0; i < m_size; i++) {
			pixR = m_InImg[i * num_C + 2];
			pixG = m_InImg[i * num_C + 1];
			pixB = m_InImg[i * num_C];

			pixY = (int)(0.299 * pixR + 0.587 * pixG + 0.114 * pixB);
			pixCR = 0.5 * pixR - 0.419 * pixG - 0.0813 * pixB + 123;
			pixCB = -0.169 * pixR - 0.331* pixG + 0.5 * pixB + 130;


			if ((77 < pixCB) && (pixCB < 127) && (133 < pixCR) && (pixCR < 173)) {
				mid_Image[i] = 255;

			}
			else {

				mid_Image[i] = 0;

			}
			//
			m_OutputImage[i * num_C] = m_InImg[i * num_C];
			m_OutputImage[i * num_C + 1] = m_InImg[i * num_C + 1];
			m_OutputImage[i * num_C + 2] = m_InImg[i * num_C + 2];



		}
		mid_Image = OnMedianSub(mid_Image);

		
		for (i = 0; i < m_height; i++) {
			for (j = 0; j < m_width; j++) {
				pos[i][j] = mid_Image[i * m_width + j];
			}
		}


		pos = DibLabeling(pos, &count);

		CString msg;
		

		int * all_l = new int[count+1];
		for (i = 0; i < count + 1; i++) {
			all_l[i] = 0;
		}
		for (i = 0; i < m_height; i++) {
			for (j = 0; j < m_width; j++) {
				for (k = 1; k < count + 1; k++) {
					if (pos[i][j] == k)
						all_l[k]++; //k는 라벨 번호. (1부터 시작) //all_l은 인덱스가 라벨번호이고 그 값으로 그 라벨번호의 개수를 가지고 있다.
				}
			}
		}
		int max_l = 0; //제일 큰 넓이를 갖고있는 라벨
		for (k = 1; k < count+1; k++) {
			if (all_l[k] > all_l[k + 1]) {
				max_l = k; //all_l의 값중에서 제일 큰걸 찾는다. //maxl은 제일 큰 이미지의 라벨번호를 담고있다.
			}
		}
		
		int *sim_f = new int[count];
		for (i = 0; i < count ; i++) {
			sim_f[i] = 0;
		}
		int last_num = 0;
		k = 0;
		for (i = 1; i < count+2; i++) { //넓이가 제일 큰 것의 -150보다 큰 라벨일 경우 sim_f에 넣는다. sim_f에는 그 라벨번호를 가지고 있다.
			if (all_l[i] > (all_l[max_l] * 7/10)) {
				sim_f[k] = i; //비슷한 크기를 가지는 라벨의 넘버를 저장하고 있음.
				k++;
			}  //비슷한 크기를 찾아낸다.
			last_num = k; //sim_f에 들어간 마지막 인덱스를 저장.
		}

		msg.Format(_T("마지막 수: %d"), last_num);
		AfxMessageBox(msg);
		int first, last;
		int col1, col2;
		int current, max =0, max_i =0, max_j =0;
		for (int label_num =0; label_num <last_num; label_num++) {
			max_i = 0, max = 0, max_j = 0, col1 = 0; col2 = 0;

			for (i = 1; i < m_height - 1; i++) {
				first = 0; last = 0;
				for (j = 1; j < m_width - 1; j++) {
					if (pos[i][j] == sim_f[label_num]) { //해당 라벨 넘버.
						if (pos[i][j - 1] == 0 && pos[i][j] == sim_f[label_num])
							first = j;
						if (pos[i][j] == sim_f[label_num] && pos[i][j + 1] == 0)
							last = j;
						current = last - first;
						if (max < current) {
							max = current;
							max_i = i;
							max_j = j;
						}
					}
				}
			}

			col1 = (max_i - max / 2) *m_width + max_j - max;
			col2 = (max_i + max / 2) *m_width + max_j - max;

			for (i = col1; i < col1 + max; i++) {
				m_OutputImage[i *num_C] = 95;
				m_OutputImage[i *num_C + 1] = 0;
				m_OutputImage[i *num_C + 2] = 255;
			}
			for (i = col2; i < col2 + max; i++) {
				m_OutputImage[i *num_C] = 95;
				m_OutputImage[i *num_C + 1] = 0;
				m_OutputImage[i *num_C + 2] = 255;
			}
			for (i = col1; i < col2; i = i + m_width) {
				m_OutputImage[i *num_C] = 95;
				m_OutputImage[i *num_C + 1] = 0;
				m_OutputImage[i *num_C + 2] = 255;
			}
			for (i = col1 + max; i < col2 + max; i = i + m_width) {
				m_OutputImage[i *num_C] = 95;
				m_OutputImage[i *num_C + 1] = 0;
				m_OutputImage[i *num_C + 2] = 255;
			}
		
		}
	
	}
}

double** CSub_ProjectDoc::DibLabeling(double ** ptr, int *cnt) {
		register int i, j;

		int w = m_width;
		int h = m_height;

		//-------------------------------------------------------------------------  
		// 임시로 레이블을 저장할 메모리 공간과 등가 테이블 생성  
		//-------------------------------------------------------------------------  

		int** map = new int*[h];
		for (i = 0; i < h; i++)
		{
			map[i] = new int[w];
			memset(map[i], 0, sizeof(int)*w);
		}

		int eq_tbl[MAX_LABEL][2] = { { 0, }, };

		//-------------------------------------------------------------------------  
		// 첫 번째 스캔 - 초기 레이블 지정 및 등가 테이블 생성  
		//-------------------------------------------------------------------------  

		int label = 0, maxl, minl, min_eq, max_eq;

		for (j = 1; j < h; j++)
			for (i = 1; i < w; i++)
			{
				if (ptr[j][i] != 0)
				{
					// 바로 위 픽셀과 왼쪽 픽셀 모두에 레이블이 존재하는 경우  
					if ((map[j - 1][i] != 0) && (map[j][i - 1] != 0))
					{
						if (map[j - 1][i] == map[j][i - 1])
						{
							// 두 레이블이 서로 같은 경우  
							map[j][i] = map[j - 1][i];
						}
						else
						{
							// 두 레이블이 서로 다른 경우, 작은 레이블을 부여  
							maxl = max(map[j - 1][i], map[j][i - 1]);
							minl = min(map[j - 1][i], map[j][i - 1]);

							map[j][i] = minl;

							// 등가 테이블 조정  
							min_eq = min(eq_tbl[maxl][1], eq_tbl[minl][1]);
							max_eq = max(eq_tbl[maxl][1], eq_tbl[minl][1]);

							eq_tbl[eq_tbl[max_eq][1]][1] = min_eq;
						}
					}
					else if (map[j - 1][i] != 0)
					{
						// 바로 위 픽셀에만 레이블이 존재할 경우  
						map[j][i] = map[j - 1][i];
					}
					else if (map[j][i - 1] != 0)
					{
						// 바로 왼쪽 픽셀에만 레이블이 존재할 경우  
						map[j][i] = map[j][i - 1];
					}
					else
					{
						// 이웃에 레이블이 존재하지 않으면 새로운 레이블을 부여  
						label++;
						map[j][i] = label;
						eq_tbl[label][0] = label;
						eq_tbl[label][1] = label;
					}
				}
			}

		//-------------------------------------------------------------------------  
		// 등가 테이블 정리  
		//-------------------------------------------------------------------------  

		int temp;
		for (i = 1; i <= label; i++)
		{
			temp = eq_tbl[i][1];
			if (temp != eq_tbl[i][0])
				eq_tbl[i][1] = eq_tbl[temp][1];
		}

		// 등가 테이블의 레이블을 1부터 차례대로 증가시키기  

		int* hash = new int[label + 1];
		memset(hash, 0, sizeof(int)*(label + 1));

		for (i = 1; i <= label; i++)
			hash[eq_tbl[i][1]] = eq_tbl[i][1];

		
		*cnt = 1;
		for (i = 1; i <= label; i++)
			if (hash[i] != 0)
				hash[i] = (*cnt)++;

		for (i = 1; i <= label; i++)
			eq_tbl[i][1] = hash[eq_tbl[i][1]];

		delete[] hash;

		//-------------------------------------------------------------------------  
		// 두 번째 스캔 - 등가 테이블을 이용하여 모든 픽셀에 고유의 레이블 부여  
		//-------------------------------------------------------------------------  

		

		for (j = 1; j < h; j++) 
			for (i = 1; i < w; i++)
			{
				if (map[j][i] != 0)
				{
					temp = map[j][i];
					ptr[j][i] = (BYTE)(eq_tbl[temp][1]);
				}
			}
		

		//-------------------------------------------------------------------------  
		// 임시 메모리 공간 해제  
		//-------------------------------------------------------------------------  

		for (i = 0; i < h; i++)
			delete[] map[i];
		delete[] map;
		(*cnt)--;
		return ptr;
	}
	

