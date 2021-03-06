// EBSymbol.cpp: implementation of the CSymbol class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SymbolBase11.h"
#include "textfile.h"
#include "Markup.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



extern DWORD gnSourceID;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCellDefLib1::CCellDefLib1()
{
	m_mapSourceCellDefs.InitHashTable(97);
}

CCellDefLib1::~CCellDefLib1()
{
	Clear();	
}

void CCellDefLib1::Clear()
{
	// 释放块定义
	for (int i=0;i<m_arrCellDefs.GetSize();i++)
	{
		CellDef def = m_arrCellDefs.GetAt(i);
		def.Clear();
	}
	
	m_arrCellDefs.RemoveAll();
	m_CacheCell.Clear();

	POSITION pos= m_mapSourceCellDefs.GetStartPosition();
	while (pos != NULL)
	{
		DWORD sourceID = 0;
		CArrayCellDef *pArrCellDef = NULL;
		m_mapSourceCellDefs.GetNextAssoc(pos,sourceID,pArrCellDef);
		if (pArrCellDef)
		{
			for ( i=0;i<pArrCellDef->GetSize();i++)
			{
				CellDef def = pArrCellDef->GetAt(i);
				def.Clear();
			}

			delete pArrCellDef;
			pArrCellDef = NULL;
		}
	}

	m_mapSourceCellDefs.RemoveAll();
}
BOOL IsChineseString(char *p)
{
	if (p == NULL) return FALSE;

	char *p1 = p;
	while (*p1 != '\0')
	{
		if ((*p1)&0x80)
		{
			return TRUE;
		}
		p1++;
	}

	return FALSE;
}
BOOL CCellDefLib1::ReadFrom(CString& strXML)
{
	Clear();

	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOLS) )
	{
		xmlFile.IntoElem();
		
		while( xmlFile.FindElem(XMLTAG_SYMBOL) )
		{
			xmlFile.IntoElem();

			if( xmlFile.FindElem(XMLTAG_TYPE) )
			{
				data = xmlFile.GetData();
				if(data.CompareNoCase(_T("CellDef")) == 0)
				{
					CellDef def;
					def.Create();

					if( xmlFile.FindElem(XMLTAG_DATA) )
					{
						xmlFile.IntoElem();
						
						if( xmlFile.FindElem(XMLTAG_NAME) )
						{
							data = xmlFile.GetData();
							strcpy(def.m_name,ConvertTCharToChar(data));
						}
						if( xmlFile.FindElem(XMLTAG_GRBUFFER2D) )
						{
							//							xmlFile.IntoElem();
							
							CString strDoc = xmlFile.GetSubDoc();
							def.m_pgr->ReadFrom(strDoc);
							
						}
						xmlFile.OutOfElem();
							
						
					}

					def.RefreshEnvelope();
					m_arrCellDefs.Add(def);					
					
				}
				else
				{
					xmlFile.OutOfElem();
					continue;
				}
			}
			xmlFile.OutOfElem();			
			
		}


	}
//	SetCellDefLib(this);
	return TRUE;


}

BOOL CCellDefLib1::WriteTo(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOLS) )
	{
		xmlFile.IntoElem();
		for(int i=0; i<m_arrCellDefs.GetSize(); i++)
		{
			CellDef def = m_arrCellDefs.GetAt(i);

			if( xmlFile.AddElem(XMLTAG_SYMBOL) )
			{
				xmlFile.IntoElem();

				xmlFile.AddElem(XMLTAG_TYPE,_T("CellDef"));

				if( xmlFile.AddElem(XMLTAG_DATA) )
				{
					xmlFile.IntoElem();

					xmlFile.AddElem(XMLTAG_NAME,(LPCTSTR)def.m_name);

					CString strDoc;
					def.m_pgr->WriteTo(strDoc);
					xmlFile.AddSubDoc(strDoc);

					
					xmlFile.OutOfElem();

				}

				xmlFile.OutOfElem();
			}

		}
		xmlFile.OutOfElem();
		
		
	}
	
	strXML = xmlFile.GetDoc();
	return TRUE;
	
	
}

#ifdef _WIN64
struct My_Graph2d
{
	void CopyFrom(const Graph2d& item)
	{
		type = item.type;
		color = item.color;
		needdraw = item.needdraw;
		use_needdraw = item.use_needdraw;
		bUseSelfcolor = item.bUseSelfcolor;
		bUseBackColor = item.bUseBackColor;
		next = (DWORD)item.next;
		owner = (DWORD)item.owner;
	}
	
	DWORD type : 4;//图元类型
	DWORD color : 24;//RGB 颜色
	DWORD needdraw : 1;//是否需要绘制
	DWORD use_needdraw : 1;//绘制时是否使用
	DWORD bUseSelfcolor : 1;// 是否使用独立颜色
	DWORD bUseBackColor : 1;// 使用背景色绘制
	DWORD next;//下一个图元的链接指针
	DWORD owner;
};
struct My_GrVertexList2d
{
	My_GrVertexList2d& operator=(const GrVertexList2d& item)
	{
		pts = (DWORD)item.pts;
		nlen = item.nlen;
		nuse = item.nuse;
		isCompress = item.isCompress;
		return *this;
	}
	
	DWORD pts;//顶点列表
	DWORD nlen : 16;//顶点内存长度和当前使用数
	DWORD nuse : 15;
	DWORD isCompress : 1;//是否做过压缩
};
//点状图形
struct My_GrPoint2d : public My_Graph2d
{
	My_GrPoint2d& operator=(const GrPoint2d& item)
	{
		CopyFrom((Graph2d)item);
		pt = item.pt;kx = item.kx; ky = item.ky; 
		angle = item.angle; bGrdSize = item.bGrdSize;
		bFlat = item.bFlat; cell = item.cell;
		width = item.width;
		return *this;
	}
	GR_PT_2D pt;
	float kx;    //x缩放比例
	float ky;    //y缩放比例
	float angle; //旋转角
	int bGrdSize : 1; //尺寸是否随显示比率变化
	int bFlat : 1; //方点还是圆点
	int cell : 24; //图元符号的索引(例如方块、圆点、矩形框、圆圈、十字丝、叉叉等等)
	float width; // 图元线宽，约定：width>=0表示指向的图元使用统一线宽width，width<0表示指向的图元的线宽比例为width
};
//线状图形
struct My_GrLineString2d : public My_Graph2d
{
	My_GrLineString2d& operator=(const GrLineString2d& item)
	{
		CopyFrom((Graph2d)item);
		ptlist = item.ptlist;
		xl = item.xl; xh = item.xh; yl = item.yl; yh = item.yh;
		width = item.width; bGrdWid = item.bGrdWid;
		fDrawScale = item.fDrawScale; style = item.style;
		return *this;
	}
	My_GrVertexList2d ptlist;
	float xl, xh, yl, yh;
	float width;
	int bGrdWid : 1;
	float fDrawScale;//比例系数
	int style : 24; //线型符号的索引，如果是基本线型，就按照基本线型绘制，否则，就按照实线绘制
};
//面状图形
struct My_GrPolygon2d : public My_Graph2d
{
	My_GrPolygon2d& operator=(const GrPolygon2d& item)
	{
		CopyFrom((Graph2d)item);
		ptlist = item.ptlist;
		xl = item.xl; xh = item.xh; yl = item.yl; yh = item.yh;
		filltype = item.filltype; index = item.index;
		pts = (DWORD)item.pts;
		return *this;
	}
	My_GrVertexList2d ptlist;
	float xl, xh, yl, yh;
	int filltype : 8;    //填充类型，分为：颜色填充、纹理填充、
	int index : 24;  //填充符号的索引，对应类型分别为：不使用、纹理ID；
	DWORD pts;	   //当为纹理填充时，此参数为纹理坐标，点数目与顶点数目一致
};
//文本图形
struct My_GrText2d : public My_Graph2d
{
	My_GrText2d& operator=(const GrText2d& item)
	{
		CopyFrom((Graph2d)item);
		pt = item.pt;
		x[0] = item.x[0]; y[0] = item.y[0];
		x[1] = item.x[1]; y[1] = item.y[1];
		x[2] = item.x[2]; y[2] = item.y[2];
		x[3] = item.x[3]; y[3] = item.y[3];
		bGrdSize = item.bGrdSize;
		bRotateWithView = item.bRotateWithView;
		text = item.text;
		settings = item.settings;
		return *this;
	}
	GR_PT_2D pt;
	float x[4], y[4];
	DWORD bGrdSize : 1;
	DWORD bRotateWithView : 1;
	TCHAR *text;
	TextSettings settings;
};
#else
#define My_GrPoint2d			GrPoint2d
#define My_GrLineString2d		GrLineString2d
#define My_GrPolygon2d		GrPolygon2d
#define My_GrText2d				GrText2d
#endif

BOOL CCellDefLib1::Load(FILE* stream)
{
	Clear();
	GrText2d aaa;
	My_GrText2d bbb;
	bbb = aaa;
	if (stream == NULL ) return FALSE;

	// 图元
	int num;
	fread(&num, sizeof(int), 1, stream);

	for (int i=0; i<num; i++)
	{
		// 图元名
		CellDef def;
		def.Create();
		fread(def.m_name,sizeof(char),_MAX_FNAME,stream);

		// Graph个数
		int numofgraph = 0;
		fread(&numofgraph, sizeof(int), 1, stream);

		// 依次读取每一个Graph
		while (numofgraph--)
		{
			int type;
			fread(&type, sizeof(int), 1, stream);

			switch(type)
			{
			case GRAPH_TYPE_POINT2D:
				{
					My_GrPoint2d point;					
					fread(&point, sizeof(My_GrPoint2d), 1, stream);

					PT_2D pt;
					COPY_2DPT(pt,point.pt);

					def.m_pgr->Point(point.color,&pt,point.kx,point.ky,0,point.bGrdSize,point.cell,point.bFlat);
					
					break;
				}
			case GRAPH_TYPE_LINESTRING2D:
				{
					My_GrLineString2d line;
					fread(&line, sizeof(My_GrLineString2d), 1, stream);

					//读后面的数据
					def.m_pgr->BeginLineString(line.color,line.width,line.bGrdWid,line.style,line.fDrawScale);
					
					for (int j=0; j<line.ptlist.nuse; j++)
					{
						GrVertex2d grpt;
						fread(&grpt,sizeof(GrVertex2d),1,stream);
						PT_2D pt;
						COPY_2DPT(pt,grpt);
						if(grpt.code == 0)
						{
							def.m_pgr->MoveTo(&pt);
						}
						else
							def.m_pgr->LineTo(&pt);
					}
					
					def.m_pgr->End();
					
					break;
				}
			case GRAPH_TYPE_POLYGON2D:
				{
					My_GrPolygon2d polygon;
					fread(&polygon, sizeof(My_GrPolygon2d), 1, stream);
					
					//读后面的数据
					def.m_pgr->BeginPolygon(polygon.color,polygon.filltype,polygon.index,(PT_2D*)polygon.pts,0);
					
					for (int j=0; j<polygon.ptlist.nuse; j++)
					{
						GrVertex2d grpt;
						fread(&grpt,sizeof(GrVertex2d),1,stream);
						PT_2D pt;
						COPY_2DPT(pt,grpt);
						if(grpt.code == 0)
						{
							def.m_pgr->MoveTo(&pt);
						}
						else
							def.m_pgr->LineTo(&pt);
					}
					
					def.m_pgr->End();

					break;
				}
			case GRAPH_TYPE_TEXT2D:
				{
					My_GrText2d text;
					fread(&text, sizeof(My_GrText2d), 1, stream);

					TCHAR content[_MAX_FNAME];
					fread(content,sizeof(char),_MAX_FNAME,stream);

					PT_2D pt;
					COPY_2DPT(pt,text.pt);
					
					def.m_pgr->Text(text.color,&pt,content,&(text.settings),text.bGrdSize);
					
					break;
				}
			default:
				break;
			}
		}

		def.RefreshEnvelope();
		m_arrCellDefs.Add(def);
		
	}
	
//	fclose( stream );

	return TRUE;

// 	CMarkup rfile;
// 	rfile.Load(m_strCellPath);
// 	CString strDoc = rfile.GetDoc();
// 	
// 	return ReadFrom(strDoc);

}

BOOL CCellDefLib1::Save(FILE* stream)
{
	CArray<CellDef,CellDef> arrItems;
	
	for( int i=0; i<m_arrCellDefs.GetSize(); i++)
	{
		CellDef item = m_arrCellDefs[i];
		if( item.m_nSourceID==0 )
		{
			arrItems.Add(item);
		}
	}

	// 图元
	int num = arrItems.GetSize();
	fwrite(&num,sizeof(int),1,stream);

	for (i=0; i<num; i++)
	{
		// 图元名
		CellDef def = arrItems.GetAt(i);
		fwrite(def.m_name,sizeof(char),_MAX_FNAME,stream);

		// Graph个数
		int numofgraph = 0;
		const Graph2d *pGr = def.m_pgr->HeadGraph();
		while( pGr )
		{
			numofgraph++;
			pGr = pGr->next;
		}
		fwrite(&numofgraph,sizeof(int),1,stream);
		
		//依次存储每一个Graph
		float xoff, yoff;
		def.m_pgr->GetOrigin(xoff,yoff);
		pGr = def.m_pgr->HeadGraph();
		while( pGr )
		{
			// 
			int type = pGr->type;
			fwrite(&type,sizeof(int),1,stream);

			switch(pGr->type)
			{
			case GRAPH_TYPE_POINT2D:
				{
					GrPoint2d *pPoint = (GrPoint2d*)pGr;
					if(pPoint == NULL)  continue;

					GrPoint2d point = *pPoint;
					point.pt.x += xoff;
					point.pt.y += yoff;
#ifdef _WIN64
					My_GrPoint2d myPoint;
					myPoint = *pPoint;
					fwrite(&myPoint, sizeof(My_GrPoint2d), 1, stream);
#else
					fwrite(&point,sizeof(GrPoint2d),1,stream);
#endif					
					break;
				}
			case GRAPH_TYPE_LINESTRING2D:
				{
					GrLineString2d *pLineString = (GrLineString2d*)pGr;
					if(pLineString == NULL)  continue;

#ifdef _WIN64
					My_GrLineString2d myLine;
					myLine = *pLineString;
					fwrite(&myLine, sizeof(My_GrLineString2d), 1, stream);
#else	
					fwrite(pLineString,sizeof(GrLineString2d),1,stream);
#endif
					// 将数据附在后面
					for( int i=0; i<pLineString->ptlist.nuse; i++)
					{
						GrVertex2d pt;
						pt.x = pLineString->ptlist.pts[i].x+xoff;
						pt.y = pLineString->ptlist.pts[i].y+yoff;
						pt.code = pLineString->ptlist.pts[i].code&1;

						fwrite(&pt,sizeof(GrVertex2d),1,stream);
						
// 						CString strtmp;
// 						strtmp.Format("%16.6f %16.6f %3d\n",
// 							(double)pLineString->ptlist.pts[i].x+m_ptXoff,
// 							(double)pLineString->ptlist.pts[i].y+m_ptYoff,
// 							pLineString->ptlist.pts[i].code&1);
						
					
					}
									
					break;
				}
			case GRAPH_TYPE_POLYGON2D:
				{
					GrPolygon2d *pPolygon = (GrPolygon2d*)pGr;
					if(pPolygon == NULL)  continue;
#ifdef _WIN64
					My_GrPolygon2d myPolygon;
					myPolygon = *pPolygon;
					fwrite(&myPolygon, sizeof(My_GrPolygon2d), 1, stream);
#else	
					fwrite(pPolygon,sizeof(GrPolygon2d),1,stream);
#endif					
					// 将数据附在后面
					for( int i=0; i<pPolygon->ptlist.nuse; i++)
					{
						GrVertex2d pt;
						pt.x = pPolygon->ptlist.pts[i].x+xoff;
						pt.y = pPolygon->ptlist.pts[i].y+yoff;
						pt.code = pPolygon->ptlist.pts[i].code&1;
						
						fwrite(&pt,sizeof(GrVertex2d),1,stream);
						
						// 						CString strtmp;
						// 						strtmp.Format("%16.6f %16.6f %3d\n",
						// 							(double)pLineString->ptlist.pts[i].x+m_ptXoff,
						// 							(double)pLineString->ptlist.pts[i].y+m_ptYoff,
						// 							pLineString->ptlist.pts[i].code&1);
						
						
					}
					break;
				}
			case GRAPH_TYPE_TEXT2D:
				{
					GrText2d *pText = (GrText2d*)pGr;
					if (pText == NULL)  continue;

					GrText2d text = *pText;
					text.pt.x += xoff;
					text.pt.y += yoff;
#ifdef _WIN64
					My_GrText2d myText;
					myText = text;
					fwrite(&myText, sizeof(My_GrPoint2d), 1, stream);
#else
					fwrite(&text,sizeof(GrText2d),1,stream);
#endif
					fwrite(text.text,sizeof(char),_MAX_FNAME,stream);
					
					break;
				}
			default:
				pGr = NULL;
				break;
			}

			pGr = pGr->next;
				
		}
		
		
	}
	
//	fclose( stream );

	return TRUE;

	
// 	CMarkup rfile;
// 	rfile.Load(strCellPath);
// 	CString strDoc = rfile.GetDoc();
// 
// 	CBaseLineTypeLib1 baseLinetypeLib;
// 	baseLinetypeLib.ReadFrom(strDoc);
// 	
// 	CString strSymbolDoc, strCellDoc, strLineDoc;
// 	this->WriteTo(strCellDoc);
// 	baseLinetypeLib.WriteTo(strLineDoc);
// 	
// 	if (GetCellDefCount()>0 && baseLinetypeLib.GetBaseLineTypeCount()>0)
// 	{
// 		//清除换行符,空格等
// 		strCellDoc.TrimRight();
// 		
// 		strSymbolDoc += strCellDoc;
// 		strSymbolDoc.TrimRight("</Symbols>");
// 		strLineDoc.TrimLeft("<Symbols>");
// 		strSymbolDoc += strLineDoc;
// 	}
// 	else if (GetCellDefCount() > 0)
// 	{
// 		strSymbolDoc += strCellDoc;
// 	}
// 	else if (baseLinetypeLib.GetBaseLineTypeCount() > 0)
// 	{
// 		strSymbolDoc += strLineDoc;
// 	}
// 	
// 	CMarkup wfile;
// 	wfile.SetDoc(strSymbolDoc);
// 	wfile.Save(strCellPath);	
// 	
// 	return TRUE;
}

CellDef CCellDefLib1::GetCellDef(int idx)
{
	int size = m_arrCellDefs.GetSize();
	if (idx>=0 && idx<size)
	{
		return m_arrCellDefs[idx];
	}
	else if (idx >= size)
	{
		CArrayCellDef *pArrCellDef = NULL;
		if (m_mapSourceCellDefs.Lookup(gnSourceID,pArrCellDef) && pArrCellDef->GetSize() > (idx-size))
		{
			return pArrCellDef->ElementAt(idx-size);
		}
	}

	return CellDef();
}

int CCellDefLib1::GetCellDefIndex(LPCTSTR name)
{
	CellDef def;
	int index = -1;

	//找到的结果 index 可能来自于数据源，所以需要对index做有效性判断
	if( m_CacheCell.Find(name,index) && index>=0 && index<m_arrCellDefs.GetSize() )
	{
		def = m_arrCellDefs.GetAt(index);
		if( gnSourceID!=0 && def.m_nSourceID!=0 && gnSourceID!=def.m_nSourceID )
		{

		}
		else
			return index;
	}

	int size = m_arrCellDefs.GetSize();

	CStringA name_a = ConvertTCharToChar(name);

	for (int i=0; i<size; i++)
	{
		def = m_arrCellDefs.GetAt(i);
		if(stricmp(def.m_name,name_a) == 0)
		{
			m_CacheCell.AddToRecent(ConvertCharToTChar(def.m_name),i);
			return i;
		}		
	}
	
	CArrayCellDef *pArrCellDef = NULL;
	if (m_mapSourceCellDefs.Lookup(gnSourceID,pArrCellDef))
	{
		for (int i=0; i<pArrCellDef->GetSize(); i++)
		{
			def = pArrCellDef->GetAt(i);
			if(stricmp(def.m_name,name_a) == 0)
			{
				int t = size + i;
				m_CacheCell.AddToRecent(ConvertCharToTChar(def.m_name),t);
				return t;
			}
			
		}
	}
	return -1;

}

int CCellDefLib1::GetCellDefCount()
{
	int size = m_arrCellDefs.GetSize();
	CArrayCellDef *pArrCellDef = NULL;
	if (m_mapSourceCellDefs.Lookup(gnSourceID,pArrCellDef))
	{
		size += pArrCellDef->GetSize();
	}
	return size;
}

int	CCellDefLib1::AddCellDef(CellDef def)
{
	int size = m_arrCellDefs.GetSize();
	for (int i=size-1; i>=0; i--)
	{
		CellDef deflocal = m_arrCellDefs.GetAt(i);
		if (stricmp(def.m_name,deflocal.m_name) == 0)
		{
			m_arrCellDefs.RemoveAt(i);
			m_CacheCell.Clear();
		}
	}

	def.RefreshEnvelope();

	if (def.m_nSourceID != 0)
	{
		CArrayCellDef *pArrCellDef = NULL;
		if (m_mapSourceCellDefs.Lookup(def.m_nSourceID,pArrCellDef))
		{
			for (int i=pArrCellDef->GetSize()-1; i>=0; i--)
			{
				CellDef deflocal = pArrCellDef->GetAt(i);
				if (stricmp(def.m_name,deflocal.m_name) == 0)
				{
					pArrCellDef->RemoveAt(i);
				}
			}
		}
		else
		{
			pArrCellDef = new CArrayCellDef;
			if (!pArrCellDef) return -1;

			m_mapSourceCellDefs.SetAt(def.m_nSourceID,pArrCellDef);
		}

		if (pArrCellDef)
		{
			return (size+pArrCellDef->Add(def));
		}
	}
	else
	{
		return m_arrCellDefs.Add(def);
	}

	return -1;
}

int	CCellDefLib1::DelCellDef(LPCTSTR name)
{
	int size = m_arrCellDefs.GetSize();

	CStringA name_a = ConvertTCharToChar(name);

	CellDef def;
	for (int i=0; i<size; i++)
	{
		def = m_arrCellDefs.GetAt(i);
		if(stricmp(def.m_name,name_a) == 0)
		{
			def.Clear();
			m_arrCellDefs.RemoveAt(i);
			m_CacheCell.Clear();
			return i;
		}
		
	}

	CArrayCellDef *pArrCellDef = NULL;
	if (m_mapSourceCellDefs.Lookup(gnSourceID,pArrCellDef))
	{
		for (int i=0; i<pArrCellDef->GetSize(); i++)
		{
			def = pArrCellDef->GetAt(i);
			if(stricmp(def.m_name,name_a) == 0)
			{
				def.Clear();
				pArrCellDef->RemoveAt(i);
				m_CacheCell.Clear();
				return i+size;
			}
			
		}
	}

	return -1;
}

void CCellDefLib1::DelAllFromSourceID(DWORD id)
{
	m_CacheCell.Clear();

	if (id == 0)
	{
		m_arrCellDefs.RemoveAll();
		return;
	}

	CArrayCellDef *pArrCellDef = NULL;
	if (m_mapSourceCellDefs.Lookup(id,pArrCellDef))
	{
		for (int i=0; i<pArrCellDef->GetSize(); i++)
		{
			CellDef def = pArrCellDef->GetAt(i);
			def.Clear();
		}

		pArrCellDef->RemoveAll();
		delete pArrCellDef;
		m_mapSourceCellDefs.RemoveKey(id);
	}
}

CellDef CCellDefLib1::GetCellDef(LPCTSTR name)
{
	int idx = GetCellDefIndex(name);
	if (idx < 0) return CellDef();

	return GetCellDef(idx);
}

BOOL CCellDefLib1::SetCellDef(int idx, CellDef def)
{
	if (idx < 0) return FALSE;

	int size = m_arrCellDefs.GetSize();

	CellDef *pCellDef = NULL;
	if (idx>=0 && idx<size)
	{		
		pCellDef = &m_arrCellDefs.ElementAt(idx);
	}
	else if (idx >= size)
	{
		CArrayCellDef *pArrCellDef = NULL;
		if (m_mapSourceCellDefs.Lookup(gnSourceID,pArrCellDef) && pArrCellDef->GetSize() > (idx-size))
		{
			pCellDef = &pArrCellDef->ElementAt(idx-size);
		}
	}

	if (pCellDef)
	{
		if (stricmp(pCellDef->m_name,def.m_name) != 0)
		{
			m_CacheCell.Clear();
		}
		
		strcpy(pCellDef->m_name, def.m_name);
		if (pCellDef->m_pgr != def.m_pgr)
		{
			pCellDef->m_pgr->CopyFrom(def.m_pgr);
		}

		return TRUE;
	}	
	
	return FALSE;	
}

BOOL CCellDefLib1::SetCellDef(LPCTSTR name, CellDef def)
{
	if (name == NULL)
		return FALSE;
	
	int idx = GetCellDefIndex(name);
	if (idx < 0) return FALSE;

	def.RefreshEnvelope();
	return SetCellDef(idx, def);	
	
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CBaseLineTypeLib1::CBaseLineTypeLib1()
{
	m_mapSourceBaseLineTypes.InitHashTable(97);
}

CBaseLineTypeLib1::~CBaseLineTypeLib1()
{
	Clear();
}

BOOL CBaseLineTypeLib1::ReadFrom(CString& strXML)
{
	m_arrItems.RemoveAll();

	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOLS) )
	{
		xmlFile.IntoElem();
		
		while( xmlFile.FindElem(XMLTAG_SYMBOL) )
		{
			xmlFile.IntoElem();
			
			BaseLineType line;
			if( xmlFile.FindElem(XMLTAG_TYPE) )
			{
				data = xmlFile.GetData();
				if(data.CompareNoCase(_T("LineType")) == 0)
				{
					if( xmlFile.FindElem(XMLTAG_DATA) )
					{
						xmlFile.IntoElem();
						
						if( xmlFile.FindElem(XMLTAG_NAME) )
						{
							data = xmlFile.GetData();
							strcpy(line.m_name,ConvertTCharToChar(data));
						}

						if( xmlFile.FindElem(XMLTAG_LINENUM) )
						{
							data = xmlFile.GetData();
							line.m_nNum = _ttoi(data);

							if(line.m_nNum > 8)
								line.m_nNum = 8;
						}

						if( xmlFile.FindElem(XMLTAG_LINELENGTH) )
						{
							data = xmlFile.GetData();
	
							int i = 0;
							double	value;
							TCHAR	*pBuf = data.GetBuffer(data.GetLength()), *pStart, *pStop, *pMax;
							
							pStart = pBuf; pStop = pStart; pMax = pBuf+data.GetLength();
							while( pStop<pMax && i<line.m_nNum)
							{								
								value = _tcstod(pStart,&pStop);
//								if( pStop>=pMax || pStop==pStart )break;
								line.m_fLens[i] = value;
								i++;
								pStart = pStop;
							}
						}

						xmlFile.OutOfElem();
						
						
					}

					m_arrItems.Add(line);
					
				}
			}

			xmlFile.OutOfElem();
			
			
		}

		xmlFile.OutOfElem();
		
		
	}
//	SetBaseLineTypeLib(this);
	return TRUE;

}

BOOL CBaseLineTypeLib1::WriteTo(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOLS) )
	{
		xmlFile.IntoElem();
		for(int i=0; i<m_arrItems.GetSize(); i++)
		{
			BaseLineType line = m_arrItems.GetAt(i);
			
			if( xmlFile.AddElem(XMLTAG_SYMBOL) )
			{
				xmlFile.IntoElem();
				
				xmlFile.AddElem(XMLTAG_TYPE,_T("LineType"));
				
				if( xmlFile.AddElem(XMLTAG_DATA) )
				{
					xmlFile.IntoElem();
					
					xmlFile.AddElem(XMLTAG_NAME,(LPCTSTR)line.m_name);
					
					data.Format(_T("%d"),line.m_nNum);
					xmlFile.AddElem(XMLTAG_LINENUM,data);
					
					data.Empty();
					for(int j=0; j<line.m_nNum; j++)
					{
						CString tmp;
						tmp.Format(_T("%.3f "),line.m_fLens[j]);
						data += tmp;
					}
					xmlFile.AddElem(XMLTAG_LINELENGTH,data);
					
					xmlFile.OutOfElem();
					
				}
				
				xmlFile.OutOfElem();
			}
			
		}
		xmlFile.OutOfElem();
		
		
	}
	strXML = xmlFile.GetDoc();
	return TRUE;

}



//基本线型（虚线线型）
struct BaseLineType2
{	
	//定义名称
	char m_name[_MAX_FNAME];
	
	//虚线段数目（少于8）
	int m_nNum;
	
	//虚线段长度数据（正数为实线，负数为虚线）
	float m_fLens[8];
};


BOOL CBaseLineTypeLib1::Load(FILE *stream)
{
	m_arrItems.RemoveAll();
	m_CacheLType.Clear();

	if (stream == NULL) return FALSE;

	int num;
	fread(&num,sizeof(int),1,stream);
	m_arrItems.SetSize(num);

//	fread(m_arrItems.GetData(),sizeof(BaseLineType),num,stream);

	for( int i=0; i<num; i++)
	{
		BaseLineType2 item2;
		fread(&item2,sizeof(item2),1,stream);

		BaseLineType item;
		memcpy(item.m_fLens,item2.m_fLens,sizeof(item.m_fLens));
		memcpy(item.m_name,item2.m_name,sizeof(item.m_name));	
		item.m_nNum = item2.m_nNum;

		m_arrItems[i] = item;
	}

	return TRUE;

// 	SetPath(strPath);
// 	
// 	CMarkup rfile;
// 	rfile.Load(m_strLinePath);
// 	CString strDoc = rfile.GetDoc();
// 	
// 	return ReadFrom(strDoc);
}

BOOL CBaseLineTypeLib1::Save(FILE *stream)
{
// 	CString strLinePath = m_strLinePath;
// 	if (strPath != NULL)
// 	{
// 		strLinePath = strPath;
// 	}
	
	if (stream == NULL ) return FALSE;

	CArray<BaseLineType2,BaseLineType2> arrItems;

	for( int i=0; i<m_arrItems.GetSize(); i++)
	{
		BaseLineType item = m_arrItems[i];
		BaseLineType2 item2;
		if( item.m_nSourceID==0 )
		{
			memcpy(item2.m_fLens,item.m_fLens,sizeof(item.m_fLens));
			memcpy(item2.m_name,item.m_name,sizeof(item.m_name));	
			item2.m_nNum = item.m_nNum;

			arrItems.Add(item2);
		}
	}

	int num = arrItems.GetSize();
	fwrite(&num,sizeof(int),1,stream);

	fwrite(arrItems.GetData(),sizeof(BaseLineType2),num,stream);

	return TRUE;
		
		// 	CMarkup rfile;
// 	rfile.Load(strLinePath);
// 	CString strDoc = rfile.GetDoc();
// 	CCellDefLib1 cellDefLib;
// 	cellDefLib.ReadFrom(strDoc);
// 	
// 	CString strSymbolDoc, strCellDoc, strLineDoc;
// 	cellDefLib.WriteTo(strCellDoc);
// 	this->WriteTo(strLineDoc);
// 	
// 	if (cellDefLib.GetCellDefCount()>0 && GetBaseLineTypeCount()>0)
// 	{
// 		//清除换行符,空格等
// 		strCellDoc.TrimRight();
// 		
// 		strSymbolDoc += strCellDoc;
// 		strSymbolDoc.TrimRight("</Symbols>");
// 		strLineDoc.TrimLeft("<Symbols>");
// 		strSymbolDoc += strLineDoc;
// 	}
// 	else if (cellDefLib.GetCellDefCount() > 0)
// 	{
// 		strSymbolDoc += strCellDoc;
// 	}
// 	else if (GetBaseLineTypeCount() > 0)
// 	{
// 		strSymbolDoc += strLineDoc;
// 	}
// 	
// 	CMarkup wfile;
// 	wfile.SetDoc(strSymbolDoc);
// 	wfile.Save(strLinePath);
	
	return TRUE;
}


//DEL BOOL  CBaseLineTypeLib1::SaveToFile(LPCTSTR strPath)
//DEL {
//DEL 	CString strLinePath = m_strLinePath;
//DEL 	if (strPath != NULL)
//DEL 	{
//DEL 		strLinePath = strPath;
//DEL 	}
//DEL 	
//DEL 	CMarkup rfile;
//DEL 	rfile.Load(strLinePath);
//DEL 	CString strDoc = rfile.GetDoc();
//DEL // 	CTextFileRead rfile(strLinePath);
//DEL // 	rfile.Read(strDoc);
//DEL 	CCellDefLib1 cellDefLib;
//DEL 	cellDefLib.ReadFrom(strDoc);
//DEL //	rfile.Close();
//DEL 	
//DEL 	CString strSymbolDoc,strCellDoc,strLineDoc;
//DEL 	cellDefLib.WriteTo(strCellDoc);
//DEL 	this->WriteTo(strLineDoc);
//DEL 
//DEL 	if (cellDefLib.GetCellDefCount()>0 && GetBaseLineTypeCount()>0)
//DEL 	{
//DEL 		//清除换行符,空格等
//DEL 		strCellDoc.TrimRight();
//DEL 		
//DEL 		strSymbolDoc += strCellDoc;
//DEL 		strSymbolDoc.TrimRight("</Symbols>");
//DEL 		strLineDoc.TrimLeft("<Symbols>");
//DEL 		strSymbolDoc += strLineDoc;
//DEL 	}
//DEL 	else if (cellDefLib.GetCellDefCount() > 0)
//DEL 	{
//DEL 		strSymbolDoc += strCellDoc;
//DEL 	}
//DEL 	else if (GetBaseLineTypeCount() > 0)
//DEL 	{
//DEL 		strSymbolDoc += strLineDoc;
//DEL 	}
//DEL 
//DEL 	CMarkup wfile;
//DEL 	wfile.SetDoc(strSymbolDoc);
//DEL 	wfile.Save(strLinePath);
//DEL // 	CTextFileWrite wfile(strLinePath, CTextFileBase::UNI16_LE);
//DEL // 	if (wfile.IsOpen())
//DEL // 	{
//DEL // 		wfile.SetCodePage(0);
//DEL // 		wfile<<strSymbolDoc;
//DEL // 	}
//DEL // 	rfile.Close();
//DEL 
//DEL 	return TRUE;
//DEL }

int	CBaseLineTypeLib1::AddBaseLineType(BaseLineType def)
{
	int size = m_arrItems.GetSize();
	for (int i=size-1; i>=0; i--)
	{
		BaseLineType line = m_arrItems.GetAt(i);
		if (stricmp(def.m_name,line.m_name) == 0)
		{
			m_arrItems.RemoveAt(i);
		}
	}
	
	if (def.m_nSourceID != 0)
	{
		CArrayBaseLineType *pArrBaseLineType = NULL;
		if (m_mapSourceBaseLineTypes.Lookup(def.m_nSourceID,pArrBaseLineType))
		{
			for (int i=pArrBaseLineType->GetSize()-1; i>=0; i--)
			{
				BaseLineType line = pArrBaseLineType->GetAt(i);
				if (stricmp(def.m_name,line.m_name) == 0)
				{
					pArrBaseLineType->RemoveAt(i);
				}
			}
		}
		else
		{
			pArrBaseLineType = new CArrayBaseLineType;
			if (!pArrBaseLineType) return -1;
			
			m_mapSourceBaseLineTypes.SetAt(def.m_nSourceID,pArrBaseLineType);
		}

		if (pArrBaseLineType)
		{
			return (size+pArrBaseLineType->Add(def));
		}
	}
	else
	{
		return m_arrItems.Add(def);
	}	
	return -1;
}

int	CBaseLineTypeLib1::DelBaseLineType(LPCTSTR name)
{
	int size = m_arrItems.GetSize();

	CStringA name_a = ConvertTCharToChar(name);

	BaseLineType def;
	for (int i=0; i<size; i++)
	{
		def = m_arrItems.GetAt(i);
		if(stricmp(def.m_name,name_a) == 0)
		{
			m_arrItems.RemoveAt(i);
			m_CacheLType.Clear();
			return i;
		}
		
	}
	
	CArrayBaseLineType *pArrBaseLineType = NULL;
	if (m_mapSourceBaseLineTypes.Lookup(gnSourceID,pArrBaseLineType))
	{
		for (int i=0; i<pArrBaseLineType->GetSize(); i++)
		{
			def = pArrBaseLineType->GetAt(i);
			if(stricmp(def.m_name,name_a) == 0)
			{
				pArrBaseLineType->RemoveAt(i);
				m_CacheLType.Clear();
				return i+size;
			}
			
		}
	}
	
	return -1;
}


void CBaseLineTypeLib1::DelAllFromSourceID(DWORD id)
{
	if (id == 0)
	{
		m_arrItems.RemoveAll();
		return;
	}

	CArrayBaseLineType *pArrBaseLineType = NULL;
	if (m_mapSourceBaseLineTypes.Lookup(id,pArrBaseLineType))
	{
		pArrBaseLineType->RemoveAll();
		delete pArrBaseLineType;
		m_mapSourceBaseLineTypes.RemoveKey(id);
	}
}

void  CBaseLineTypeLib1::Clear()
{
	m_CacheLType.Clear();
	m_arrItems.RemoveAll();

	POSITION pos= m_mapSourceBaseLineTypes.GetStartPosition();
	while (pos != NULL)
	{
		DWORD sourceID = 0;
		CArrayBaseLineType *pArrBaseLineType = NULL;
		m_mapSourceBaseLineTypes.GetNextAssoc(pos,sourceID,pArrBaseLineType);
		if (pArrBaseLineType)
		{
			delete pArrBaseLineType;
			pArrBaseLineType = NULL;
		}
	}
	
	m_mapSourceBaseLineTypes.RemoveAll();
}

BaseLineType CBaseLineTypeLib1::GetBaseLineType(LPCTSTR name)
{
	int idx = GetBaseLineTypeIndex(name);
	if (idx < 0) return BaseLineType();

	return GetBaseLineType(idx);
}

BaseLineType CBaseLineTypeLib1::GetBaseLineType(int idx)
{
	int size = m_arrItems.GetSize();
	if (idx>=0 && idx<size)
	{
		return m_arrItems[idx];
	}
	else if (idx >= size)
	{
		CArrayBaseLineType *pArrBaseLineType = NULL;
		if (m_mapSourceBaseLineTypes.Lookup(gnSourceID,pArrBaseLineType) && pArrBaseLineType->GetSize() > (idx - size))
		{
			return pArrBaseLineType->ElementAt(idx-size);
		}
	}
	
	return BaseLineType();
}

int	CBaseLineTypeLib1::GetBaseLineTypeCount()
{
	int size = m_arrItems.GetSize();
	CArrayBaseLineType *pArrBaseLineType = NULL;
	if (m_mapSourceBaseLineTypes.Lookup(gnSourceID,pArrBaseLineType))
	{
		size += pArrBaseLineType->GetSize();
	}
	return size;
}

int CBaseLineTypeLib1::GetBaseLineTypeIndex(LPCTSTR name)
{
	BaseLineType def;
	int index = -1;
	if( m_CacheLType.Find(name,index) )
	{
		def = m_arrItems.GetAt(index);
		if( gnSourceID!=0 && def.m_nSourceID!=0 && gnSourceID!=def.m_nSourceID )
		{

		}
		else
			return index;
	}

	int size = m_arrItems.GetSize();

	CStringA name_a = ConvertTCharToChar(name);

	for (int i=0; i<size; i++)
	{
		def = m_arrItems.GetAt(i);
		if(stricmp(def.m_name,name_a) == 0)
		{
			m_CacheLType.AddToRecent(ConvertCharToTChar(def.m_name),i);
			return i;
		}		
	}
	
	CArrayBaseLineType *pArrBaseLineType = NULL;
	if (m_mapSourceBaseLineTypes.Lookup(gnSourceID,pArrBaseLineType))
	{
		for (int i=0; i<pArrBaseLineType->GetSize(); i++)
		{
			def = pArrBaseLineType->GetAt(i);
			if(stricmp(def.m_name,name_a) == 0)
			{
				int t = size + i;
				m_CacheLType.AddToRecent(ConvertCharToTChar(def.m_name),t);
				return t;
			}
			
		}
	}
	return -1;
}

BOOL CBaseLineTypeLib1::SetBaseLineType(int idx, BaseLineType line)
{
	if (idx < 0) return FALSE;

	int size = m_arrItems.GetSize();

	BaseLineType *pBaseLineType = NULL;
	if (idx>=0 && idx<size)
	{		
		pBaseLineType = &m_arrItems.ElementAt(idx);
	}
	else if (idx >= size)
	{
		CArrayBaseLineType *pArrBaseLineType = NULL;
		if (m_mapSourceBaseLineTypes.Lookup(gnSourceID,pArrBaseLineType) && pArrBaseLineType->GetSize() > (idx-size))
		{
			pBaseLineType = &pArrBaseLineType->ElementAt(idx-size);
		}
	}

	if (pBaseLineType)
	{
		if (stricmp(pBaseLineType->m_name,line.m_name) != 0)
		{
			m_CacheLType.Clear();
		}
		
		*pBaseLineType = line;

		return TRUE;
	}	
	
	return FALSE;

}

BOOL CBaseLineTypeLib1::SetBaseLineType(LPCTSTR name, BaseLineType line)
{
	if (name == NULL)
		return FALSE;
	
	int idx = GetBaseLineTypeIndex(name);
	if (idx < 0) return FALSE;

	return SetBaseLineType(idx, line);
	
}


CCellDefLib1* gpCellDefLib = NULL;
CBaseLineTypeLib1* gpBaseLineTypeLib = NULL;
DWORD gnSourceID = 0;


BOOL IsExist(LPCTSTR name, CStringArray &arrItems)
{
	BOOL bExist = FALSE;
	for(int i=0; i<arrItems.GetSize(); i++)
	{
		if (_tcsicmp(name,arrItems.GetAt(i)) == 0)
		{
			bExist = TRUE;
			break;
		}
	}
	return bExist;
}
/*
BOOL IsCellExist(LPCTSTR name, CCellDefLib1 *pCellDef)
{

	CCellDefLib1 *pCellDefLib = GetCellDefLib();
	if (pCellDef)
	{
		pCellDefLib = pCellDef;
	}
	if (!pCellDefLib)  return FALSE;
	
	CStringArray str;
	for (int i=0; i<pCellDefLib->GetCellDefCount(); i++)
		str.Add(pCellDefLib->GetCellDef(i).m_name);
	
	return IsExist(name,str);
}

BOOL IsLineExist(LPCTSTR name, CBaseLineTypeLib1 *pBaseLine)
{
	CBaseLineTypeLib1 *pLineTypeLib = GetBaseLineTypeLib();
	if (pBaseLine)
	{
		pLineTypeLib = pBaseLine;
	}
	if (!pLineTypeLib)  return FALSE;
	
	CStringArray str;
	for (int i=0; i<pLineTypeLib->GetBaseLineTypeCount(); i++)
		str.Add(pLineTypeLib->GetBaseLineType(i).m_name);
	
	return IsExist(name,str);
}

CCellDefLib1* GetCellDefLib()
{
	return gpCellDefLib;
}


CBaseLineTypeLib1* GetBaseLineTypeLib()
{
	return gpBaseLineTypeLib;
}


void SetCellDefLib(CCellDefLib1* pLib)
{
	gpCellDefLib = pLib;
}


void SetBaseLineTypeLib(CBaseLineTypeLib1* pLib)
{
	gpBaseLineTypeLib = pLib;
}

void SetLibSourceID(DWORD id)
{
	gnSourceID = id;
}

DWORD GetLibSourceID()
{
	return gnSourceID;
}


CString ExtractFileName(LPCTSTR pathname)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR extstr[_MAX_EXT];
	_tsplitpath(pathname, drive, dir, fname, extstr);
	return CString(fname);
}



CString ExtractFileNameExt(LPCTSTR pathname)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR extstr[_MAX_EXT];
	_tsplitpath(pathname, drive, dir, fname, extstr);
	return CString(fname) + extstr;
}


CString ExtractPath(LPCTSTR pathname)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR extstr[_MAX_EXT];
	_tsplitpath(pathname, drive, dir, fname, extstr);
	return CString(drive) + dir;
}
*/