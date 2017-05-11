#include "StdAfx.h"
#include "PropertyItem.h"

IMPLEMENT_DYNAMIC(CPropertyItem, CMFCPropertyGridProperty)

CPropertyItem::CPropertyItem(const CString& strName, const _variant_t& v, Pipe pipe, LPCTSTR lpszDescr, CobjcontainerDoc::OP code)
	: CMFCPropertyGridProperty(strName, v, lpszDescr)
{
	m_pipe = pipe;
	m_code = code;
}


CPropertyItem::~CPropertyItem(void)
{
}


IMPLEMENT_DYNAMIC(CPropertyItemFile, CMFCPropertyGridFileProperty)

CPropertyItemFile::CPropertyItemFile(const CString& strName
								, const _variant_t& v
								,  Pipe pipe
								, LPCTSTR lpszDescr
								, CobjcontainerDoc::OP code
								, BOOL bOpenFileDialog
								, const CString& strFileName
								, LPCTSTR lpszDefExt
								, DWORD dwFlags
								, LPCTSTR lpszFilter
								, DWORD_PTR dwData)
	: CMFCPropertyGridFileProperty(strName, bOpenFileDialog, strFileName, lpszDefExt, dwFlags, lpszFilter, lpszDescr, dwData)
{
	m_pipe = pipe;
	m_code = code;
}

CPropertyItemFile::~CPropertyItemFile(void)
{

}

