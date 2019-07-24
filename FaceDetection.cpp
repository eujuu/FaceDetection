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


// CSub_ProjectDoc ����/�Ҹ�
CSub_ProjectDoc::CSub_ProjectDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.
}
CSub_ProjectDoc::~CSub_ProjectDoc()
{
}
BOOL CSub_ProjectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

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
	// BITMAP Filer Header�Ķ��Ÿ�� ����
	dibHf.bfType = 0x4d42; // 'BM'
	dibHf.bfSize = dwBitsSize + sizeof(BITMAPFILEHEADER); // ��ü���� ũ��
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
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}
#ifdef SHARED_HANDLERS

// ����� �׸��� �����մϴ�.
void CSub_ProjectDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// ������ �����͸� �׸����� �� �ڵ带 �����Ͻʽÿ�.
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

// �˻� ó���⸦ �����մϴ�.
void CSub_ProjectDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ������ �����Ϳ��� �˻� �������� �����մϴ�.
	// ������ �κ��� ";"�� ���еǾ�� �մϴ�.

	// ��: strSearchContent = _T("point;rectangle;circle;ole object;");
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
// CSub_ProjectDoc ����
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
// CSub_ProjectDoc ���
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
	hFile.Read(&dibHf, sizeof(BITMAPFILEHEADER)); // ���� ��带 ����
	if (dibHf.bfType != 0x4D42) { 
		AfxMessageBox(L"Not BMP file!!"); 
		return FALSE; 
	}

	//�� ������ BMP�������� �˻�. 0x4d42=='BM'
	hFile.Read(&dibHi, sizeof(BITMAPINFOHEADER)); //"���������� Header"�� �д´�.

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
	// �������� ����

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
	// Ŭ�����忡 �����ϱ� ���� ������ ���̸� ����
	int rwsize = WIDTHBYTES(biBitCount*width);

	DWORD dwBitsSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 +  rwsize*height * sizeof(char); //
	// �޸� �Ҵ�(������常 ���ܽ�Ų ����)
	HGLOBAL m_hImage = (HGLOBAL)::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	LPSTR pDIB = (LPSTR) ::GlobalLock((HGLOBAL)m_hImage);
	// ����Ÿ����
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
	// Ŭ������ ����
	::OpenClipboard(NULL);
	::SetClipboardData(CF_DIB, m_hImage);
	::CloseClipboard();
	::GlobalUnlock((HGLOBAL)m_hImage);
	GlobalFree(m_hImage);
}


double** CSub_ProjectDoc::OnMaskProcess(unsigned char * Target, double Mask[3][3])
{
	//ȸ��ó���� �Ͼ�� �Լ�
	// TODO: ���⿡ ���� �ڵ� �߰�.
	int i, j, n, m;
	double ** tempInputImage, **tempOutputImage, S = 0.0;
	m_height = dibHi.biHeight;
	m_width = dibHi.biWidth;
	tempInputImage = Image2DMem(m_height + 2, m_width + 2);
	//�Է°��� ���� �޸� �Ҵ�
	tempOutputImage = Image2DMem(m_height, m_width);
	//��°��� ���� �޸� �Ҵ�

	//1���� �Է� ���� m_InputImage[] �� ���� 2���� �迭 tempInputImage[][]�� �Ҵ�
	for (i = 0; i < m_height; i++) {
		for (j = 0; j < m_width; j++) {
			tempInputImage[i + 1][j + 1] = (double)Target[i * m_width + j];
		}
	}

	//ȸ�� ����

	for (i = 1; i < m_height + 1; i++) {
		for (j = 1; j < m_width + 1; j++) {
			for (n = -1; n < 2; n++) {
				for (m = -1; m < 2; m++) {
					S += Mask[n + 1][m + 1] * tempInputImage[i + n][j + m]; //unsigned char ���� ����...
				}
			}
			tempOutputImage[i - 1][j - 1] = S;
			S = 0.0;
		}
	}

	return tempOutputImage; //��� �� ��ȯ
}


double** CSub_ProjectDoc::Image2DMem(int height, int width)
{
	//2���� �޸� �Ҵ��� ���� �Լ�

	double** temp;
	int i, j;
	temp = new double *[height ];
	for (i = 0; i<height ; i++) {
		temp[i] = new double[width ];
	}
	for (i = 0; i<height ; i++) {
		for (j = 0; j<width ; j++) {
			temp[i][j] = 0.0; //0 ����
		}
	} // �Ҵ��2�����޸𸮸��ʱ�ȭ
	return temp;

}

void CSub_ProjectDoc::OnSwap(double *a, double *b)
{ // �����ͱ�ȯ�Լ�
	double temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void CSub_ProjectDoc::OnBubleSort(double * A, int MAX)
{ // ��������������ó���ϴ��Լ�
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
	int i, j, n, m, M = 7, index = 0; // M = ������ø�����
	double *Mask, Value;
	Mask = new double[M*M]; // ����ũ��ũ�����
	m_Re_height = (m_height);
	m_Re_width = (m_width);
	m_Re_size = m_Re_height* m_Re_width;
	m_filterImage = new unsigned char[m_Re_size];
	m_tempImage = Image2DMem(m_height + 6, m_width + 6);
	for (i = 0; i<m_height; i++) {
		for (j = 0; j<m_width; j++) {
			m_tempImage[i][j] = (double)m_Image[i*m_width + j]; //�� ������ ������ �迭�� �ֱ�
		}
	}
	for (i = 0; i<m_height; i++) {
		for (j = 0; j<m_width; j++) {
			for (n = 0; n<M; n++) {
				for (m = 0; m<M; m++) {
					Mask[n*M + m] = m_tempImage[i + n][j + m];
					// �Է¿�������������߶󸶽�ũ�迭������
				}
			}
			OnBubleSort(Mask, M*M); // ����ũ������Ȱ�������
			Value = Mask[(int)(M*M / 2)]; // ���ĵȰ��߰����������
			m_filterImage[index] = (unsigned char)Value;
			// ����������
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
	//��ġ ������ �迭
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
						all_l[k]++; //k�� �� ��ȣ. (1���� ����) //all_l�� �ε����� �󺧹�ȣ�̰� �� ������ �� �󺧹�ȣ�� ������ ������ �ִ�.
				}
			}
		}
		int max_l = 0; //���� ū ���̸� �����ִ� ��
		for (k = 1; k < count+1; k++) {
			if (all_l[k] > all_l[k + 1]) {
				max_l = k; //all_l�� ���߿��� ���� ū�� ã�´�. //maxl�� ���� ū �̹����� �󺧹�ȣ�� ����ִ�.
			}
		}
		
		int *sim_f = new int[count];
		for (i = 0; i < count ; i++) {
			sim_f[i] = 0;
		}
		int last_num = 0;
		k = 0;
		for (i = 1; i < count+2; i++) { //���̰� ���� ū ���� -150���� ū ���� ��� sim_f�� �ִ´�. sim_f���� �� �󺧹�ȣ�� ������ �ִ�.
			if (all_l[i] > (all_l[max_l] * 7/10)) {
				sim_f[k] = i; //����� ũ�⸦ ������ ���� �ѹ��� �����ϰ� ����.
				k++;
			}  //����� ũ�⸦ ã�Ƴ���.
			last_num = k; //sim_f�� �� ������ �ε����� ����.
		}

		msg.Format(_T("������ ��: %d"), last_num);
		AfxMessageBox(msg);
		int first, last;
		int col1, col2;
		int current, max =0, max_i =0, max_j =0;
		for (int label_num =0; label_num <last_num; label_num++) {
			max_i = 0, max = 0, max_j = 0, col1 = 0; col2 = 0;

			for (i = 1; i < m_height - 1; i++) {
				first = 0; last = 0;
				for (j = 1; j < m_width - 1; j++) {
					if (pos[i][j] == sim_f[label_num]) { //�ش� �� �ѹ�.
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
		// �ӽ÷� ���̺��� ������ �޸� ������ � ���̺� ����  
		//-------------------------------------------------------------------------  

		int** map = new int*[h];
		for (i = 0; i < h; i++)
		{
			map[i] = new int[w];
			memset(map[i], 0, sizeof(int)*w);
		}

		int eq_tbl[MAX_LABEL][2] = { { 0, }, };

		//-------------------------------------------------------------------------  
		// ù ��° ��ĵ - �ʱ� ���̺� ���� �� � ���̺� ����  
		//-------------------------------------------------------------------------  

		int label = 0, maxl, minl, min_eq, max_eq;

		for (j = 1; j < h; j++)
			for (i = 1; i < w; i++)
			{
				if (ptr[j][i] != 0)
				{
					// �ٷ� �� �ȼ��� ���� �ȼ� ��ο� ���̺��� �����ϴ� ���  
					if ((map[j - 1][i] != 0) && (map[j][i - 1] != 0))
					{
						if (map[j - 1][i] == map[j][i - 1])
						{
							// �� ���̺��� ���� ���� ���  
							map[j][i] = map[j - 1][i];
						}
						else
						{
							// �� ���̺��� ���� �ٸ� ���, ���� ���̺��� �ο�  
							maxl = max(map[j - 1][i], map[j][i - 1]);
							minl = min(map[j - 1][i], map[j][i - 1]);

							map[j][i] = minl;

							// � ���̺� ����  
							min_eq = min(eq_tbl[maxl][1], eq_tbl[minl][1]);
							max_eq = max(eq_tbl[maxl][1], eq_tbl[minl][1]);

							eq_tbl[eq_tbl[max_eq][1]][1] = min_eq;
						}
					}
					else if (map[j - 1][i] != 0)
					{
						// �ٷ� �� �ȼ����� ���̺��� ������ ���  
						map[j][i] = map[j - 1][i];
					}
					else if (map[j][i - 1] != 0)
					{
						// �ٷ� ���� �ȼ����� ���̺��� ������ ���  
						map[j][i] = map[j][i - 1];
					}
					else
					{
						// �̿��� ���̺��� �������� ������ ���ο� ���̺��� �ο�  
						label++;
						map[j][i] = label;
						eq_tbl[label][0] = label;
						eq_tbl[label][1] = label;
					}
				}
			}

		//-------------------------------------------------------------------------  
		// � ���̺� ����  
		//-------------------------------------------------------------------------  

		int temp;
		for (i = 1; i <= label; i++)
		{
			temp = eq_tbl[i][1];
			if (temp != eq_tbl[i][0])
				eq_tbl[i][1] = eq_tbl[temp][1];
		}

		// � ���̺��� ���̺��� 1���� ���ʴ�� ������Ű��  

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
		// �� ��° ��ĵ - � ���̺��� �̿��Ͽ� ��� �ȼ��� ������ ���̺� �ο�  
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
		// �ӽ� �޸� ���� ����  
		//-------------------------------------------------------------------------  

		for (i = 0; i < h; i++)
			delete[] map[i];
		delete[] map;
		(*cnt)--;
		return ptr;
	}
	

