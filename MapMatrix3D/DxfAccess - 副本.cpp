// DxfAccess.cpp: implementation of the CDxfRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GDIPlus\GdiPlus.h"
using namespace Gdiplus;
#include "EditBase.h"
#include "DxfAccess.h"
#include "FtrLayer.h"
#include "GeoPoint.h"
#include "GeoSurface.h"
#include "GeoCurve.h"
#include "GeoText.h"
#include "GeoDirPoint.h"
#include "GeoParallel.h"
#include "Feature.h"
#include "SmartViewBaseType.h"
#include "editbasedoc.h"
#include "DlgDataSource.h"
#include "SymbolLib.h"
#include "SmartViewFunctions.h"
#include "ExMessage.h"
#include "RegDef.h"
#include "RegDef2.h"
#include "PlotWChar.h"
#include "Functions_temp.h"
#include "SQLiteAccess.h"
#include <tchar.h>
#include "SmartViewFunctions.h"


//#include <afxcoll.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))
#define _FABS(x) ((x)>=0?(x):(-(x)))


void ReplaceChar(CString& str, const char *c1, const char *c2);
void ReplaceChar(char *str, const char *c1, const char *c2);

BOOL TrimRZero(CString& str);

extern BOOL CreateEntityArray(IAcadEntity *pEntity, VARIANT *pvar);

#define cadTextScale			0.707

typedef _com_ptr_t<IOdaLineType> IOdaLineTypePtr;


BYTE clrTable_CAD[] = {
0,0,0,
255,0,0,
255,255,0,
0,255,0,
0,255,255,
0,0,255,
255,0,255,
255,255,255,
65,65,65,
128,128,128,
255,0,0,
255,170,170,
189,0,0,
189,126,126,
129,0,0,
129,86,86,
104,0,0,
104,69,69,
79,0,0,
79,53,53,
255,63,0,
255,191,170,
189,46,0,
189,141,126,
129,31,0,
129,96,86,
104,25,0,
104,78,69,
79,19,0,
79,59,53,
255,127,0,
255,212,170,
189,94,0,
189,157,126,
129,64,0,
129,107,86,
104,52,0,
104,86,69,
79,39,0,
79,66,53,
255,191,0,
255,234,170,
189,141,0,
189,173,126,
129,96,0,
129,118,86,
104,78,0,
104,95,69,
79,59,0,
79,73,53,
255,255,0,
255,255,170,
189,189,0,
189,189,126,
129,129,0,
129,129,86,
104,104,0,
104,104,69,
79,79,0,
79,79,53,
191,255,0,
234,255,170,
141,189,0,
173,189,126,
96,129,0,
118,129,86,
78,104,0,
95,104,69,
59,79,0,
73,79,53,
127,255,0,
212,255,170,
94,189,0,
157,189,126,
64,129,0,
107,129,86,
52,104,0,
86,104,69,
39,79,0,
66,79,53,
63,255,0,
191,255,170,
46,189,0,
141,189,126,
31,129,0,
96,129,86,
25,104,0,
78,104,69,
19,79,0,
59,79,53,
0,255,0,
170,255,170,
0,189,0,
126,189,126,
0,129,0,
86,129,86,
0,104,0,
69,104,69,
0,79,0,
53,79,53,
0,255,63,
170,255,191,
0,189,46,
126,189,141,
0,129,31,
86,129,96,
0,104,25,
69,104,78,
0,79,19,
53,79,59,
0,255,127,
170,255,212,
0,189,94,
126,189,157,
0,129,64,
86,129,107,
0,104,52,
69,104,86,
0,79,39,
53,79,66,
0,255,191,
170,255,234,
0,189,141,
126,189,173,
0,129,96,
86,129,118,
0,104,78,
69,104,95,
0,79,59,
53,79,73,
0,255,255,
170,255,255,
0,189,189,
126,189,189,
0,129,129,
86,129,129,
0,104,104,
69,104,104,
0,79,79,
53,79,79,
0,191,255,
170,234,255,
0,141,189,
126,173,189,
0,96,129,
86,118,129,
0,78,104,
69,95,104,
0,59,79,
53,73,79,
0,127,255,
170,212,255,
0,94,189,
126,157,189,
0,64,129,
86,107,129,
0,52,104,
69,86,104,
0,39,79,
53,66,79,
0,63,255,
170,191,255,
0,46,189,
126,141,189,
0,31,129,
86,96,129,
0,25,104,
69,78,104,
0,19,79,
53,59,79,
0,0,255,
170,170,255,
0,0,189,
126,126,189,
0,0,129,
86,86,129,
0,0,104,
69,69,104,
0,0,79,
53,53,79,
63,0,255,
191,170,255,
46,0,189,
141,126,189,
31,0,129,
96,86,129,
25,0,104,
78,69,104,
19,0,79,
59,53,79,
127,0,255,
212,170,255,
94,0,189,
157,126,189,
64,0,129,
107,86,129,
52,0,104,
86,69,104,
39,0,79,
66,53,79,
191,0,255,
234,170,255,
141,0,189,
173,126,189,
96,0,129,
118,86,129,
78,0,104,
95,69,104,
59,0,79,
73,53,79,
255,0,255,
255,170,255,
189,0,189,
189,126,189,
129,0,129,
129,86,129,
104,0,104,
104,69,104,
79,0,79,
79,53,79,
255,0,191,
255,170,234,
189,0,141,
189,126,173,
129,0,96,
129,86,118,
104,0,78,
104,69,95,
79,0,59,
79,53,73,
255,0,127,
255,170,212,
189,0,94,
189,126,157,
129,0,64,
129,86,107,
104,0,52,
104,69,86,
79,0,39,
79,53,66,
255,0,63,
255,170,191,
189,0,46,
189,126,141,
129,0,31,
129,86,96,
104,0,25,
104,69,78,
79,0,19,
79,53,59,
51,51,51,
80,80,80,
105,105,105,
130,130,130,
190,190,190,
255,255,255 
};


//for dxf convert
int ConvertAlignmentAndJustification(int type, BOOL fdb2Cad)
{
	if (fdb2Cad)
	{
		int nJustification;
		switch(type)
		{
		case TAH_LEFT:
			nJustification = TJ_TOPLEFT;
			break;
		case TAH_LEFT|TAV_TOP:
			nJustification = TJ_TOPLEFT;
			break;
		case TAH_MID:
			nJustification = TJ_MIDDLE;			
			break;
		case TAH_MID|TAV_TOP:
			nJustification = TJ_TOPCENTER;			
			break;
		case TAH_RIGHT:
			nJustification = TJ_RIGHT;
			break;
		case TAH_RIGHT|TAV_TOP:
			nJustification = TJ_TOPRIGHT;			
			break;
		case TAH_LEFT|TAV_MID:
			nJustification = TJ_MIDDLELEFT;
			break;
		case TAH_MID|TAV_MID:
			nJustification = TJ_MIDDLECENTER;		
			break;
		case TAH_RIGHT|TAV_MID:	
			nJustification = TJ_MIDDLERIGHT;			
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			nJustification = TJ_BOTTOMLEFT;		
			break;
		case TAH_MID|TAV_BOTTOM:
			nJustification = TJ_BOTTOMCENTER;			
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			nJustification = TJ_BOTTOMRIGHT;
			break;
		default:
			nJustification = TJ_LEFT;			
			break;
		}
		
		return nJustification;
	}
	else
	{
		int nAlign;
		switch(type)
		{
		case TJ_LEFT:
			nAlign = TAH_LEFT|TAV_TOP;
			break;
		case TJ_TOPLEFT:
			nAlign = TAH_LEFT|TAV_TOP;
			break;
		case TJ_MIDDLE:
			nAlign = TAH_MID;			
			break;
		case TJ_TOPCENTER:
			nAlign = TAH_MID|TAV_TOP;			
			break;
		case TJ_RIGHT:
			nAlign = TAH_RIGHT;
			break;
		case TJ_TOPRIGHT:
			nAlign = TAH_RIGHT|TAV_TOP;			
			break;
		case TJ_MIDDLELEFT:
			nAlign = TAH_LEFT|TAV_MID;
			break;
		case TJ_MIDDLECENTER:
			nAlign = TAH_MID|TAV_MID;		
			break;
		case TJ_MIDDLERIGHT:	
			nAlign = TAH_RIGHT|TAV_MID;			
			break;
		case TJ_BOTTOMLEFT:
			nAlign = TAH_LEFT|TAV_BOTTOM;		
			break;
		case TJ_BOTTOMCENTER:
			nAlign = TAH_MID|TAV_BOTTOM;			
			break;
		case TJ_BOTTOMRIGHT:
			nAlign = TAH_RIGHT|TAV_BOTTOM;
			break;
		default:
			nAlign = TAH_LEFT;			
			break;
		}
		
		return nAlign;
	}
	
}


//for dwg convert
int ConvertAlignment_DWG(int type, BOOL fdb2Cad)
{
	if (fdb2Cad)
	{
		int nJustification;
		switch(type)
		{
		case TAH_LEFT:
		case TAH_LEFT|TAV_TOP:
			nJustification = acAlignmentTopLeft;
			break;
		case TAH_MID:
		case TAH_MID|TAV_TOP:
			nJustification = acAlignmentTopCenter;			
			break;
		case TAH_RIGHT:
		case TAH_RIGHT|TAV_TOP:
			nJustification = acAlignmentTopRight;			
			break;
		case TAH_LEFT|TAV_MID:
			nJustification = acAlignmentMiddleLeft;
			break;
		case TAH_MID|TAV_MID:
			nJustification = acAlignmentMiddleCenter;		
			break;
		case TAH_RIGHT|TAV_MID:	
			nJustification = acAlignmentMiddleRight;			
			break;
		case TAH_LEFT|TAV_BOTTOM:
		case TAV_BOTTOM:
			nJustification = acAlignmentBottomLeft;		
			break;
		case TAH_MID|TAV_BOTTOM:
			nJustification = acAlignmentBottomCenter;			
			break;
		case TAH_RIGHT|TAV_BOTTOM:
			nJustification = acAlignmentBottomRight;
			break;
		default:
			nJustification = acAlignmentBottomLeft;			
			break;
		}
		
		return nJustification;
	}
	else
	{
		int nAlign;
		switch(type)
		{
		case acAlignmentLeft:
		case acAlignmentBottomLeft:
			nAlign = TAH_LEFT|TAV_BOTTOM;
			break;
		case acAlignmentCenter:
		case acAlignmentBottomCenter:
			nAlign = TAH_MID|TAV_BOTTOM;
			break;
		case acAlignmentRight:
		case acAlignmentBottomRight:
			nAlign = TAH_RIGHT|TAV_BOTTOM;	
			break;
		case acAlignmentTopLeft:
			nAlign = TAH_LEFT|TAV_TOP;			
			break;
		case acAlignmentTopCenter:
			nAlign = TAH_MID|TAV_TOP;
			break;
		case acAlignmentTopRight:
			nAlign = TAH_RIGHT|TAV_TOP;			
			break;
		case acAlignmentMiddleLeft:
			nAlign = TAH_LEFT|TAV_MID;
			break;
		case acAlignmentMiddleCenter:
			nAlign = TAH_MID|TAV_MID;		
			break;
		case acAlignmentMiddleRight:	
			nAlign = TAH_RIGHT|TAV_MID;			
			break;
		default:
			nAlign = TAH_LEFT|TAV_BOTTOM;			
			break;
		}
		
		return nAlign;
	}
	
}



CComBSTR CharToWChar(const char *str)
{
	WCHAR str3[1024]={0};
	int num = MultiByteToWideChar(CP_ACP,0,str,strlen(str)+1,str3,1024);
	
	return CComBSTR(str3);
}


CString WCharToChar(const WCHAR *str)
{
	char str3[1024]={0};
	int num = WideCharToMultiByte(CP_ACP,0,str,wcslen(str)+1,str3,1024,NULL,NULL);
	
	return CString(str3);
}


COLORREF GetRGBOfCAD(int idx)
{
	idx = idx*3;
	if( idx<0 || idx>=sizeof(clrTable_CAD)/sizeof(clrTable_CAD[0]) )
		return RGB(255,255,255);
	return RGB(clrTable_CAD[idx],clrTable_CAD[idx+1],clrTable_CAD[idx+2]);
}

int ConverttoIndexOfCAD(COLORREF clr)
{
	int sum = sizeof(clrTable_CAD)/sizeof(clrTable_CAD[0]);
	int r = GetRValue(clr), g = GetGValue(clr), b = GetBValue(clr);
	int min=-1, cur, k=-1;
	for( int i=0; i<sum; i+=3)
	{
		cur = _FABS(r-clrTable_CAD[i])+_FABS(g-clrTable_CAD[i+1])+_FABS(b-clrTable_CAD[i+2]);
		if( min<0 || min>cur )
		{
			k = i/3;
			min = cur;
		}
		
		if( cur==0 )break;
	}
	
	return k;
}

struct TempObjForLidarPoint
{
	long layhdl;
	CFeature *pFtr;
};

struct TempObjForLidarPoint1
{
	CString  name;
	CFeature *pFtr;
};


void RemoveChar(char *str, char c);
void RemoveChar(char *str, const char *c);
void RemoveChar(CString& str, const char *c);

extern BOOL Create3DPointArray(PT_3D *pts, int num, VARIANT *pvar, int size, BOOL b3d);
extern BOOL Get3DPointArray(CArray<PT_3D,PT_3D> &pts, VARIANT *pvar,BOOL b3d);
extern void ArrayClear(VARIANT *pvar);

void CDxfRead::ImportLidarDxf()
{
	CArray<TempObjForLidarPoint,TempObjForLidarPoint> arrObjs;
	int				nMaxPtNum = 1000;
	CFtrLayer *pLayer = NULL;
	long lSum = 0;
	CFeature  *pFtr = NULL;
	CGeometry *pObj = NULL;
	PT_3DEX expt;

	CDxfDrawing &dxfFile = m_DxfDraw;

	if (m_bImportDWG)
	{
		if( m_pDb )
			goto DwgSection; 
		else
			return;
	}	

	LAYER Layer;
	
	int i;

	//获取实体对象总数
	lSum = 0;
	pLayer = m_pDS->GetFtrLayerByIndex(0);
	if( dxfFile.FindEntity(FIND_FIRST,NULL)>0 )
	{
		do
		{
			lSum++;
		}
		while( dxfFile.FindEntity( FIND_NEXT,NULL )>0 );
	}

	GProgressStart(lSum);

	//实体对象
	ENTITYHEADER	EntityHeader;
	char			EntityData[4096];

	memset(&EntityHeader,0,sizeof(EntityHeader));
	memset(&EntityData,0,sizeof(EntityData));
	memset(&Layer,0,sizeof(Layer));

	TempObjForLidarPoint item;
	
	Layer.Objhandle = -1;
	pLayer = m_pDS->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));

	if( dxfFile.FindEntity(FIND_FIRST,NULL)>0 && pLayer!=NULL )
	{
		do
		{
			dxfFile.GetCurEntityHeader(&EntityHeader);
			dxfFile.GetCurEntityData(EntityData);

			if( EntityHeader.EntityType!=ENT_POINT )
				continue;

			//层发生变化
			if( Layer.Objhandle!=EntityHeader.LayerObjhandle || pObj==NULL || (pObj!=NULL && pObj->GetDataPointSum()>nMaxPtNum) )
			{
				if( Layer.Objhandle!=EntityHeader.LayerObjhandle )
				{
					Layer.Objhandle = EntityHeader.LayerObjhandle;
					dxfFile.FindTableType(TAB_LAYER,FIND_BYHANDLE,&Layer);
				}

				//查找合适的多点对象
				BOOL bFindProper = FALSE;
				int nSize = arrObjs.GetSize();
				for( i=0; i<nSize; i++ )
				{
					TempObjForLidarPoint t = arrObjs.GetAt(i);
					if( t.layhdl==EntityHeader.LayerObjhandle && 
						t.pFtr!=NULL && t.pFtr->GetGeometry()!=NULL &&
						t.pFtr->GetGeometry()->GetDataPointSum()<nMaxPtNum )
					{
						pFtr = t.pFtr;
						pObj = pFtr->GetGeometry();
						bFindProper = TRUE;
						break;
					}
				}

				//如果没有找到，就新建一个多点对象
				if( !bFindProper )
				{
					pFtr = pLayer->CreateDefaultFeature(m_pDS->GetScale(),CLS_GEOMULTIPOINT);
					if( !pFtr )continue;

					pObj = pFtr->GetGeometry();
					if (!pFtr) continue;

					item.layhdl = Layer.Objhandle;
					item.pFtr = pFtr;
					
					arrObjs.Add(item);
				}
			}

			if( !pFtr || !pObj)continue;

			if( EntityHeader.Color<=0 || EntityHeader.Color>255 )
			{
				EntityHeader.Color = 255;
			}
			COLORREF col = GetRGBOfCAD(EntityHeader.Color);
			//设置颜色和点数据
			pObj->SetColor(col);

			PENTPOINT pp = (PENTPOINT)EntityData;
			PT_3DEX pt;
			COPY_3DPT(pt,pp->Point0);
			pt.pencode = penLine;
			CArray<PT_3DEX,PT_3DEX> pts;
			pObj->GetShape(pts);
			pts.Add(pt);
			pObj->CreateShape(pts.GetData(),pts.GetSize());

			//增长进度条
			GProgressStep();
		}
		while( dxfFile.FindEntity( FIND_NEXT,NULL )>0 );
	}

	//将多点对象添加到文档中
	for( i=0; i<arrObjs.GetSize(); i++ )
	{
		pFtr = arrObjs.GetAt(i).pFtr;
		if (!pFtr) continue;
		pObj = pFtr->GetGeometry();
		if (!pObj) continue;

		if( pObj->GetDataPointSum()>0 )
		{
			//图幅名
			CString strMapName = m_dlgDxfImport.m_strMapName;
			//层名
			
			OBJHANDLE objLayer = arrObjs.GetAt(i).layhdl;
			LAYER Layer;
			Layer.Objhandle = objLayer;
			if(m_DxfDraw.FindTableType(TAB_LAYER,FIND_BYHANDLE,&Layer) < 0)
				continue;
			
			if (Layer.Name == NULL)  continue;
			CString strLayerName;
			strLayerName = m_lstFile.FindMatchItem(Layer.Name,1,0);
			if(strLayerName.IsEmpty())
				strLayerName = Layer.Name;

			CString tname = strLayerName;
			
			if(m_dlgDxfImport.m_bUseIndex)
			{
				__int64 code = _atoi64(strLayerName);
				const char *rname = NULL;
				CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
				CConfigLibManager *pCfgLibManager = pApp->GetConfigLibManager();
				if (pCfgLibManager->GetScheme(m_pDS->GetScale())->FindLayerIdx(TRUE,code,rname))
					strLayerName = rname;
			}
			
			if (strLayerName.IsEmpty()) continue;
			CFtrLayer *pFtrLay = m_pDS->GetFtrLayer(strLayerName/*, strMapName*/);
			
			if (pFtrLay == NULL)
			{				
				pFtrLay = m_pDS->CreateFtrLayer(strLayerName);
				if (!pFtrLay)  continue;
				COLORREF col = GetRGBOfCAD(Layer.Color);
				pFtrLay->SetColor(col);
				m_pDS->AddFtrLayer(pFtrLay);
			}
			
			pFtr->SetCode(strMapName);

			m_pDS->AddObject(pFtr,pFtrLay->GetID());
		}
		else
			delete pFtr;
	}

	//进度条复位
	GProgressEnd();

	return;

DwgSection:

	CArray<TempObjForLidarPoint1,TempObjForLidarPoint1> arrObjs1;

	VARIANT var;

	//获取实体对象总数
	m_pMS->get_Count(&lSum);

	GProgressStart(lSum);

	TempObjForLidarPoint1 item1;
	
	pLayer = m_pDS->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));

	if (!pLayer) return;

	IAcadEntity *pEntity = NULL;
	HRESULT hr;
	
	CString lastLayer = "-1";

	BOOL bRelease = TRUE;
	for (int ent=0; ent<lSum; ent++)
	{
		GProgressStep();

		if (!bRelease && pEntity)
		{
			pEntity->Release();
			bRelease = TRUE;
		}

		hr = m_pMS->Item((_variant_t)(long)ent,&pEntity);
		if(FAILED(hr))continue;

		bRelease = FALSE;

		BSTR type;
		pEntity->get_ObjectName(&type);

		_bstr_t tmpbstr(type,FALSE);
		
		const char *pType = (const char*)tmpbstr;

		if (stricmp(pType,"AcDbPoint") != 0) continue;

		//层名
		BSTR bName;
		pEntity->get_Layer(&bName);

		_bstr_t tmpbstr1(bName,FALSE);
		
		CString LayerName = (char*)tmpbstr1;
		
		//层发生变化
		if( lastLayer.CompareNoCase(LayerName) != 0 || pObj==NULL || (pObj!=NULL && pObj->GetDataPointSum()>nMaxPtNum) )
		{
			if( lastLayer.CompareNoCase(LayerName) != 0 )
			{
				lastLayer = LayerName;
			}
			
			//查找合适的多点对象
			BOOL bFindProper = FALSE;
			int nSize = arrObjs1.GetSize();
			for( i=0; i<nSize; i++ )
			{
				TempObjForLidarPoint1 t = arrObjs1.GetAt(i);
				if( t.name.CompareNoCase(LayerName)==0 && 
					t.pFtr!=NULL && t.pFtr->GetGeometry()!=NULL &&
					t.pFtr->GetGeometry()->GetDataPointSum()<nMaxPtNum )
				{
					pFtr = t.pFtr;
					pObj = pFtr->GetGeometry();
					bFindProper = TRUE;
					break;
				}
			}
			
			//如果没有找到，就新建一个多点对象
			if( !bFindProper )
			{
				pFtr = pLayer->CreateDefaultFeature(m_pDS->GetScale(),CLS_GEOMULTIPOINT);
				if( !pFtr )continue;
				
				pObj = pFtr->GetGeometry();
				if (!pFtr) continue;
				
				item1.name = LayerName;
				item1.pFtr = pFtr;
				
				arrObjs1.Add(item1);
			}
		}
		
		if( !pFtr || !pObj)continue;
		
		ACAD_COLOR color;
		pEntity->get_color(&color);
		
		long col;
		if (color == acByLayer)
		{
			col = -1;
		}
		else
		{
			col = GetRGBOfCAD(color);
		}
		//设置颜色和点数据
		pObj->SetColor(col);
		
		PT_3DEX pt;
		
		IAcadPoint *pPoint = NULL;
		hr = pEntity->QueryInterface(&pPoint);			
		if(FAILED(hr))continue;			
		
		hr = pPoint->get_Coordinates(&var);			
		
		CArray<PT_3D,PT_3D> arr;
		Get3DPointArray(arr,&var);
		if (arr.GetSize() <= 0)
		{
			pPoint->Release();
			continue;
		}
		COPY_3DPT(pt,arr[0]);
		pt.pencode = penLine;
		CArray<PT_3DEX,PT_3DEX> pts;
		pObj->GetShape(pts);
		pts.Add(pt);
		pObj->CreateShape(pts.GetData(),pts.GetSize());

		pPoint->Release();

		pEntity->Release();
		bRelease = TRUE;
		
	}

	//将多点对象添加到文档中
	for( i=0; i<arrObjs1.GetSize(); i++ )
	{
		pFtr = arrObjs1.GetAt(i).pFtr;
		if (!pFtr) continue;
		pObj = pFtr->GetGeometry();
		if (!pObj) continue;

		if( pObj->GetDataPointSum()>0 )
		{
			//图幅名
			CString strMapName = m_dlgDxfImport.m_strMapName;
			//层名
			CString Name = arrObjs1.GetAt(i).name;
			
			if (Name.IsEmpty())  continue;

			CString strLayerName;
			strLayerName = m_lstFile.FindMatchItem(Name,1,0);
			if(strLayerName.IsEmpty())
				strLayerName = Name;

			CString tname = strLayerName;
			
			if(m_dlgDxfImport.m_bUseIndex)
			{
				__int64 code = _atoi64(strLayerName);
				const char *rname = NULL;
				CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
				CConfigLibManager *pCfgLibManager = pApp->GetConfigLibManager();
				if (pCfgLibManager->GetScheme(m_pDS->GetScale())->FindLayerIdx(TRUE,code,rname))
					strLayerName = rname;
			}
			
			if (strLayerName.IsEmpty()) continue;
			CFtrLayer *pFtrLay = m_pDS->GetFtrLayer(strLayerName/*, strMapName*/);
			
			if (pFtrLay == NULL)
			{				
				pFtrLay = m_pDS->CreateFtrLayer(strLayerName);
				if (!pFtrLay)  continue;
				IAcadLayer *pLayer = FindDwgLayer(Name);
				if (!pLayer) continue;
				
				AcColor color;
				pLayer->get_color(&color);
				pLayer->Release();
				
				pFtrLay->SetColor(GetRGBOfCAD(color));
				m_pDS->AddFtrLayer(pFtrLay);
			}
			
			pFtr->SetCode(strMapName);

			m_pDS->AddObject(pFtr,pFtrLay->GetID());
		}
		else
			delete pFtr;
	}

	//进度条复位
	GProgressEnd();

	return;
}










CDxfRead::CDxfRead(CDlgDoc *pDoc, BOOL bUndo)
{
	m_pDS = NULL;
	m_bImportDWG = FALSE;
	m_dwg = NULL;
	m_iHost = NULL;
	m_iApplication = NULL;
    m_pCurDocument = NULL;
	m_pDb = NULL;
	m_pMS = NULL;
	m_pLayers = NULL;
	m_pDoc = pDoc;

	m_pUndo = NULL;
	if (bUndo && m_pDoc)
	{
		m_pUndo = new CUndoFtrs(m_pDoc,_T("Import CAD"));
	}
}

CDxfRead::~CDxfRead()
{
	CloseRead();
	if (m_pUndo)
	{
		delete m_pUndo;
		m_pUndo = NULL;
	}
		
}

BOOL CDxfRead::OpenRead(CString dxfPath)
{
	m_strFileName = dxfPath;//m_dlgDxfImport.m_strDxfFile;
	m_lstFile.Open(m_dlgDxfImport.m_strLstFile);

	if (!m_bImportDWG)
	{
		if( m_DxfDraw.isOpen() )
		{
			m_DxfDraw.Destroy();
		}
		
		m_DxfDraw.Create();
		return m_DxfDraw.LoadDXFFile(m_strFileName);
	}
	else
	{
		BeginCheck41License

		HRESULT        hr;
		IClassFactory  *pCF;
		CoGetClassObject(__uuidof(OdaHostApp), CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void **)&pCF);
		if (!pCF)
		{
			AfxMessageBox("can't find dwg module,please register the module!");
			return FALSE;
		}
		hr = pCF->CreateInstance(NULL, IID_IOdaHostApp, (void **)&m_iHost);
		if (CLASS_E_NOTLICENSED == hr)
		{
			IClassFactory2  *pFactory;
			
			hr = pCF->QueryInterface(IID_IClassFactory2, (void **)&pFactory);
			if (S_OK == hr)
			{
				hr = pFactory->CreateInstanceLic(NULL, NULL, IID_IOdaHostApp, (BSTR)_bstr_t("some key"), (void **)&m_iHost);
				pFactory->Release();
			}
		}
		pCF->Release();
		
		if (S_OK != hr)
			return FALSE;
		
		hr = m_iHost->get_Application(&m_iApplication);
		if (S_OK != hr)
		{
			m_iHost->Release();
			m_iHost = NULL;
			return FALSE;
		}
		
		IAcadDocuments *pDocuments;
		// read this file 
		hr = m_iApplication->get_Documents(&pDocuments);
		if( FAILED(hr) )return FALSE;
		
		_variant_t      vPassword;
		BSTR str = m_strFileName.AllocSysString();
		hr = pDocuments->Open(str, _variant_t(true), vPassword, &m_pCurDocument);
		SysFreeString(str);
		
		pDocuments->Release();
		
		if (FAILED(hr)) return FALSE;
		
		hr = m_pCurDocument->get_Database(&m_pDb);
		if (FAILED(hr)) return FALSE;
		
		hr = m_pDb->get_ModelSpace(&m_pMS);		
		if (FAILED(hr)) return FALSE;

		hr = m_pDb->get_Layers(&m_pLayers);		
		if (FAILED(hr)) return FALSE;

		EndCheck41License
	}

	return TRUE;
}

BOOL CDxfRead::CloseRead()
{
	if (!m_bImportDWG)
	{
		if( m_DxfDraw.isOpen() )
		{
			m_DxfDraw.Destroy();
		}
	}
	else
	{
		if (m_pMS)
		{
			m_pMS->Release();
			m_pMS = NULL;
		}

		if (m_pLayers)
		{
			m_pLayers->Release();
			m_pLayers = NULL;
		}
		
		if (m_pDb)
		{
			m_pDb->Release();
			m_pDb = NULL;
		}
		
		if (m_pCurDocument)
		{
			m_pCurDocument->Release();
			m_pCurDocument = NULL;
		}
		
		if (m_iApplication)
		{
			m_iApplication->Quit();
			m_iApplication->Release();
			m_iApplication = NULL;
		}
		
		if (m_iHost)
		{
			m_iHost->Release();
			m_iHost = NULL;
		}
		
	}
	m_lstFile.Clear();
	
	return TRUE;
}


int CDxfRead::GetFtrLayers(CPtrArray &FtrLays)
{
	return 0;
}

IAcadLayer* CDxfRead::FindDwgLayer(CString name)
{
	if (!m_pLayers) return NULL;
	
	HRESULT hr;
	long size;
	m_pLayers->get_Count(&size);
	IAcadLayer *pLayer = NULL;
	for (int i=0; i<size; i++)
	{
		hr = m_pLayers->Item((_variant_t)(long)(i),&pLayer);
		if (FAILED(hr)) continue;
		
		BSTR   bName;			
		pLayer->get_Name(&bName);
		
		_bstr_t tmpstr = _bstr_t(bName,FALSE);
		
		CString strLayerName = (const char*)tmpstr;
		
		if (strLayerName.CompareNoCase(name) == 0)
		{
			break;
		}
		
		pLayer->Release();
	}
	
	if( i>=size )
		return NULL;

	return pLayer;
	
}

//计算凸度弧的中点
static void GetCenterOf3PArc(double dStarX, double dStarY, double dEndX, double dEndY, double dConvexityDegree, double *retx, double *rety)
{
	if (!retx || !rety) return;
	//斜率
	double k = (dEndY - dStarY) / (dEndX - dStarX);
	//弦中点
	double mid_x = (dStarX + dEndX) / 2.0;
	double mid_y = (dStarY + dEndY) / 2.0;

	//弦长的平方
	double dLength2 = pow(dStarX - dEndX, 2) + pow(dStarY - dEndY, 2);
	//弧中点到弦的距离的平方
	double dis2 = dLength2*0.25*dConvexityDegree*dConvexityDegree;

	double A = sqrt(dis2 / (1 + k*k));
	double cenx = mid_x - k*A;
	double ceny = mid_y + A;

	PT_3D pts[3];
	pts[0].x = dStarX; pts[0].y = dStarY;
	pts[1].x = cenx; pts[1].y = ceny;
	pts[2].x = dEndX; pts[2].y = dEndY;
	int ret = GraphAPI::GIsClockwise(pts, 3);
	if (dConvexityDegree < 0 && ret == 1)
	{
		*retx = cenx;
		*rety = ceny;
	}
	else if (dConvexityDegree > 0 && ret == 0)
	{
		*retx = cenx;
		*rety = ceny;
	}
	else
	{
		*retx = mid_x + k*A;
		*rety = mid_y - A;
	}
}

BOOL CDxfRead::ReadFeatures()
{
	CScheme *pScheme = NULL;
	CString strMapName;
	long lObjSum = 0;

	if (m_bImportDWG)
	{
		if( m_pDb )
			return ReadFeatures_dwg(); 
		else
			return FALSE;
	}

	if(!m_DxfDraw.isOpen())
		return FALSE;

	ReadBlocks_dxf();
	
	lObjSum = 0;
	if( m_DxfDraw.FindEntity(FIND_FIRST,NULL)>0 )
	{
		do
		{
			lObjSum++;
		} while (m_DxfDraw.FindEntity(FIND_NEXT,NULL) > 0);
	}
	GProgressStart(lObjSum);

	//实体对象
	ENTITYHEADER	EntityHeader;
	char			EntityData[4096];

	//图幅名
	strMapName = m_dlgDxfImport.m_strMapName;
// 	int slashIndex = m_strFileName.ReverseFind('\\');
// 	strMapName = m_strFileName.Mid(slashIndex+1);
	pScheme = gpCfgLibMan->GetScheme(m_pDS->GetScale());
	 
	if( m_DxfDraw.FindEntity(FIND_FIRST,NULL)>0 )
	{
		do 
		{
			GProgressStep();			 
			
			m_DxfDraw.GetCurEntityHeader(&EntityHeader);
			m_DxfDraw.GetCurEntityData(EntityData);

			//层名
			OBJHANDLE objLayer = EntityHeader.LayerObjhandle;
			LAYER Layer;
			Layer.Objhandle = objLayer;
			if(m_DxfDraw.FindTableType(TAB_LAYER,FIND_BYHANDLE,&Layer) < 0)
				continue;

			CString strLayerName;
			if (!m_dlgDxfImport.m_strLstFile.IsEmpty())
			{
				strLayerName = m_lstFile.FindMatchItem(Layer.Name,1,0);
			}			
			if (strLayerName.IsEmpty())
				strLayerName = Layer.Name;

			CString tname = strLayerName;
			
			if (m_dlgDxfImport.m_bUseIndex)
			{
				__int64 code = _atoi64(strLayerName);
				const char *rname = NULL;
				if (!pScheme->FindLayerIdx(TRUE,code,rname))
					strLayerName = rname;
			}
			
			if (strLayerName.IsEmpty()) continue;
			CFtrLayer *pFtrLay = m_pDS->GetFtrLayer(strLayerName/*, strMapName*/);

			if (pFtrLay == NULL)
			{				
				pFtrLay = m_pDS->CreateFtrLayer(strLayerName);
				if (!pFtrLay)  continue;
				//pFtrLay->SetMapName(strMapName);
				COLORREF col = GetRGBOfCAD(Layer.Color);
				pFtrLay->SetColor(col);
				m_pDS->AddFtrLayer(pFtrLay);			
			}

			if (!m_pDS->SetCurFtrLayer(pFtrLay->GetID()))
				continue;

			//如果导入了Lidar点数据，这里对点数据就不作处理了
			if (m_dlgDxfImport.m_bLidarPoints && EntityHeader.EntityType==ENT_POINT)
			{
				continue;
			}			
			
			double all_z = EntityHeader.Height;
			BOOL bUse_AllZ = (all_z!=0);
			
			//地物类型
			int nType;
			switch (EntityHeader.EntityType)
			{
			case ENT_LINE:
			case ENT_CIRCLE:
			case ENT_SHAPE:
			case ENT_SPLINE:
			case ENT_ARC:
				nType = CLS_GEOCURVE;
				break;
			case ENT_POLYLINE:
				{
					PENTPOLYLINE pl = (PENTPOLYLINE)EntityData;
					if( pl->Flag&1 )
					{
						nType = CLS_GEOSURFACE;
					}
					else 
					{
						nType = CLS_GEOCURVE;
					}
				}
				break;
			case ENT_POINT:
			case ENT_INSERT:
				nType = CLS_GEOPOINT;
				break;
			case ENT_TEXT:
			case ENT_MTEXT:
				nType = CLS_GEOTEXT;
				break;
			case ENT_FACE3D:
				nType = CLS_GEOSURFACE;
				break;
			default:
				nType = 0;
			}	

			//纠正可能的类型误判(因为CAD没有严格的面和线的区分)
			CSchemeLayerDefine *pItem = pScheme->GetLayerDefine(pFtrLay->GetName());
			if (pItem)
			{
				int nLayerClass = pItem->GetGeoClass();
				if( nLayerClass==CLS_GEOSURFACE && nType==CLS_GEOCURVE  )
				{
					nType = CLS_GEOSURFACE;
				}
				else if( nLayerClass==CLS_GEOCURVE && nType==CLS_GEOSURFACE  )
				{
					nType = CLS_GEOCURVE;
				}
				else if( (nLayerClass==CLS_GEOPOINT || nLayerClass==CLS_GEODIRPOINT) && nType==CLS_GEOCURVE )
				{
					nType = CLS_GEODIRPOINT;
				}
				
			}

			//获取数据
			double fAng = 0;    // 点角度
			CArray<PT_3DEX,PT_3DEX> arrPts;
			switch( EntityHeader.EntityType )
			{
			case ENT_SHAPE:
				break;
			case ENT_ARC:
				{
					PENTARC pa = (PENTARC)EntityData;
					PT_3DEX pt;
					pt.pencode = penArc;
					pt.x = pa->Point0.x + pa->Radius * cos(pa->StartAngle*PI/180);
					pt.y = pa->Point0.y + pa->Radius * sin(pa->StartAngle*PI/180);
					pt.z = pa->Point0.z;
					arrPts.Add(pt);
					
					if (pa->StartAngle < pa->EndAngle)
					{
						pt.x = pa->Point0.x + pa->Radius * cos((pa->StartAngle+pa->EndAngle)*PI/180/2);
						pt.y = pa->Point0.y + pa->Radius * sin((pa->StartAngle+pa->EndAngle)*PI/180/2);
						pt.z = pa->Point0.z;
						arrPts.Add(pt);
					}
					else
					{
						pt.x = pa->Point0.x + pa->Radius * cos((pa->StartAngle+pa->EndAngle)*PI/180/2+PI);
						pt.y = pa->Point0.y + pa->Radius * sin((pa->StartAngle+pa->EndAngle)*PI/180/2+PI);
						pt.z = pa->Point0.z;
						arrPts.Add(pt);

					}
					
					pt.x = pa->Point0.x + pa->Radius * cos(pa->EndAngle*PI/180);
					pt.y = pa->Point0.y + pa->Radius * sin(pa->EndAngle*PI/180);
					pt.z = pa->Point0.z;
					arrPts.Add(pt);
				}
				break;
			case ENT_CIRCLE:
				{
					PENTCIRCLE pa = (PENTCIRCLE)EntityData;
					PT_3DEX pt;
					pt.pencode = penArc;
					pt.x = pa->Point0.x + pa->Radius;
					pt.y = pa->Point0.y;
					pt.z = pa->Point0.z;
					arrPts.Add(pt);
					
					pt.x = pa->Point0.x;
					pt.y = pa->Point0.y + pa->Radius;
					pt.z = pa->Point0.z;
					arrPts.Add(pt);
					
					pt.x = pa->Point0.x - pa->Radius;
					pt.y = pa->Point0.y;
					pt.z = pa->Point0.z;
					arrPts.Add(pt);
					
					pt.x = pa->Point0.x + pa->Radius;
					pt.y = pa->Point0.y;
					pt.z = pa->Point0.z;
					arrPts.Add(pt);
				}
				break;
			case ENT_LINE:
				{
					PENTLINE pl = (PENTLINE)EntityData;
					PT_3DEX pt;
					pt.pencode = penLine;
					
					COPY_3DPT(pt,pl->Point0);
					arrPts.Add(pt);
					COPY_3DPT(pt,pl->Point1);
					arrPts.Add(pt);
				}
				break;
			case ENT_POLYLINE:
				{
					PENTPOLYLINE pl = (PENTPOLYLINE)EntityData;
					//int pos = arrPts.GetSize();
					for( int i=0; i<pl->nVertex; i++)
					{
						// 过滤拟合创建的额外结点
						if (pl->pVertex[i].Flag&1 || pl->pVertex[i].Flag&8)
						{
							continue;
						}

						PT_3DEX pt;
						if( (pl->Flag&8) || (pl->Flag&16) )
						{
							COPY_3DPT(pt,pl->pVertex[i].Point);
						}
						//二维点
						else
						{
							COPY_3DPT(pt,pl->pVertex[i].Point);
							if( bUse_AllZ )pt.z = all_z;
						}
						
						if (pl->pVertex[i].Flag&16)
						{
							pt.pencode = penSpline;
						}
						else
						{
							pt.pencode = penLine;
						}

						double va = pl->pVertex[i].Bulge;
						//带凸度的多段线转成三点弧
						if (fabs(va) > 0.000001 && arrPts.GetSize() > 0)
						{
							PT_3DEX start = arrPts[arrPts.GetSize() - 1];
							PT_3DEX ret;
							GetCenterOf3PArc(start.x, start.y, pt.x, pt.y, va, &ret.x, &ret.y);
							ret.z = (start.z + pt.z) / 2;
							ret.pencode = pen3PArc;
							arrPts.Add(ret);

							pt.pencode = pen3PArc;
						}

						arrPts.Add(pt);
					}
					
					//闭合
					int num = arrPts.GetSize();
					if( (pl->Flag&1) && num > 2)
					{
						arrPts.Add(arrPts[0]);
					}
				}
				break;
			case ENT_SPLINE:
				{
					PENTSPLINE pl = (PENTSPLINE)EntityData;
					
					for( int i=0; i<pl->ControlPointsNum; i++)
					{
						PT_3DEX pt;
						COPY_3DPT(pt,pl->pControlPoints[i].Point);
						
						pt.pencode = penSpline;
						arrPts.Add(pt);
					}

					ConvertCADSpline(arrPts);
					
					//闭合
					int num = arrPts.GetSize();
					if( (pl->SplineFlag&1) && nType == CLS_GEOCURVE && num > 2)
					{
						arrPts.Add(arrPts[0]);
					}
				}
				break;
			case ENT_FACE3D:
				{
					PENT3DFACE pl = (PENT3DFACE)EntityData;
					PT_3DEX pt;
					pt.pencode = penMove;
					COPY_3DPT(pt,pl->Point0);
					arrPts.Add(pt);
					pt.pencode = penLine;
					COPY_3DPT(pt,pl->Point1);
					arrPts.Add(pt);
					COPY_3DPT(pt,pl->Point2);
					arrPts.Add(pt);
					COPY_3DPT(pt,pl->Point3);
					arrPts.Add(pt);
				}
				break;
			case ENT_POINT:
				{
					PENTPOINT pp = (PENTPOINT)EntityData;
					PT_3DEX pt;
					COPY_3DPT(pt,pp->Point0);
					pt.pencode = penLine;
					arrPts.Add(pt);		
					fAng = pp->Angle;
				}
				break;
			case ENT_INSERT:
				{
					PENTINSERT pp = (PENTINSERT)EntityData;
					PT_3DEX pt;
					COPY_3DPT(pt,pp->Point0);
					pt.pencode = penLine;
					arrPts.Add(pt);	
					fAng = pp->RotationAngle;
				}
				break;
			case ENT_TEXT:
				{
					PENTTEXT pt = (PENTTEXT)EntityData;
					PT_3DEX expt;
					COPY_3DPT(expt,pt->Point0);
					expt.pencode = penLine;
					arrPts.Add(expt);			
				}
				break;
			case ENT_MTEXT:
				{
					PENTMTEXT pt = (PENTMTEXT)EntityData;
					PT_3DEX expt;
					COPY_3DPT(expt,pt->Point0);
					expt.pencode = penLine;
					arrPts.Add(expt);
				}
				break;
			}

			BOOL bDirPoint = (fabs(fAng)>1e-4);
			if (bDirPoint)
			{
				nType = CLS_GEODIRPOINT;
			}

			CFeature *pFtr = pFtrLay->CreateDefaultFeature(m_pDS->GetScale(), nType);
			
			if (!pFtr || !pFtr->GetGeometry())   continue;
			
			// 如果地物与当前层缺省数据类型不一致就不导入（过滤导出的注记导入)
			{
				if (pItem && ( (m_dlgDxfImport.m_bUnImport && pItem->GetGeoClass() != pFtr->GetGeometry()->GetClassType()) ||
					(!m_dlgDxfImport.m_bImportAnno && pItem->GetGeoClass() != CLS_GEOTEXT && pFtr->GetGeometry()->GetClassType() == CLS_GEOTEXT) ) )
				{
					delete pFtr;
					pFtr = NULL;
					continue;
				}
				
			}

			BOOL bResult = pFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());

			if (bDirPoint)
			{
				((CGeoDirPoint*)pFtr->GetGeometry())->SetDirection(fAng);
			}

			//创建失败
			if (pFtr->GetGeometry()->GetDataPointSum() < 1)
			{
				delete pFtr;
				pFtr = NULL;
				continue;
			}
						
			if( nType==CLS_GEOPOINT && EntityHeader.EntityType==ENT_INSERT )
			{
				PENTINSERT pi = (PENTINSERT)EntityData;
				CGeoPoint *pGeo = (CGeoPoint*)pFtr->GetGeometry();
				
				pGeo->m_fKx = pi->XScale;
				pGeo->m_fKy = pi->YScale;
				pGeo->m_lfAngle = pi->RotationAngle;
				
				BLOCKHEADER blkHeader;
				blkHeader.Objhandle = pi->BlockHeaderObjhandle;
				if( !m_dlgDxfImport.m_bBlockAsPoint && m_DxfDraw.FindBlock(FIND_BYHANDLE,&blkHeader)>0 )
				{
					//跳过 *Model_Space 等等
					if( blkHeader.Name[0]=='*' )
					{
						delete pFtr;
						pFtr = NULL;
						continue;
					}

					if(m_dlgDxfImport.m_bSaveLinetype)
					{
						CString symname = blkHeader.Name;
						symname = "@CAD_" + symname;
						pGeo->SetSymbolName(symname);
					}
				}						
			}
			else if (nType == CLS_GEOTEXT)
			{
				if (EntityHeader.EntityType == ENT_TEXT)
				{
					PENTTEXT pt = (PENTTEXT)EntityData;
					CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
					pText->SetText(pt->strText);
					TEXT_SETTINGS0 setting;
					setting.fHeight = pt->TextData.Height/cadTextScale/GetSymbolDrawScale();
					setting.fTextAngle = pt->TextData.RotationAngle;
				
					if (pt->TextData.WidthFactor > 0)
						setting.fWidScale = (double)pt->TextData.WidthFactor;
					else
						setting.fWidScale = 1;

					CString font = ReadTextFontName_dxf(pt->TextData.TextStyleObjhandle);
					if( !font.IsEmpty() )
						strcpy(setting.strFontName,font);

					setting.fInclineAngle = pt->TextData.Oblique;
					if( pt->TextData.Oblique!=0.0 )
						setting.nInclineType = SHRUGR;

					setting.nAlignment = ConvertAlignmentAndJustification(pt->TextData.Justification,FALSE);

					pText->SetSettings(&setting);
				}
				else if (EntityHeader.EntityType == ENT_MTEXT)
				{
					PENTMTEXT pt = (PENTMTEXT)EntityData;
					CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
					pText->SetText(pt->strText);
					TEXT_SETTINGS0 setting;
					setting.fHeight = pt->TextData.Height/0.6/GetSymbolDrawScale();	
					setting.fTextAngle = pt->TextData.RotationAngle;
					
					pText->SetSettings(&setting);
				}				

			}

			if( m_dlgDxfImport.m_bSaveLinetype && (nType==CLS_GEOCURVE || nType==CLS_GEOSURFACE ) )
			{
				OBJHANDLE hLinetype = EntityHeader.LTypeObjhandle;
				if( hLinetype!=0 )
				{
					LTYPE ltype;
					ltype.Objhandle = hLinetype;
					if( m_DxfDraw.FindTableType(TAB_LTYPE,FIND_BYHANDLE,&ltype)!=0 )
					{
						if( stricmp(ltype.Name,"continuous")==0 || stricmp(ltype.Name,"bylayer")==0 )
						{

						}
						else if( strlen(ltype.Name)>0 )
						{
							pFtr->GetGeometry()->SetSymbolName(CString("CAD_")+ltype.Name);
						}						
					}
				}
			}
		
			if ( EntityHeader.Color<=0 || (EntityHeader.Color>255 && EntityHeader.Color!=256) )
			{
				EntityHeader.Color = 255;
			}

			long col = 0;
			// Color ByLayer
			if (EntityHeader.Color == 256)
			{
				col = -1;
			}
			else
				col = GetRGBOfCAD(EntityHeader.Color);
		
			pFtr->GetGeometry()->SetColor(col);

			pFtr->SetCode(strMapName);

			if (!m_pDS->AddObject(pFtr))
			{
				delete pFtr;
				pFtr = NULL;
				continue;
			}

			if (m_pUndo)
			{
				m_pUndo->AddNewFeature(FTR_HANDLE(pFtr));
			}

			m_arrFtrs.Add(pFtr);			

		} while (m_DxfDraw.FindEntity(FIND_NEXT,NULL) > 0);		

	}
	GProgressEnd();

	if (m_pUndo)
	{
		m_pUndo->Commit();
	}

	return TRUE;
}


template<class T>
void ReadArrayData(SAFEARRAY *pArray, T **ppts, int *npt)
{
	T *pStructs = NULL;
	SafeArrayAccessData(pArray, (void **)&pStructs);
	
	long n1 = 0, n2 = 0;
	SafeArrayGetLBound(pArray,1,&n1);
	SafeArrayGetUBound(pArray,1,&n2);
	
	int size = n2-n1+1;
	
	*ppts = new T[size];
	
	memcpy(*ppts,pStructs+n1,size*sizeof(T));
	
	*npt = size;
	
	SafeArrayUnaccessData(pArray);
}


template<class T>
SAFEARRAY *CreateArrayBaseType(int type, const T *pts, int npt)
{	
	SAFEARRAY *pArray = SafeArrayCreateVector(type, 0, npt);
	if( pArray==NULL )
		return NULL;
	
	T *pStructs = NULL;
	
	SafeArrayAccessData(pArray, (void **)&pStructs);
	
	if( pStructs )
	{
		memcpy(pStructs,pts,sizeof(T)*npt);
	}
	
	SafeArrayUnaccessData(pArray);
	
	return pArray;
}



template<class T>
class RecordInfoCache
{
public:
	RecordInfoCache()
	{
		pRecordInfo = NULL;
		
		HRESULT hr = GetRecordInfoFromGuids(
			LIBID_OdaX,
			1,
			0,
			LOCALE_USER_DEFAULT,
			__uuidof(T),
			&pRecordInfo);
	}
	~RecordInfoCache()
	{
		if( pRecordInfo )
		{
			pRecordInfo->Release();
		}
	}
	IRecordInfo *pRecordInfo;
};

template<class T>
SAFEARRAY *CreateArrayCom(const T *pts, int npt)
{
	static RecordInfoCache<T> cache;
	
	if( cache.pRecordInfo!=NULL )
	{
		SAFEARRAY *pArray = SafeArrayCreateVectorEx(VT_RECORD, 0, npt, cache.pRecordInfo);
		
		if( pArray==NULL )
		{
			return NULL;
		}
		
		T *pStructs = NULL;
		
		SafeArrayAccessData(pArray, (void **)&pStructs);
		
		if( pStructs )
		{
			memcpy(pStructs,pts,sizeof(T)*npt);
		}
		
		SafeArrayUnaccessData(pArray);
		
		return pArray;
	}
	
	return NULL;	
}




extern CString VarToString(_variant_t& var);
void ReadXData(VARIANT& v1, VARIANT& v2, CString& strcode)
{
	short *buf = NULL;
	int num = 0;

	VARIANT *buf2 = NULL;
	int num2;

	if( v1.vt==(VT_ARRAY|VT_I2) && v2.vt==(VT_ARRAY|VT_VARIANT) )
	{
		ReadArrayData(v1.parray,&buf,&num);

		ReadArrayData(v2.parray,&buf2,&num2);

		CString text1, text2;
		int i=0;
		for( i=0; i<num; i++)
		{
			_variant_t var = buf[i];
			text1 = VarToString(var);
			_variant_t var2 = buf2[i];
			text2 = VarToString(var2);

			if( text1=="1001" && text2=="SOUTH" )
			{
				i++;
				break;
			}
		}
		if(i<num)
		{
			_variant_t var = buf[i];
			text1 = VarToString(var);
			if( text1=="1000")
			{
				_variant_t var2 = buf2[i];
				strcode = VarToString(var2);
			}
		}
	}
}



static BSTR CharToWChar2(const char *str)
{
	WCHAR str3[1024]={0};
	int num = MultiByteToWideChar(CP_ACP,0,str,strlen(str)+1,str3,1024);
	
	return SysAllocString(str3);
}


void WriteXdata(IAcadEntity *pEntity, short code, LPCTSTR text)
{
	_variant_t var[2];
	var[0] = CharToWChar2("SOUTH");
	var[1] = CharToWChar2(text);

	short codes[2] = { 1001, code};

	SAFEARRAY *pArray1 = CreateArrayBaseType(VT_I2, codes, 2);
	SAFEARRAY *pArray2 = CreateArrayBaseType(VT_VARIANT, var, 2);

	var[0].Clear();
	var[1].Clear();

	VARIANT v1, v2;
	VariantInit(&v1);
	VariantInit(&v2);

	v1.vt = VT_ARRAY|VT_I2;
	v1.parray = pArray1;

	v2.vt = VT_ARRAY|VT_VARIANT;
	v2.parray = pArray2;

	HRESULT hr = pEntity->SetXData(v1,v2);

	SafeArrayDestroy(pArray1);
	SafeArrayDestroy(pArray2);
}



void WriteCassCode(IAcadEntity *pEntity, LPCTSTR code)
{	
	if( code!=NULL && strlen(code)>0 )
	{
		WriteXdata(pEntity,1000,code);
	}
}

_COM_SMARTPTR_TYPEDEF(IAcadAcCmColor, __uuidof(IAcadAcCmColor));

CLSID CLSID_AcadAcCmColor1 = {0x9c8f8bf6,0x73ed,0x4b35,{0x93,0xb1,0xaf,0x6b,0xd8,0x2b,0x69,0x5d}};

static IAcadAcCmColorPtr TrueColor(COLORREF color)
{
	IAcadAcCmColorPtr clr;
	clr.CreateInstance(CLSID_AcadAcCmColor1);
	clr->SetRGB(GetRValue(color),GetGValue(color),GetBValue(color));
	return clr;
}


BOOL CDxfRead::ReadFeatures_dwg()
{
	CScheme *pScheme = NULL;
	CString strMapName;
	long lObjSum = 0;

	//读取块
	ReadBlocks_dwg();
	
	lObjSum = 0;
	m_pMS->get_Count(&lObjSum);
	GProgressStart(lObjSum);

	//图幅名
	strMapName = m_dlgDxfImport.m_strMapName;
	pScheme = gpCfgLibMan->GetScheme(m_pDS->GetScale());
	 
	IAcadEntity *pEntity = NULL;
	HRESULT hr;
	BOOL bRelease = TRUE;
	for (int ent=0; ent<lObjSum; ent++)
	{
		GProgressStep();

		if (!bRelease && pEntity)
		{
			pEntity->Release();
			bRelease = TRUE;
		}
		
		hr = m_pMS->Item((_variant_t)(long)ent,&pEntity);
		if(FAILED(hr))continue;

		bRelease = FALSE;

		CValueTable tab;
		tab.BeginAddValueItem();
		
		//层名
		BSTR bName;
		pEntity->get_Layer(&bName);

		_bstr_t tmpbstr(bName,FALSE);

		CString strName = (char*)tmpbstr;

		CString strLayerName;
		
		//读取CASS码
		BSTR type;
		pEntity->get_ObjectName(&type);

		VARIANT varType, varValue;
		pEntity->GetXData(NULL,&varType,&varValue);

		CString strcode;//CASS代码
		ReadXData(varType,varValue, strcode);

		CFtrLayer *pFtrLay = NULL;
		if(m_dlgDxfImport.m_bReadCassCode)
		{
			if (!strcode.IsEmpty())
			{
				pFtrLay = m_pDS->GetFtrLayer(strcode);
				strLayerName = strcode;
			}
		}

		if(pFtrLay==NULL)
		{
			if (!m_dlgDxfImport.m_strLstFile.IsEmpty())
			{
				if(m_dlgDxfImport.m_bReadCassCode)
					strLayerName = m_lstFile.FindMatchItem(strcode,5,0);
				else
					strLayerName = m_lstFile.FindMatchItem(strName,1,0);
			}			
			if (strLayerName.IsEmpty())
				strLayerName = strName;
			
			CString tname = strLayerName;
			
			if (m_dlgDxfImport.m_bUseIndex)
			{
				__int64 code = _atoi64(strLayerName);
				const char *rname = NULL;
				if (pScheme->FindLayerIdx(TRUE,code,rname))
				{
					strLayerName = rname;
				}
			}
			
			if (strLayerName.IsEmpty()) continue;
			pFtrLay = m_pDS->GetFtrLayer(strLayerName);
		}

		if (pFtrLay == NULL)
		{				
			pFtrLay = m_pDS->CreateFtrLayer(strLayerName);
			if (!pFtrLay)  continue;
			IAcadLayer *pLayer = FindDwgLayer(strName);
			if (!pLayer) continue;
 
			AcColor color;
			pLayer->get_color(&color);
			pLayer->Release();

			pFtrLay->SetColor(GetRGBOfCAD(color));

			m_pDS->AddFtrLayer(pFtrLay);			
		}
		
		if (!m_pDS->SetCurFtrLayer(pFtrLay->GetID()))
			continue;

		_bstr_t tmpbstr1(type,FALSE);

		const char *pType = (const char*)tmpbstr1;

		// 获取类型和数据
		CArray<PT_3DEX,PT_3DEX> arrPts;
		
		int nType = 0;
		VARIANT var;
		VariantInit(&var);
		CArray<PT_3D,PT_3D> pts;

		if (stricmp(pType,"AcDbBlockReference") == 0)
		{			
			nType = CLS_GEOPOINT;
			
			IAcadBlockReference *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;			
			
			pobj->get_InsertionPoint(&var);		
			
			Get3DPointArray(pts,&var);
			int num = pts.GetSize();
			
			if (num <= 0)
			{
				pobj->Release();
				continue;
			}
			
			arrPts.Add(PT_3DEX(pts[0],penLine));
			
			pobj->Release();
		}
		else if (stricmp(pType,"AcDbPoint") == 0)
		{
			//如果导入了Lidar点数据，这里对点数据就不作处理了
			if (m_dlgDxfImport.m_bLidarPoints)
				continue;			

			nType = CLS_GEOPOINT;

			IAcadPoint *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;			
			
			hr = pobj->get_Coordinates(&var);			
			
			Get3DPointArray(pts,&var);
			int num = pts.GetSize();
			if (num <= 0)
			{
				pobj->Release();
				continue;
			}

			arrPts.Add(PT_3DEX(pts[0],penLine));
				
			pobj->Release();
		}
		else if (stricmp(pType,"AcDbArc") == 0)
		{
			nType = CLS_GEOCURVE;

			IAcadArc *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;

			PT_3D cenPt;
			pobj->get_Center(&var);
			Get3DPointArray(pts,&var);
			cenPt = pts[0];

			double radius, startAng, endAng;
			pobj->get_StartAngle(&ACAD_ANGLE(startAng));
			pobj->get_EndAngle(&ACAD_ANGLE(endAng));
			pobj->get_Radius(&radius);

			PT_3DEX pt;
			pt.pencode = penArc;
			pt.x = cenPt.x + radius * cos(startAng);
			pt.y = cenPt.y + radius * sin(startAng);
			pt.z = cenPt.z;
			arrPts.Add(pt);
			
			if (startAng < endAng)
			{
				pt.x = cenPt.x + radius * cos((startAng+endAng)/2);
				pt.y = cenPt.y + radius * sin((startAng+endAng)/2);
				pt.z = cenPt.z;
				arrPts.Add(pt);
			}
			else
			{
				pt.x = cenPt.x + radius * cos((startAng+endAng)/2+PI);
				pt.y = cenPt.y + radius * sin((startAng+endAng)/2+PI);
				pt.z = cenPt.z;
				arrPts.Add(pt);
				
			}
			
			pt.x = cenPt.x + radius * cos(endAng);
			pt.y = cenPt.y + radius * sin(endAng);
			pt.z = cenPt.z;
			arrPts.Add(pt);
			
			double lfthick = 0;
			pobj->get_Thickness(&lfthick);
			tab.AddValue("CAD_Thickness",&CVariantEx((_variant_t)lfthick),"CAD_thickness");
			
			pobj->Release();
		}
		else if (stricmp(pType,"AcDbCircle") == 0)
		{
			nType = CLS_GEOCURVE;
			
			IAcadCircle *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;

			PT_3D cenPt;
			pobj->get_Center(&var);
			Get3DPointArray(pts,&var);
			cenPt = pts[0];
			
			double radius;
			pobj->get_Radius(&radius);

			PT_3DEX pt;
			pt.pencode = penArc;
			pt.x = cenPt.x + radius;
			pt.y = cenPt.y;
			pt.z = cenPt.z;
			arrPts.Add(pt);
			
			pt.x = cenPt.x;
			pt.y = cenPt.y + radius;
			pt.z = cenPt.z;
			arrPts.Add(pt);
			
			pt.x = cenPt.x - radius;
			pt.y = cenPt.y;
			pt.z = cenPt.z;
			arrPts.Add(pt);
			
			pt.x = cenPt.x + radius;
			pt.y = cenPt.y;
			pt.z = cenPt.z;
			arrPts.Add(pt);
			
			double lfthick = 0;
			pobj->get_Thickness(&lfthick);
			tab.AddValue("CAD_Thickness",&CVariantEx((_variant_t)lfthick),"CAD_thickness");

			pobj->Release();
		}
		else if (stricmp(pType,"AcDbPolyline") == 0)
		{
			nType = CLS_GEOCURVE;
			
			IAcadLWPolyline *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;

			pobj->get_Coordinates(&var);

			double height;
			pobj->get_Elevation(&height);
			
			Get3DPointArray(pts,&var,FALSE);
			
			int pos = arrPts.GetSize();
			for( int i=0; i<pts.GetSize(); i++)
			{
				PT_3DEX pt;
				COPY_3DPT(pt,pts[i]);
				pt.z = height;
				pt.pencode = penLine;
				arrPts.Add(pt);
			}
			
			VARIANT_BOOL closed;
			pobj->get_Closed(&closed);
			
			if ((bool)closed)
			{
				nType = CLS_GEOSURFACE;
				arrPts.Add(arrPts.GetAt(pos));
			}
		
			double lfthick = 0;
			pobj->get_Thickness(&lfthick);
			tab.AddValue("CAD_Thickness",&CVariantEx((_variant_t)lfthick),"CAD_thickness");
			
			pobj->Release();
		}
		else if (stricmp(pType,"AcDbLine") == 0)
		{
			nType = CLS_GEOCURVE;
			
			IAcadLine *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;

			PT_3D startPt, endPt;
			VARIANT var1, var2;
			pobj->get_StartPoint(&var1);
			pobj->get_EndPoint(&var2);
			Get3DPointArray(pts,&var1);
			arrPts.Add(PT_3DEX(pts[0],penLine));
			Get3DPointArray(pts,&var2);
			arrPts.Add(PT_3DEX(pts[0],penLine));

			double lfthick = 0;
			pobj->get_Thickness(&lfthick);
			tab.AddValue("CAD_Thickness",&CVariantEx((_variant_t)lfthick),"CAD_thickness");
			
			pobj->Release();

			VariantClear(&var1);
			VariantClear(&var2);
		}
		else if (stricmp(pType,"AcDb2dPolyline") == 0)
		{
			nType = CLS_GEOCURVE;
			
			IAcadPolyline *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;
			
			pobj->get_Coordinates(&var);
			
			double height;
			pobj->get_Elevation(&height);
			
			Get3DPointArray(pts,&var);
			
			int pos = arrPts.GetSize();
			for( int i=0; i<pts.GetSize(); i++)
			{
				PT_3DEX pt;
				COPY_3DPT(pt,pts[i]);
				pt.z = height;
				pt.pencode = penLine;
				arrPts.Add(pt);
			}
			
			VARIANT_BOOL closed;
			pobj->get_Closed(&closed);
			
			if ((bool)closed)
			{
				nType = CLS_GEOSURFACE;
				arrPts.Add(arrPts.GetAt(pos));
			}
			
			double lfthick = 0;
			pobj->get_Thickness(&lfthick);
			tab.AddValue("CAD_Thickness",&CVariantEx((_variant_t)lfthick),"CAD_thickness");
			
			pobj->Release();
		}
		else if (stricmp(pType,"AcDb3dPolyline") == 0)
		{
			nType = CLS_GEOCURVE;
			
			IAcad3DPolyline *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;
			

			pobj->get_Coordinates(&var);

			Get3DPointArray(pts,&var);

			int pos = arrPts.GetSize();
			for( int i=0; i<pts.GetSize(); i++)
			{
				PT_3DEX pt;
				COPY_3DPT(pt,pts[i]);
				pt.pencode = penLine;
				arrPts.Add(pt);
			}
			
			VARIANT_BOOL closed;
			pobj->get_Closed(&closed);
			
			if ((bool)closed)
			{
				nType = CLS_GEOSURFACE;
				arrPts.Add(arrPts.GetAt(pos));
			}
			
			pobj->Release();
		}
		else if (stricmp(pType,"AcDbSpline") == 0)
		{
			nType = CLS_GEOCURVE;
			
			IAcadSpline *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;				
			
			long nCtrlPoints = 0;
			pobj->get_NumberOfControlPoints(&nCtrlPoints);
			
			for( int i=0; i<nCtrlPoints; i++)
			{
				pobj->GetControlPoint(i,&var);
				Get3DPointArray(pts,&var);
				
				arrPts.Add(PT_3DEX(pts[0],penLine));

				VariantClear(&var);
			}
			
			if( nCtrlPoints<=1 )
			{
				pobj->Release();
				continue;
			}
			
			VARIANT_BOOL closed;
			pobj->get_Closed(&closed);
			
			//闭合
			if ((bool)closed)
			{
				arrPts.Add(arrPts[0]);
			}
			
			if( nCtrlPoints>2 )
			{
				ConvertCADSpline(arrPts);
			}
			
			pobj->Release();
		}
		else if (stricmp(pType,"AcDbText") == 0)
		{
			nType = CLS_GEOTEXT;

			IAcadText *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;			
			
			pobj->get_InsertionPoint(&var);		
			
			Get3DPointArray(pts,&var);
			int num = pts.GetSize();
			if (num <= 0)
			{
				pobj->Release();
				continue;
			}
			
			arrPts.Add(PT_3DEX(pts[0],penLine));
			
			double lfthick = 0;
			pobj->get_Thickness(&lfthick);
			tab.AddValue("CAD_Thickness",&CVariantEx((_variant_t)lfthick),"CAD_thickness");
			
			pobj->Release();
		}
		else if( stricmp(pType,"AcDbHatch")==0 )
		{
			nType = CLS_GEOSURFACE;

			IAcadHatch *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;	

			long nLoops = 0;
			pobj->get_NumberOfLoops(&nLoops);
			VARIANT var2;

			HRESULT hr = 0;

			for( int j=0; j<nLoops; j++)
			{
				HRESULT hr = pobj->GetLoopAt(j,&var2);

				if( var2.vt==(VT_DISPATCH|VT_ARRAY) )
				{
					SAFEARRAY *pArray = var2.parray;
					
					IDispatch HUGEP **pData = NULL;
					SafeArrayAccessData(pArray,(void HUGEP**)&pData);

					for( int k=0; k<pArray->rgsabound[0].cElements; k++)
					{
						IDispatch *pobj2 = pData[k];
						IAcadEntity *pobj3 = NULL;
						pobj2->QueryInterface(&pobj3);

						BSTR type3;
						pobj3->get_ObjectName(&type3);
						
						_bstr_t tmpbstr3(type3,FALSE);
						
						const char *pType3 = (const char*)tmpbstr3;

						if (stricmp(pType3,"AcDbPolyline") == 0)
						{
							IAcadLWPolyline *pobj4 = NULL;
							hr = pobj3->QueryInterface(&pobj4);			
							if(FAILED(hr))continue;

							VARIANT var4;
							
							pobj4->get_Coordinates(&var4);
							
							double height;
							pobj->get_Elevation(&height);
							
							Get3DPointArray(pts,&var4,FALSE);
							
							int pos = arrPts.GetSize();
							for( int n=0; n<pts.GetSize(); n++)
							{
								PT_3DEX pt;
								COPY_3DPT(pt,pts[n]);
								pt.z = height;
								if( arrPts.GetSize()>0 && n==0 )
									pt.pencode = penMove;
								else
									pt.pencode = penLine;
								arrPts.Add(pt);
							}
							
							arrPts.Add(arrPts.GetAt(0));
							
							double lfthick = 0;
							pobj4->get_Thickness(&lfthick);
							tab.AddValue("CAD_Thickness",&CVariantEx((_variant_t)lfthick),"CAD_thickness");
							
							pobj4->Release();

							VariantClear(&var4);
						}

						if( k>0 )
							nType = CLS_GEOMULTISURFACE;
						else
							nType = CLS_GEOSURFACE;

						pobj3->Release();
						if( pobj2!=pobj3 )pobj2->Release();
					}
										
					SafeArrayUnaccessData(pArray);
				}

				VariantClear(&var2);
			}

			pobj->Release();
		}


		VariantClear(&var);
		
		//纠正可能的类型误判(因为CAD没有严格的面和线的区分)
		CSchemeLayerDefine *pItem = pScheme->GetLayerDefine(pFtrLay->GetName());
		if (pItem)
		{
			int nLayerClass = pItem->GetGeoClass();
			if( nLayerClass==CLS_GEOSURFACE && nType==CLS_GEOCURVE  )
			{
				nType = CLS_GEOSURFACE;
			}
			else if( nLayerClass==CLS_GEOCURVE && nType==CLS_GEOSURFACE  )
			{
				nType = CLS_GEOCURVE;
			}
			else if( nLayerClass==CLS_GEOPOINT && nType==CLS_GEOCURVE )
			{
				nType = CLS_GEODIRPOINT;
			}
			else if( nLayerClass==CLS_GEODIRPOINT && nType==CLS_GEOPOINT )
			{
				nType = CLS_GEODIRPOINT;
			}
		}
		
		CFeature *pFtr = pFtrLay->CreateDefaultFeature(m_pDS->GetScale(), nType);
		
		if (!pFtr || !pFtr->GetGeometry())   continue;

		BOOL bUnImport = FALSE;
		if(pItem)
		{
			int nCls = pItem->GetGeoClass();
			if(m_dlgDxfImport.m_bUnImport && nType!=nCls)// 如果地物与当前层缺省数据类型不一致就不导入
			{
				bUnImport = TRUE;
				if( nType==CLS_GEOPOINT && (nCls==CLS_GEOPOINT || nCls==CLS_GEODIRPOINT))
				{
					bUnImport = FALSE;
				}
				else if (nType==CLS_GEOCURVE && (nCls==CLS_GEOCURVE || nCls==CLS_GEODCURVE || nCls==CLS_GEOPARALLEL))
				{
					bUnImport = FALSE;
				}
			}
			if(!m_dlgDxfImport.m_bImportAnno && nType==CLS_GEOTEXT && nCls!=CLS_GEOTEXT)//过滤导出的注记导入
			{
				bUnImport = TRUE;
			}
		}
		if(bUnImport)
		{
			delete pFtr;
			pFtr = NULL;
			continue;
		}
		
		BOOL bResult = pFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		
		//创建失败
		if (pFtr->GetGeometry()->GetDataPointSum() < 1)
		{
			delete pFtr;
			pFtr = NULL;
			continue;
		}
		
		if (stricmp(pType,"AcDbBlockReference") == 0 && (nType==CLS_GEOPOINT||nType==CLS_GEODIRPOINT) )
		{			
			IAcadBlockReference *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;	

			double kx=1.0, ky=1.0, kz=1.0, rotation=0;
			pobj->get_Rotation(&rotation);
			pobj->get_XScaleFactor(&kx);
			pobj->get_YScaleFactor(&ky);
			pobj->get_ZScaleFactor(&kz);
			
			BSTR name;
			pobj->get_Name(&name);
			
			_bstr_t tmpstr(name,FALSE);

			CGeoPoint *pGeo = (CGeoPoint*)pFtr->GetGeometry();
			pGeo->m_fKx = kx;
			pGeo->m_fKy = ky;

			CString symname = (LPCTSTR)tmpstr;

			//跳过 *Model_Space 等等
			if( !symname.IsEmpty() && symname[0]=='*' )
			{
				delete pFtr;
				pFtr = NULL;
				pobj->Release();
				continue;
			}
			if(m_dlgDxfImport.m_bSaveLinetype)
			{
				tab.AddValue("CAD_SymbolName",&CVariantEx((_variant_t)(LPCTSTR)symname),"CAD_SymbolName");
				symname = "@CAD_" + symname;
				pGeo->SetSymbolName(symname);
			}
			pGeo->m_lfAngle = rotation*180/PI;

			pobj->Release();
		}
		else if (nType == CLS_GEOTEXT)
		{
			IAcadText *pobj = NULL;
			hr = pEntity->QueryInterface(&pobj);			
			if(FAILED(hr))continue;	

			BSTR text;
			pobj->get_TextString(&text);
			_bstr_t tmpstr(text,FALSE);

			double height=10, WidthFactor=1.0, rotation=0, obliAngle = 0;
			AcAlignment align = acAlignmentLeft;
			pobj->get_Height(&height);
			pobj->get_ScaleFactor(&ACAD_NOUNITS(WidthFactor));
			pobj->get_Rotation(&rotation);
			pobj->get_ObliqueAngle(&obliAngle);
			pobj->get_Alignment(&align);

			BSTR stylename;
			pobj->get_StyleName(&stylename);
			_bstr_t tmpstr2(stylename,FALSE);

			CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
			pText->SetText(LPCTSTR(tmpstr));
			TEXT_SETTINGS0 setting;
			setting.fHeight = height/cadTextScale/GetSymbolDrawScale();
			setting.fWidScale = WidthFactor;
			setting.fTextAngle = rotation;
			setting.fInclineAngle = obliAngle;
			setting.nAlignment = ConvertAlignment_DWG((int)align,FALSE);
			if( obliAngle!=0.0 )
				setting.nInclineType = SHRUGR;

			CString font = ReadTextFontName_dwg(stylename);
			if( !font.IsEmpty() )
			{
				strcpy(setting.strFontName,font);
			}
			
			pText->SetSettings(&setting);

			tab.AddValue("CAD_TextStyle",&CVariantEx((_variant_t)(LPCTSTR)tmpstr2),"CAD_TextStyle");
			tab.AddValue("CAD_FontHeight",&CVariantEx((_variant_t)height),"CAD_FontHeight");
			tab.AddValue("CAD_FontWidthFactor",&CVariantEx((_variant_t)WidthFactor),"CAD_FontWidthFactor");

			pobj->Release();
			
		}

		if( m_dlgDxfImport.m_bSaveLinetype && (nType==CLS_GEOCURVE || nType==CLS_GEOSURFACE ) )
		{			
			BSTR ltname = NULL;
			pEntity->get_Linetype(&ltname);
			
			_bstr_t tmpbstr2(ltname,FALSE);
			
			const char *pname = (const char*)tmpbstr2;

			if( stricmp(pname,"bylayer")==0 || stricmp(pname,"continuous")==0 )
			{
			}
			else  if( strlen(pname)>0 )
			{
				tab.AddValue("CAD_SymbolName",&CVariantEx((_variant_t)pname),"CAD_SymbolName");
				pFtr->GetGeometry()->SetSymbolName(CString("CAD_")+pname);
			}
		}
		
		ACAD_COLOR color;
		pEntity->get_color(&color);
		
		long col;
		if (color == acByLayer)
		{
			col = -1;
		}
		else
		{
			col = GetRGBOfCAD(color);
// 			IAcadAcCmColor *pColor = NULL;
// 			pEntity->get_TrueColor(&pColor);
// 			long r,g,b;
// 			pColor->get_Red(&r);
// 			pColor->get_Green(&g);
// 			pColor->get_Blue(&b);
// 			col = RGB(r,g,b);
// 			pColor->Release();
		}

		tab.EndAddValueItem();
		
		pFtr->GetGeometry()->SetColor(col);
		
		pFtr->SetCode(strMapName);
		
		if (!m_pDS->AddObject(pFtr, pFtrLay->GetID()))
		{
			delete pFtr;
			pFtr = NULL;
			continue;
		}

		m_pDS->GetXAttributesSource()->SetXAttributes(pFtr,tab);

		if (m_pUndo)
		{
			m_pUndo->AddNewFeature(FTR_HANDLE(pFtr));
		}
		
		m_arrFtrs.Add(pFtr);

		pEntity->Release();
		bRelease = TRUE;
	}

	if (!bRelease && pEntity)
	{
		pEntity->Release();
		bRelease = TRUE;
	}
	
	GProgressEnd();

	if (m_pUndo)
	{
		m_pUndo->Commit();
	}

	return TRUE;
}

CString CDxfRead::ReadTextFontName_dxf(OBJHANDLE hStyle)
{
	STYLE style;
	memset(&style,0,sizeof(style));
	style.Objhandle = hStyle;

	if( !m_DxfDraw.FindTableType(TAB_STYLE,FIND_BYHANDLE,&style) )
		return CString();

	TextStyle ts = GetUsedTextStyles()->GetTextStyleByName(style.Name);
	if( ts.IsValid() )
		return CString(style.Name);
	else
	{
		return m_fontFileFinder.FindFontName(style.PrimaryFontFilename);
	}
}


CString CDxfRead::ReadTextFontName_dwg(BSTR styleName)
{
	_bstr_t tmp(styleName);
	CString name = (LPCTSTR)tmp;

	TextStyle ts = GetUsedTextStyles()->GetTextStyleByName(name);
	if( ts.IsValid() )
		return name;

	//获取字体文件名
	CString fontFileName;
	IAcadTextStyles *pStyles = NULL;
	m_pDb->get_TextStyles(&pStyles);
	
	IAcadTextStyle *pStyle = NULL;
	
	long nCount = 0;
	pStyles->get_Count(&nCount);
	for( int i=0; i<nCount; i++)
	{
		pStyles->Item((_variant_t)(long)i,&pStyle);
		
		BSTR wname = NULL, wfname = NULL;
		if( pStyle && pStyle->get_Name(&wname)==0 )
		{
			pStyle->get_fontFile(&wfname);
			fontFileName = (LPCTSTR)(_bstr_t)wfname;

			pStyle->Release();
			
			if( stricmp(name,WCharToChar(wname))==0 )
			{
				break;
			}
		}
	}
	pStyles->Release();
	
	//查找对应的字体名
	if( i<nCount )
	{
		return m_fontFileFinder.FindFontName(fontFileName);
	}
	
	return CString();
}

void CDxfRead::ReadBlocks_dxf()
{
	if( !m_pDoc )
		return;

	BLOCKHEADER blkHeader;
	if( m_DxfDraw.FindBlock(FIND_FIRST,&blkHeader)>0 )
	{
		do 
		{
			//实体对象
			ENTITYHEADER	EntityHeader;
			char			EntityData[4096];
			GrBuffer buf;
			
			//读取块中实体
			if( m_DxfDraw.FindEntity(FIND_FIRST,blkHeader.Name)>0 )
			{
				do 
				{
					m_DxfDraw.GetCurEntityHeader(&EntityHeader);
					m_DxfDraw.GetCurEntityData(EntityData);
					
					double all_z = EntityHeader.Height;
					BOOL bUse_AllZ = (all_z!=0);
					
					//地物类型
					int nType;
					switch (EntityHeader.EntityType)
					{
					case ENT_LINE:
					case ENT_CIRCLE:
					case ENT_SHAPE:
					case ENT_SPLINE:
					case ENT_ARC:
						nType = CLS_GEOCURVE;
						break;
					case ENT_POLYLINE:
						{
							PENTPOLYLINE pl = (PENTPOLYLINE)EntityData;
							if( pl->Flag&1 )
							{
								nType = CLS_GEOSURFACE;
							}
							else 
							{
								nType = CLS_GEOCURVE;
							}
						}
						break;
					case ENT_POINT:
					case ENT_INSERT:
						nType = CLS_GEOPOINT;
						break;
					case ENT_TEXT:
					case ENT_MTEXT:
						nType = CLS_GEOTEXT;
						break;
					case ENT_FACE3D:
						nType = CLS_GEOSURFACE;
						break;
					default:
						nType = 0;
					}	

					//获取数据
					double fAng = 0;    // 点角度
					CArray<PT_3DEX,PT_3DEX> arrPts;
					switch( EntityHeader.EntityType )
					{
					case ENT_SHAPE:
						break;
					case ENT_ARC:
						{
							PENTARC pa = (PENTARC)EntityData;
							PT_3DEX pt;
							pt.pencode = penArc;
							pt.x = pa->Point0.x + pa->Radius * cos(pa->StartAngle*PI/180);
							pt.y = pa->Point0.y + pa->Radius * sin(pa->StartAngle*PI/180);
							pt.z = pa->Point0.z;
							arrPts.Add(pt);
							
							if (pa->StartAngle < pa->EndAngle)
							{
								pt.x = pa->Point0.x + pa->Radius * cos((pa->StartAngle+pa->EndAngle)*PI/180/2);
								pt.y = pa->Point0.y + pa->Radius * sin((pa->StartAngle+pa->EndAngle)*PI/180/2);
								pt.z = pa->Point0.z;
								arrPts.Add(pt);
							}
							else
							{
								pt.x = pa->Point0.x + pa->Radius * cos((pa->StartAngle+pa->EndAngle)*PI/180/2+PI);
								pt.y = pa->Point0.y + pa->Radius * sin((pa->StartAngle+pa->EndAngle)*PI/180/2+PI);
								pt.z = pa->Point0.z;
								arrPts.Add(pt);

							}
							
							pt.x = pa->Point0.x + pa->Radius * cos(pa->EndAngle*PI/180);
							pt.y = pa->Point0.y + pa->Radius * sin(pa->EndAngle*PI/180);
							pt.z = pa->Point0.z;
							arrPts.Add(pt);
						}
						break;
					case ENT_CIRCLE:
						{
							PENTCIRCLE pa = (PENTCIRCLE)EntityData;
							PT_3DEX pt;
							pt.pencode = penArc;
							pt.x = pa->Point0.x + pa->Radius;
							pt.y = pa->Point0.y;
							pt.z = pa->Point0.z;
							arrPts.Add(pt);
							
							pt.x = pa->Point0.x;
							pt.y = pa->Point0.y + pa->Radius;
							pt.z = pa->Point0.z;
							arrPts.Add(pt);
							
							pt.x = pa->Point0.x - pa->Radius;
							pt.y = pa->Point0.y;
							pt.z = pa->Point0.z;
							arrPts.Add(pt);
							
							pt.x = pa->Point0.x + pa->Radius;
							pt.y = pa->Point0.y;
							pt.z = pa->Point0.z;
							arrPts.Add(pt);
						}
						break;
					case ENT_LINE:
						{
							PENTLINE pl = (PENTLINE)EntityData;
							PT_3DEX pt;
							pt.pencode = penLine;
							
							COPY_3DPT(pt,pl->Point0);
							arrPts.Add(pt);
							COPY_3DPT(pt,pl->Point1);
							arrPts.Add(pt);
						}
						break;
					case ENT_POLYLINE:
						{
							PENTPOLYLINE pl = (PENTPOLYLINE)EntityData;
							//int pos = arrPts.GetSize();
							for( int i=0; i<pl->nVertex; i++)
							{
								// 过滤拟合创建的额外结点
								if (pl->pVertex[i].Flag&1 || pl->pVertex[i].Flag&8)
								{
									continue;
								}

								PT_3DEX pt;
								if( (pl->Flag&8) || (pl->Flag&16) )
								{
									COPY_3DPT(pt,pl->pVertex[i].Point);
								}
								//二维点
								else
								{
									COPY_3DPT(pt,pl->pVertex[i].Point);
									if( bUse_AllZ )pt.z = all_z;
								}
								
								if (pl->pVertex[i].Flag&16)
								{
									pt.pencode = penSpline;
								}
								else
								{
									pt.pencode = penLine;
								}
								arrPts.Add(pt);
							}
							
							//闭合
							int num = arrPts.GetSize();
							if( (pl->Flag&1) && num > 2)
							{
								arrPts.Add(arrPts[0]);
							}
						}
						break;
					case ENT_SPLINE:
						{
							PENTSPLINE pl = (PENTSPLINE)EntityData;
							
							for( int i=0; i<pl->ControlPointsNum; i++)
							{
								PT_3DEX pt;
								COPY_3DPT(pt,pl->pControlPoints[i].Point);
								
								pt.pencode = penSpline;
								arrPts.Add(pt);
							}

							ConvertCADSpline(arrPts);
							
							//闭合
							int num = arrPts.GetSize();
							if( (pl->SplineFlag&1) && nType == CLS_GEOCURVE && num > 2)
							{
								arrPts.Add(arrPts[0]);
							}
						}
						break;
					case ENT_FACE3D:
						{
							PENT3DFACE pl = (PENT3DFACE)EntityData;
							PT_3DEX pt;
							pt.pencode = penMove;
							COPY_3DPT(pt,pl->Point0);
							arrPts.Add(pt);
							pt.pencode = penLine;
							COPY_3DPT(pt,pl->Point1);
							arrPts.Add(pt);
							COPY_3DPT(pt,pl->Point2);
							arrPts.Add(pt);
							COPY_3DPT(pt,pl->Point3);
							arrPts.Add(pt);
						}
						break;
					case ENT_POINT:
						{
							PENTPOINT pp = (PENTPOINT)EntityData;
							PT_3DEX pt;
							COPY_3DPT(pt,pp->Point0);
							pt.pencode = penLine;
							arrPts.Add(pt);		
							fAng = pp->Angle;
						}
						break;
					case ENT_INSERT:
						{
							PENTINSERT pp = (PENTINSERT)EntityData;
							PT_3DEX pt;
							COPY_3DPT(pt,pp->Point0);
							pt.pencode = penLine;
							arrPts.Add(pt);	
							fAng = pp->RotationAngle;
						}
						break;
					case ENT_TEXT:
						{
							PENTTEXT pt = (PENTTEXT)EntityData;
							PT_3DEX expt;
							COPY_3DPT(expt,pt->Point0);
							expt.pencode = penLine;
							arrPts.Add(expt);			
						}
						break;
					case ENT_MTEXT:
						{
							PENTMTEXT pt = (PENTMTEXT)EntityData;
							PT_3DEX expt;
							COPY_3DPT(expt,pt->Point0);
							expt.pencode = penLine;
							arrPts.Add(expt);
						}
						break;
					}

					BOOL bDirPoint = (fabs(fAng)>1e-4);
					if (bDirPoint)
					{
						nType = CLS_GEODIRPOINT;
					}

					CFeature *pFtr = m_pDS->GetCurFtrLayer()->CreateDefaultFeature(m_pDS->GetScale(),nType);
					
					if (!pFtr || !pFtr->GetGeometry())continue;					

					BOOL bResult = pFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());

					if (bDirPoint)
					{
						((CGeoDirPoint*)pFtr->GetGeometry())->SetDirection(fAng);
					}

					//创建失败
					if (pFtr->GetGeometry()->GetDataPointSum() < 1)
					{
						delete pFtr;
						pFtr = NULL;
						continue;
					}

					if (nType == CLS_GEOTEXT)
					{
						if (EntityHeader.EntityType == ENT_TEXT)
						{
							PENTTEXT pt = (PENTTEXT)EntityData;
							CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
							pText->SetText(pt->strText);
							TEXT_SETTINGS0 setting;
							setting.fHeight = pt->TextData.Height/0.6/GetSymbolDrawScale();
							setting.fTextAngle = pt->TextData.RotationAngle;
						
							if (pt->TextData.WidthFactor > 0)
								setting.fWidScale = (double)pt->TextData.WidthFactor;
							else
								setting.fWidScale = 1;

							pText->SetSettings(&setting);
						}
						else if (EntityHeader.EntityType == ENT_MTEXT)
						{
							PENTMTEXT pt = (PENTMTEXT)EntityData;
							CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
							pText->SetText(pt->strText);
							TEXT_SETTINGS0 setting;
							setting.fHeight = pt->TextData.Height/0.6/GetSymbolDrawScale();	
							setting.fTextAngle = pt->TextData.RotationAngle;
							
							pText->SetSettings(&setting);
						}
					}
				
					if ( EntityHeader.Color<=0 || (EntityHeader.Color>255 && EntityHeader.Color!=256) )
					{
						EntityHeader.Color = 255;
					}

					long col = 0;
					// Color ByLayer
					if (EntityHeader.Color == 256)
					{
						col = -1;
					}
					else
						col = GetRGBOfCAD(EntityHeader.Color);
				
					pFtr->GetGeometry()->SetColor(col);

					pFtr->Draw(&buf);

					delete pFtr;
					pFtr = NULL;
			

				} while (m_DxfDraw.FindEntity(FIND_NEXT,blkHeader.Name) > 0);		

			}

			CString str2 = blkHeader.Name;
			//跳过 *Model_Space 等等
			if( !str2.IsEmpty() && str2[0]!='*' && buf.GetVertexPts(NULL)>0 )
			{
				GrBuffer2d buf2d;
				buf2d.AddBuffer(&buf);

				if( m_pDoc )
				{
					m_pDoc->SaveCellDefToFile("CAD_"+str2,&buf2d);
				}
			}

		} while( m_DxfDraw.FindBlock(FIND_NEXT,&blkHeader)>0 );
	}

	return;
}


void CDxfRead::ConvertCADSpline(CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	int npt = GraphAPI::GKickoffSame2DPoints(arrPts.GetData(),arrPts.GetSize());
	if( npt<=2 )
		return;

	int npt2 = npt*10;
	PT_3D *pts_out = new PT_3D[npt2];
	rbspline(arrPts.GetData(),npt,3,NULL,npt2,pts_out);					
	
	arrPts.RemoveAll();
	for( int i=0; i<npt2; i++)
	{
		arrPts.Add(PT_3DEX(pts_out[i],penLine));
	}
	
	delete[] pts_out;

	double toler = 0;
	if( m_pDS )
		toler = m_pDS->GetScale()*0.00001;
	if( toler<GraphAPI::g_lfDisTolerance )
		toler = GraphAPI::g_lfDisTolerance;
	
	SimpleCompressPoints(arrPts,toler);
}

void CDxfRead::ReadBlocks_dwg()
{
	if( !m_pDoc )
		return;

	if( !m_pDb )
		return;

	IAcadBlocks *pBlocks = NULL;
	m_pDb->get_Blocks(&pBlocks);

	if( !pBlocks )
		return;

	long nBlocks = 0;
	pBlocks->get_Count(&nBlocks);
	
	IAcadBlock *pBlock = NULL;
	IAcadEntity *pEntity = NULL;
	HRESULT hr;
	BOOL bRelease = TRUE;

	for( int j=0; j<nBlocks; j++)
	{
		pBlocks->Item((_variant_t)(long)j,&pBlock);
		if( !pBlock )
			continue;

		BSTR name;
		pBlock->get_Name(&name);
		
		bstr_t tmpbstr1(name,FALSE);
		CString str2 = (LPCTSTR)tmpbstr1;

		//跳过 *Model_Space 等等
		if( !str2.IsEmpty() && str2[0]=='*' )
		{
			pBlock->Release();
			continue;
		}

		GrBuffer buf;

		long lObjSum = 0;
		pBlock->get_Count(&lObjSum);

		for (int ent=0; ent<lObjSum; ent++)
		{
			if (!bRelease && pEntity)
			{
				pEntity->Release();
				bRelease = TRUE;
			}
			
			hr = pBlock->Item((_variant_t)(long)ent,&pEntity);
			if(FAILED(hr))continue;

			bRelease = FALSE;		
			
			BSTR type;
			pEntity->get_ObjectName(&type);

			_bstr_t tmpbstr1(type,FALSE);

			const char *pType = (const char*)tmpbstr1;

			// 获取类型和数据
			CArray<PT_3DEX,PT_3DEX> arrPts;
			
			int nType = 0;
			VARIANT var;
			VariantInit(&var);
			CArray<PT_3D,PT_3D> pts;

			if (stricmp(pType,"AcDbBlockReference") == 0)
			{
				continue;
			}
			else if (stricmp(pType,"AcDbPoint") == 0)
			{
				//如果导入了Lidar点数据，这里对点数据就不作处理了
				if (m_dlgDxfImport.m_bLidarPoints)
					continue;			

				nType = CLS_GEOPOINT;

				IAcadPoint *pobj = NULL;
				hr = pEntity->QueryInterface(&pobj);			
				if(FAILED(hr))continue;			
				
				hr = pobj->get_Coordinates(&var);			
				
				Get3DPointArray(pts,&var);
				int num = pts.GetSize();
				if (num <= 0)
				{
					pobj->Release();
					continue;
				}

				arrPts.Add(PT_3DEX(pts[0],penLine));
					
				pobj->Release();
			}
			else if (stricmp(pType,"AcDbArc") == 0)
			{
				nType = CLS_GEOCURVE;

				IAcadArc *pobj = NULL;
				hr = pEntity->QueryInterface(&pobj);			
				if(FAILED(hr))continue;

				PT_3D cenPt;
				pobj->get_Center(&var);
				Get3DPointArray(pts,&var);
				cenPt = pts[0];

				double radius, startAng, endAng;
				pobj->get_StartAngle(&ACAD_ANGLE(startAng));
				pobj->get_EndAngle(&ACAD_ANGLE(endAng));
				pobj->get_Radius(&radius);

				PT_3DEX pt;
				pt.pencode = penArc;
				pt.x = cenPt.x + radius * cos(startAng);
				pt.y = cenPt.y + radius * sin(startAng);
				pt.z = cenPt.z;
				arrPts.Add(pt);
				
				if (startAng < endAng)
				{
					pt.x = cenPt.x + radius * cos((startAng+endAng)/2);
					pt.y = cenPt.y + radius * sin((startAng+endAng)/2);
					pt.z = cenPt.z;
					arrPts.Add(pt);
				}
				else
				{
					pt.x = cenPt.x + radius * cos((startAng+endAng)/2+PI);
					pt.y = cenPt.y + radius * sin((startAng+endAng)/2+PI);
					pt.z = cenPt.z;
					arrPts.Add(pt);
					
				}
				
				pt.x = cenPt.x + radius * cos(endAng);
				pt.y = cenPt.y + radius * sin(endAng);
				pt.z = cenPt.z;
				arrPts.Add(pt);
				
				pobj->Release();
			}
			else if (stricmp(pType,"AcDbCircle") == 0)
			{
				nType = CLS_GEOCURVE;
				
				IAcadCircle *pobj = NULL;
				hr = pEntity->QueryInterface(&pobj);			
				if(FAILED(hr))continue;

				PT_3D cenPt;
				pobj->get_Center(&var);
				Get3DPointArray(pts,&var);
				cenPt = pts[0];
				
				double radius;
				pobj->get_Radius(&radius);

				PT_3DEX pt;
				pt.pencode = penArc;
				pt.x = cenPt.x + radius;
				pt.y = cenPt.y;
				pt.z = cenPt.z;
				arrPts.Add(pt);
				
				pt.x = cenPt.x;
				pt.y = cenPt.y + radius;
				pt.z = cenPt.z;
				arrPts.Add(pt);
				
				pt.x = cenPt.x - radius;
				pt.y = cenPt.y;
				pt.z = cenPt.z;
				arrPts.Add(pt);
				
				pt.x = cenPt.x + radius;
				pt.y = cenPt.y;
				pt.z = cenPt.z;
				arrPts.Add(pt);

				pobj->Release();
			}
			else if (stricmp(pType,"AcDbPolyline") == 0)
			{
				nType = CLS_GEOCURVE;
				
				IAcadLWPolyline *pobj = NULL;
				hr = pEntity->QueryInterface(&pobj);			
				if(FAILED(hr))continue;

				pobj->get_Coordinates(&var);

				double height;
				pobj->get_Elevation(&height);
				
				Get3DPointArray(pts,&var,FALSE);
				
				int pos = arrPts.GetSize();
				for( int i=0; i<pts.GetSize(); i++)
				{
					PT_3DEX pt;
					COPY_3DPT(pt,pts[i]);
					pt.z = height;
					pt.pencode = penLine;
					arrPts.Add(pt);
				}
				
				VARIANT_BOOL closed;
				pobj->get_Closed(&closed);
				
				if ((bool)closed)
				{
					nType = CLS_GEOSURFACE;
					arrPts.Add(arrPts.GetAt(pos));
				}
								
				pobj->Release();
			}
			else if (stricmp(pType,"AcDbLine") == 0)
			{
				nType = CLS_GEOCURVE;
				
				IAcadLine *pobj = NULL;
				hr = pEntity->QueryInterface(&pobj);			
				if(FAILED(hr))continue;

				PT_3D startPt, endPt;
				VARIANT var1, var2;
				pobj->get_StartPoint(&var1);
				pobj->get_EndPoint(&var2);
				Get3DPointArray(pts,&var1);
				arrPts.Add(PT_3DEX(pts[0],penLine));
				Get3DPointArray(pts,&var2);
				arrPts.Add(PT_3DEX(pts[0],penLine));
				
				pobj->Release();
			}
			else if (stricmp(pType,"AcDb2dPolyline") == 0)
			{
				nType = CLS_GEOCURVE;
				
				IAcadPolyline *pobj = NULL;
				hr = pEntity->QueryInterface(&pobj);			
				if(FAILED(hr))continue;				
				
				pobj->get_Coordinates(&var);
				
				Get3DPointArray(pts,&var);
				
				int pos = arrPts.GetSize();
				for( int i=0; i<pts.GetSize(); i++)
				{
					PT_3DEX pt;
					COPY_3DPT(pt,pts[i]);
					pt.pencode = penLine;
					arrPts.Add(pt);
				}
				
				VARIANT_BOOL closed;
				pobj->get_Closed(&closed);
				
				if ((bool)closed)
				{
					nType = CLS_GEOSURFACE;
					arrPts.Add(arrPts.GetAt(pos));
				}				
				
				pobj->Release();
			}
			else if (stricmp(pType,"AcDb3dPolyline") == 0)
			{
				nType = CLS_GEOCURVE;
				
				IAcad3DPolyline *pobj = NULL;
				hr = pEntity->QueryInterface(&pobj);			
				if(FAILED(hr))continue;
				

				pobj->get_Coordinates(&var);

				Get3DPointArray(pts,&var);

				int pos = arrPts.GetSize();
				for( int i=0; i<pts.GetSize(); i++)
				{
					PT_3DEX pt;
					COPY_3DPT(pt,pts[i]);
					pt.pencode = penLine;
					arrPts.Add(pt);
				}
				
				VARIANT_BOOL closed;
				pobj->get_Closed(&closed);
				
				if ((bool)closed)
				{
					nType = CLS_GEOSURFACE;
					arrPts.Add(arrPts.GetAt(pos));
				}
				
				pobj->Release();
			}
			else if (stricmp(pType,"AcDbSpline") == 0)
			{
				nType = CLS_GEOCURVE;
				
				IAcadSpline *pobj = NULL;
				hr = pEntity->QueryInterface(&pobj);			
				if(FAILED(hr))continue;				
				
				long nCtrlPoints = 0;
				pobj->get_NumberOfControlPoints(&nCtrlPoints);

				for( int i=0; i<nCtrlPoints; i++)
				{
					pobj->GetControlPoint(i,&var);
					Get3DPointArray(pts,&var);

					arrPts.Add(PT_3DEX(pts[0],penLine));
				}

				if( nCtrlPoints<=1 )
				{
					pobj->Release();
					continue;
				}

				VARIANT_BOOL closed;
				pobj->get_Closed(&closed);
				
				//闭合
				if ((bool)closed)
				{
					arrPts.Add(arrPts[0]);
				}

				if( nCtrlPoints>2 )
				{
					ConvertCADSpline(arrPts);
				}
				
				pobj->Release();
			}
			else if (stricmp(pType,"AcDbText") == 0)
			{
				nType = CLS_GEOTEXT;

				IAcadText *pobj = NULL;
				hr = pEntity->QueryInterface(&pobj);			
				if(FAILED(hr))continue;			
				
				pobj->get_InsertionPoint(&var);		
				
				Get3DPointArray(pts,&var);
				int num = pts.GetSize();
				if (num <= 0)
				{
					pobj->Release();
					continue;
				}
				
				arrPts.Add(PT_3DEX(pts[0],penLine));
				
				pobj->Release();
			}

			VariantClear(&var);		
			
			CFeature *pFtr = m_pDS->GetCurFtrLayer()->CreateDefaultFeature(m_pDS->GetScale(), nType);
			
			if (!pFtr || !pFtr->GetGeometry())   continue;
			
			BOOL bResult = pFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
			
			//创建失败
			if (pFtr->GetGeometry()->GetDataPointSum()<1)
			{
				delete pFtr;
				pFtr = NULL;
				continue;
			}
			
			if (nType == CLS_GEOTEXT)
			{
				IAcadText *pobj = NULL;
				hr = pEntity->QueryInterface(&pobj);			
				if(FAILED(hr))continue;	

				BSTR text;
				pobj->get_TextString(&text);
				_bstr_t tmpstr(text,FALSE);

				double height=10, WidthFactor=1.0, rotation=0;
				pobj->get_Height(&height);
				pobj->get_ScaleFactor(&ACAD_NOUNITS(WidthFactor));
				pobj->get_Rotation(&rotation);

				CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
				pText->SetText(LPCTSTR(tmpstr));
				TEXT_SETTINGS0 setting;
				setting.fHeight = height/0.6/GetSymbolDrawScale();
				setting.fWidScale = WidthFactor;
				setting.fTextAngle = rotation;
				
				pText->SetSettings(&setting);

				pobj->Release();
				
			}
			
			ACAD_COLOR color;
			pEntity->get_color(&color);
			
			long col;
			if (color == acByLayer)
			{
				col = -1;
			}
			else
			{
				col = GetRGBOfCAD(color);
			}
			
			pFtr->GetGeometry()->SetColor(col);

			pFtr->Draw(&buf);

			delete pFtr;
			pFtr = NULL;
			
			pEntity->Release();
			bRelease = TRUE;
		}

		if (!bRelease && pEntity)
		{
			pEntity->Release();
			bRelease = TRUE;
		}

		if( m_pDoc!=NULL && buf.GetVertexPts(NULL)>0 )
		{
			str2 = "CAD_" + str2;			
			
			GrBuffer2d buf2d;
			buf2d.AddBuffer(&buf);
			m_pDoc->SaveCellDefToFile((LPCTSTR)str2,&buf2d);
		}

		pBlock->Release();
	}

	pBlocks->Release();
}



BOOL CDxfRead::OnImportDxf(CDlgDataSource *pDS, BOOL bSilence, CString path)
{
	if ( !pDS )
	{
		CString strText;
		strText.LoadString(IDS_DATASOURCE_EMPTY);
		AfxMessageBox(strText);
		return FALSE;
	}
	//初始化成员变量
	m_pDS = pDS;

	CFtrLayer *pCurFtrLayer = m_pDS->GetCurFtrLayer();

//	m_dlgDxfImport.m_bNewLayer = TRUE;

	if (!bSilence)
	{
		if (m_dlgDxfImport.DoModal() != IDOK)
			return FALSE;

		// 判断导出dxf还是dwg
		if (m_dlgDxfImport.m_strDxfFile.Right(4).CompareNoCase(".dwg") == 0)
		{
			m_bImportDWG = TRUE;
		}
	}
	else
	{
		m_dlgDxfImport.m_strDxfFile = path;
	}	

	if (!m_bImportDWG)
	{
		GOutPut(StrFromResID(IDS_DOC_READDXF));
	}
	else
	{
		GOutPut(StrFromResID(IDS_DOC_READDWG));		
	}
	
	GOutPut(StrFromResID(IDS_DOC_LOADING));
	
	if ( !OpenRead(m_dlgDxfImport.m_strDxfFile) )
	{
		return FALSE;
	}
	
	//首先导入Lidar点数据
	if( m_dlgDxfImport.m_bLidarPoints )
	{
		ImportLidarDxf();
	}
	
	ReadFeatures();
	
	CloseRead();

	if (pCurFtrLayer)
	{
		m_pDS->SetCurFtrLayer(pCurFtrLayer->GetID());
	}
	
	GOutPut(StrFromResID(IDS_DOC_IMPORT_END));

	return TRUE;

}


BOOL CDxfRead::OnImportMultiDxf(CDlgDataSource *pDS)
{
	//初始化成员变量
	m_pDS = pDS;
	
	CFtrLayer *pCurFtrLayer = m_pDS->GetCurFtrLayer();
	
	//	m_dlgDxfImport.m_bNewLayer = TRUE;
	
	if (1)
	{
		if (m_dlgDxfImport.DoModal() != IDOK)
			return FALSE;

		if( m_dlgDxfImport.m_arrFileNames.GetSize()<=0 )
			return FALSE;	

	}	
	
	if (!m_bImportDWG)
	{
		GOutPut(StrFromResID(IDS_DOC_READDXF));
	}
	else
	{
		GOutPut(StrFromResID(IDS_DOC_READDWG));		
	}

	CString save_mapname = m_dlgDxfImport.m_strMapName;
	

	GOutPut(StrFromResID(IDS_DOC_LOADING));

	for( int i=0; i<m_dlgDxfImport.m_arrFileNames.GetSize(); i++)
	{
		CString fileName = m_dlgDxfImport.m_arrFileNames[i];
		CString strMsg;
		strMsg.Format("%2d : %s",i+1,(LPCTSTR)fileName);
		GOutPut(strMsg);

		if( save_mapname.CompareNoCase("*")==0 )
		{
			CString fileTitle = fileName;
			int pos = fileName.ReverseFind('\\');
			if( pos>=0 )fileTitle = fileName.Mid(pos+1);

			m_dlgDxfImport.m_strMapName = fileTitle;
		}

		// 判断dxf还是dwg
		if (fileName.Right(4).CompareNoCase(".dwg") == 0)
		{
			m_bImportDWG = TRUE;
		}

		if ( !OpenRead(fileName) )
		{
			return FALSE;
		}

		//首先导入Lidar点数据
		if( m_dlgDxfImport.m_bLidarPoints )
		{
			ImportLidarDxf();
		}
		
		ReadFeatures();
		
		CloseRead();		

	}

	m_dlgDxfImport.m_strMapName = save_mapname;

	if (pCurFtrLayer)
	{
		m_pDS->SetCurFtrLayer(pCurFtrLayer->GetID());
	}	

	GOutPut(StrFromResID(IDS_DOC_IMPORT_END));
	
	return TRUE;
}


BOOL CDxfRead::ConvertDxf2FDB(CScheme* pScheme, CStringArray& org_path, CString& des_folder)
{
    for(int i=0; i<org_path.GetSize(); ++i)
	{
		DWORD scale = gpCfgLibMan->GetScaleByScheme(pScheme);
		if(scale == 0)
			return FALSE;
		//
		TCHAR driver[_MAX_DRIVE], path[_MAX_PATH], fname[_MAX_FNAME], ext[_MAX_EXT];
		_tsplitpath(org_path[i], driver, path, fname, ext);
		//
		CString temp_path = des_folder+fname+".fdb";
		//
        CSQLiteAccess *pAccess = new CSQLiteAccess();
		if(!pAccess)
		  continue;
		
		pAccess->Attach(temp_path);
		
		CDlgDataSource *pDataSource = new CDlgDataSource(NULL);	
		if( !pDataSource )
		{
			delete pAccess;
			continue;
		}
		pDataSource->SetAccessObject(pAccess);

		
		pAccess->BatchUpdateBegin();
		pAccess->CreateFileSys(scale, pScheme);
		pAccess->BatchUpdateEnd();
		
		pDataSource->LoadAll(NULL);	
		pDataSource->ResetDisplayOrder();
		pAccess->BatchUpdateBegin();
		pDataSource->SaveAllLayers();
	    pAccess->BatchUpdateEnd();

		pDataSource->SetScale(scale);
		//
		m_pDS = pDataSource;
        m_dlgDxfImport.Create(CDlgImportDxf::IDD,NULL);
		m_dlgDxfImport.m_arrFileNames.Add(org_path[i]);

		CString fileTitle = org_path[i];
		int pos = org_path[i].ReverseFind('\\');
		if( pos>=0 )fileTitle = org_path[i].Mid(pos+1);
		m_dlgDxfImport.m_strMapName = fileTitle;
        // 判断dxf还是dwg
		if (org_path[i].Right(4).CompareNoCase(".dwg") == 0)
		{
			m_bImportDWG = TRUE;
		}
		
		pAccess->BatchUpdateBegin();

		if ( !OpenRead(org_path[i]) )
		{
			delete pDataSource;
			pDataSource = NULL;
			pAccess->BatchUpdateEnd();
			continue;
		}

		
		ReadFeatures();
		
		CloseRead();
		
		pAccess->BatchUpdateEnd();

        pAccess->Close();
		delete pDataSource;
		pDataSource = NULL;
	}
	//
	return TRUE;
}

CFontFileFind::CFontFileFind()
{
	m_bLoad = FALSE;
}


CFontFileFind::~CFontFileFind()
{

}


CString CFontFileFind::FindFontFile(LPCTSTR font)
{
	if( !m_bLoad )
		Load();

	for( int i=0; i<m_fontList.GetSize(); i++)
	{
		if( m_fontList[i].CompareNoCase(font)==0 )
			return m_fileList[i];
	}

	return CString();
}

CString CFontFileFind::FindFontName(LPCTSTR file)
{
	if( !m_bLoad )
		Load();
	
	for( int i=0; i<m_fileList.GetSize(); i++)
	{
		if( m_fileList[i].CompareNoCase(file)==0 )
			return m_fontList[i];
	}
	
	return CString();
}


//字体名称中可能有尾部说明字符，这里需要把这些说明字符删除；
//例如，宋体-方正超大字符集 (TrueType)，需要去掉“ (TrueType)”；
void CFontFileFind::CutTailChars(char *buf)
{
	int len = strlen(buf);
	char *p = buf+len-1;

	//去除尾部空格
	while( p>buf && p[0]==' ' )
	{
		p--;
	}

	//如果不是')'，则没有尾部说明字符，直接返回
	if( p[0]!=')' )
		return;

	//找对应的'('
	while( p>buf && p[0]!='(' )
	{
		p--;
	}

	//'('前面有一个空格
	p--;

	if( p>buf && p[0]==' ' )
		p[0] = '\0';
}

void CFontFileFind::Load()
{
	m_fontList.RemoveAll();
	m_fileList.RemoveAll();
	
	//以下几个不能正确比较，就单独单独放入
	m_fontList.Add(CString("宋体"));
	m_fileList.Add(CString("SURSONG.TTF"));

	m_fontList.Add(CString("仿宋_GB2312"));
	m_fileList.Add(CString("SIMFANG.TTC"));

	m_fontList.Add(CString("楷体_GB2312"));
	m_fileList.Add(CString("SIMKAI.TTC"));

	CRegKey reg;

	if( reg.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",KEY_QUERY_VALUE)==ERROR_SUCCESS )
	{
		int       nIndexValue=0; 
		HKEY	hkOpen = reg;

		char       ValueName[256];          
		BYTE       DataValue[256];          

		while(1)
		{
			DWORD       cbValueName=256;          
			DWORD       cbDataValue=256;          
			DWORD       dwType = 0;   

			memset(ValueName,0,sizeof(ValueName));
			memset(DataValue,0,sizeof(DataValue));

			if( ERROR_NO_MORE_ITEMS==      
				RegEnumValue(hkOpen,nIndexValue,      
					ValueName,&cbValueName,NULL,          
					&dwType,DataValue,&cbDataValue)      
				)      
			{ 
				break;      
			}
			
			if( dwType==REG_SZ )
			{
				CutTailChars(ValueName);
				m_fontList.Add(ValueName);
				m_fileList.Add((char*)DataValue);
			}    
			nIndexValue++;  
		}

		reg.Close();
	}

	m_bLoad = TRUE;
}

static void KillInvalidChar(char* str)
{
	if (strlen(str) <= 0) return;
	
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	
	wstring wstr0;
	ConvertCharToWstring(str,wstr0,codepage);
	
	wchar_t *wstr = (wchar_t*)wstr0.c_str();
	wchar_t wc;
	
	wchar_t *pos = wstr, *wsrc = wstr;
	while(*pos!=0)
	{
		wc = *pos;
		if( (wc>=32 && wc<=127) || wc>=256 )
		{
			if( wstr!=pos )
				*wstr= *pos;
			wstr++;
		}
		pos++;
	}
	*wstr = 0;
	
	string ret0;
	ConvertWcharToString(wsrc,ret0,codepage,NULL,0);
	const char *ret = ret0.c_str();
	strcpy(str,ret);
}

static void NormalizeName(CString& name)
{
	RemoveChar(name," \\()*\"、（）；'，,?;:/<>=");
	ReplaceChar(name, "＿", "_");
	ReplaceChar(name, "—", "_");
	ReplaceChar(name, "-", "_");
	ReplaceChar(name, ".", "_");
	ReplaceChar(name, "：", "_");

	char *buf = name.GetBuffer(1);
	KillInvalidChar(buf);
	name.ReleaseBuffer();
}

static void NormalizeName(char* name)
{
	RemoveChar(name," \\()*\"、（）；'，,?;:/<>=");
	ReplaceChar(name, "＿", "_");
	ReplaceChar(name, "—", "_");
	ReplaceChar(name, "-", "_");
	ReplaceChar(name, ".", "_");
	ReplaceChar(name, "：", "_");
	KillInvalidChar(name);
}

void RemoveChar(char *str, char c)
{
	if (strlen(str) <= 0) return;
	
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	
	wstring wstr0;
	ConvertCharToWstring(str,wstr0,codepage);
	
	wchar_t *wstr = (wchar_t*)wstr0.c_str();
	wchar_t wc = c;
	
	wchar_t *pos = wstr, *wsrc = wstr;
	while(*pos!=0)
	{
		if( *pos!=wc )
		{
			if( wstr!=pos )
				*wstr= *pos;
			wstr++;
		}
		pos++;
	}
	*wstr = 0;
	
	string ret0;
	ConvertWcharToString(wsrc,ret0,codepage,NULL,0);
	const char *ret = ret0.c_str();
	strcpy(str,ret);
}

CDxfWrite::SubSymbolItem::SubSymbolItem()
{
	memset(this,0,sizeof(*this));
}

CDxfWrite::CDxfWrite()
{	
	m_pDS = NULL;
	m_nBlockNo = 0;
	m_bExportDWG = FALSE;
	m_iHost = NULL;
	m_iApplication = NULL;
    m_pCurDocument = NULL;
	m_pDb = NULL;
	m_pMS = NULL;
	m_pCurFdbLayer = NULL;

	m_hDefLinetype = NULL;
	m_hDefLayer = NULL;
}

CDxfWrite::~CDxfWrite()
{
	CloseWrite();	
}

BOOL CDxfWrite::OpenWrite(LPCTSTR fileName)
{
	m_strFileName = fileName;

	m_lstFile.SetReadIgnoredChar(TRUE,';');
	
	m_lstFile.Open(m_dlgDxfExport.m_strLstFile);
	m_lstFileEx.Open(m_dlgDxfExport.m_strColourFile);

	m_arrNewBlockNames.RemoveAll();
	m_arrDWGBlockNames.RemoveAll();

	if (!m_bExportDWG)
	{
		if( m_DxfDraw.isOpen() )
		{
			m_DxfDraw.Destroy();
		}
		
		m_DxfDraw.Create();
	}
	else
	{
		BeginCheck41License

		// 先删除存在的dwg文件
		if (_access(m_strFileName,0) == 0 && !DeleteFile(m_strFileName))
		{
			AfxMessageBox(IDS_TIP_CLOSE_DWGFILE);
			return FALSE;
		}

		HRESULT        hr;
		IClassFactory  *pCF;
		CoGetClassObject(__uuidof(OdaHostApp), CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void **)&pCF);
		if (!pCF)
		{
			AfxMessageBox("can't find dwg module,please register the module!");
			return FALSE;
		}

		hr = pCF->CreateInstance(NULL, IID_IOdaHostApp, (void **)&m_iHost);
		if (CLASS_E_NOTLICENSED == hr)
		{
			IClassFactory2  *pFactory;
			
			hr = pCF->QueryInterface(IID_IClassFactory2, (void **)&pFactory);
			if (S_OK == hr)
			{
				hr = pFactory->CreateInstanceLic(NULL, NULL, IID_IOdaHostApp, (BSTR)_bstr_t("some key"), (void **)&m_iHost);
				pFactory->Release();
			}
		}
		if (!m_iHost)
		{
			AfxMessageBox("Failed to Create HostApp!");
			return FALSE;
		}
		pCF->Release();
		
		if (S_OK != hr)
		{
			AfxMessageBox("Failed to Create InstanceLic!");
			return FALSE;
		}
		
		hr = m_iHost->get_Application(&m_iApplication);
		if (S_OK != hr)
		{
			m_iHost->Release();
			m_iHost = NULL;
			AfxMessageBox("Failed to Create Application!");
			return FALSE;
		}
				
		IAcadDocuments *pDocuments;
		// read this file 
		hr = m_iApplication->get_Documents(&pDocuments);
		if( hr!=S_OK )
		{
			AfxMessageBox("Failed to Create Documents!");
			return FALSE;
		}

		// 获得dwg模板文件
		char path[_MAX_PATH];
		if( GetModuleFileName(NULL,path,_MAX_FNAME)==0 )
		{
			AfxMessageBox("Failed to GetModuleFileName!");
			return FALSE;
		}
		
		char *pos;
		// 生成bin路径
		if( (pos=strrchr(path,'\\')) )*pos = '\0';
		if( (pos=strrchr(path,'\\')) )*pos = '\0';
		strcat(path,"\\bin\\acadiso.dwt");
		
		//IAcadDocument *pNewDoc;
		_bstr_t tmpstr = _bstr_t(path);
		hr = pDocuments->Add((_variant_t)tmpstr,&m_pCurDocument);

		if( !SUCCEEDED(hr) )
		{
			pDocuments->Release();

			CString strMsg;
			strMsg.Format("Cannot find the template file:\n%s",path);
			AfxMessageBox(strMsg);			
			return FALSE;
		}
		
		_variant_t  vParam;	
		vParam.vt = VT_ERROR; // optional parameter
		
		_bstr_t tmpstr1 = _bstr_t((const char*)fileName);
		m_pCurDocument->SaveAs(tmpstr1,(_variant_t)(long)odR18_dwg,vParam);
		
		hr = m_pCurDocument->get_Database(&m_pDb);
		
		pDocuments->Release();

		if (!SUCCEEDED(hr))
		{
			AfxMessageBox("Failed to get_Database!");
			return FALSE;
		}

		hr = m_pDb->get_ModelSpace(&m_pMS);

		if (!SUCCEEDED(hr))
		{
			AfxMessageBox("Failed to get_ModelSpace!");
			return FALSE;
		}

		//注册应用程序，后面写XData数据需要这一点
		IAcadRegisteredApplications *pRegApps = NULL;
		m_pDb->get_RegisteredApplications(&pRegApps);
		
		IAcadRegisteredApplication *pRegApp = NULL;
		if( pRegApps )
		{
			pRegApps->Add(L"SOUTH",&pRegApp);			
			if( pRegApp )pRegApp->Release();
		
			pRegApps->Release();
		}

		EndCheck41License
	}
	
	return TRUE;

}

BOOL CDxfWrite::CloseWrite()
{
	if (!m_bExportDWG)
	{
		if( m_DxfDraw.isOpen() )
		{
			//m_DxfDraw.m_nDigitNumber = 4;
			m_DxfDraw.SaveDXFFile(m_strFileName);
			m_DxfDraw.Destroy();
		}

	}
	else
	{
		if (m_pCurDocument)
		{
			m_pCurDocument->Save();
		}		

		if (m_pMS)
		{
			m_pMS->Release();
			m_pMS = NULL;
		}

		if (m_pDb)
		{
			m_pDb->Release();
			m_pDb = NULL;
		}

		if (m_pCurDocument)
		{
			m_pCurDocument->Release();
			m_pCurDocument = NULL;
		}
		
		if (m_iApplication)
		{
			m_iApplication->Quit();
			m_iApplication->Release();
			m_iApplication = NULL;
		}

		if (m_iHost)
		{
			m_iHost->Release();
			m_iHost = NULL;
		}
	}
	
	m_lstFile.Clear();
	m_lstFileEx.Close();
	return TRUE;
}

void TruncateString(char* p, int len)
{
	char *str = p;
	if ( strlen(str)<len )return;
	
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	
	wstring wstr0;
	ConvertCharToWstring(str,wstr0,codepage);
		
	wchar_t *pos = (wchar_t*)wstr0.c_str();
	int nChar = 0;
	while(*pos!=0)
	{
		if( *pos>=256 )
		{
			nChar+=2;
		}
		else
		{
			nChar+=1;
		}
		if( nChar>(len-2) )
			break;
		pos++;
	}

	*pos = '\0';
	
	string ret0;
	ConvertWcharToString(wstr0.c_str(),ret0,codepage,NULL,0);
	const char *ret = ret0.c_str();
	strcpy(str,ret);
}


void TruncateStringHead(char* p, int len)
{

	char *str = p;
	if ( strlen(str)<len )return;
	
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	
	wstring wstr0;
	ConvertCharToWstring(str,wstr0,codepage);
		
	wchar_t *pos = (wchar_t*)wstr0.c_str() + wstr0.size()-1;
	wchar_t *pos0 = (wchar_t*)wstr0.c_str();
	int nChar = 0;
	while(pos>pos0)
	{
		if( *pos>=256 )
		{
			nChar+=2;
		}
		else
		{
			nChar+=1;
		}
		if( nChar>(len-2) )
			break;
		pos--;
	}
	
	string ret0;
	ConvertWcharToString(pos,ret0,codepage,NULL,0);
	const char *ret = ret0.c_str();
	strcpy(str,ret);

}


CString CDxfWrite::GetExportedLayerName(LPCTSTR name, int nSymIndex)
{
	CString strLayerName = name;
	if( strLayerName.IsEmpty() )
	{
		if( m_pCurFdbLayer!=NULL )
			strLayerName = m_pCurFdbLayer->GetName();
	}

	if(strLayerName.IsEmpty())
	{
		strLayerName = "0";
	}
	
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	CConfigLibManager *pCfgLibManager = pApp->GetConfigLibManager();
	CScheme *pScheme = pCfgLibManager->GetScheme(m_pDS->GetScale());
	
	char layName[MAX_STRLEN] = {0};

	strncpy(layName,strLayerName,MAX_STRLEN-1);
	
	if (m_dlgDxfExport.m_bUseLayerGroup && m_pCurFdbLayer )
	{
		strncpy(layName,m_pCurFdbLayer->GetGroupName(),MAX_STRLEN-1);
	}
	else
	{
		if(m_dlgDxfExport.m_bUseIndex)
		{
			__int64 code;
			const char *name = (const char *)(LPCTSTR)strLayerName;
			if(pScheme && pScheme->FindLayerIdx(FALSE,code,name))
			{
				strLayerName.Format("%I64d", code);
			}
		}
		
		CString strMatchName;
		SubSymbols arr;
		GetExportedSubsymbols(strLayerName,arr);

		for( int i=0; i<arr.GetSize(); i++)
		{
			if( arr[i].index==nSymIndex )
			{
				strMatchName = arr[i].layer;
				break;
			}
		}

		//如果没有写简单的层对照项，就看子符号中序号为0的对照项
		if( nSymIndex==-1 && i>=arr.GetSize() )
		{
			for( i=0; i<arr.GetSize(); i++)
			{
				if( arr[i].index==0 )
				{
					strMatchName = arr[i].layer;
					break;
				}
			}

			if(strMatchName.GetLength()<=0 && arr.GetSize()>0 )
			{
				strLayerName = arr[0].layer;
			}
		}

		if(!strMatchName.IsEmpty())
			strLayerName = strMatchName;
		
		strncpy(layName,strLayerName,MAX_STRLEN-1);
		
	}
	
	NormalizeName(layName);

	TruncateString(layName,32);
	layName[31]  = 0;

	return CString(layName);
}

BOOL CDxfWrite::SaveFtrLayer(CFtrLayer *pLayer)
{
	if (!pLayer || !pLayer->IsVisible())  return FALSE;

	//若层无地物则不添加
	if(pLayer->GetObjectCount() < 1)
		return FALSE;

	m_curFdbLayerName = pLayer->GetName();
	m_pCurFdbLayer = pLayer;
	COLORREF color=-1;
	SubSymbols arr;
	GetExportedSubsymbols(m_curFdbLayerName,arr);
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	CConfigLibManager *pCfgLibManager = pApp->GetConfigLibManager();
	CScheme *pScheme = pCfgLibManager->GetScheme(m_pDS->GetScale());
	CString layName;
	int size = m_lstFileEx.m_ListFile.GetSize();
	for (int i = 0;i<size;i++)
	{
		CString strt = m_lstFileEx.m_ListFile.GetAt(i)->m_strGroup;	
		CString	strtt = m_lstFileEx.m_ListFile.GetAt(i)->m_strColour;
		if (!m_lstFileEx.m_ListFile.GetAt(i)->m_bHasLayer&&m_lstFileEx.m_ListFile.GetAt(i)->m_strGroup.CompareNoCase(pLayer->GetGroupName())==0)
		{
			color = strtoul(strtt,NULL,16);
		}
		else if (m_lstFileEx.m_ListFile.GetAt(i)->m_bHasLayer&&m_lstFileEx.m_ListFile.GetAt(i)->m_strGroup.CompareNoCase(pLayer->GetGroupName())==0)
		{
			int nsize = m_lstFileEx.m_ListFile.GetAt(i)->m_lstOne.GetSize();
			for (int j = 0;j<nsize;j++)
			{
				CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(pLayer->GetName());
				if (!pSchemeLayer)
				{
					continue;
				}
				char strID[20];
				sprintf(strID, "%I64d", pSchemeLayer->GetLayerCode());
				CString str = m_lstFileEx.m_ListFile.GetAt(i)->m_lstOne.GetAt(j);
				CString str1 = m_lstFileEx.m_ListFile.GetAt(i)->m_lstTwo.GetAt(j);
				if(0 == str1.Compare(strID))
				{
					color = strtoul(str,NULL,16);	
					break;
				}
				else
				{
					color = strtoul(strtt,NULL,16);
				}
			}
		}
	}
	if (-1 == color)
	{
		color = pLayer->GetColor();
	}

	for( i=0; i<arr.GetSize(); i++)
	{
		layName = GetExportedLayerName(m_curFdbLayerName,arr[i].index);
		if( !layName.IsEmpty() )
			SaveLayer(layName,color,0);
	}

	layName = GetExportedLayerName(m_curFdbLayerName,-1);
	if (layName.IsEmpty())  return FALSE;

	SaveLayer(layName,color,0);

	return TRUE;

}


BOOL CDxfWrite::SaveLayer(LPCTSTR layName, COLORREF color, int lineWeight)
{
	if (!m_bExportDWG)
	{
		LAYER Layer;
		strcpy(Layer.Name,layName);
		
		BOOL bFindLayer = FALSE;
		
		if(m_DxfDraw.FindTableType(TAB_LAYER,FIND_BYNAME,&Layer) > 0)
			bFindLayer = TRUE;
		
		if(!bFindLayer)
		{
			Layer.StandardFlags = 0;
			Layer.Color = ConverttoIndexOfCAD( color );
			if( Layer.Color==0 )Layer.Color = 255;
			Layer.LineTypeObjhandle = m_hDefLinetype;
			Layer.LineWeight = lineWeight;
			Layer.PlotFlag = FALSE;
			Layer.PlotStyleObjhandle = 0;
			
			m_DxfDraw.AddLayer(&Layer);
			
		}
		
		//设置为当前层
		m_DxfDraw.SetLayer(Layer.Name);
	}
	else if( m_pDb )
	{
		HRESULT        hr;
		IAcadLayers*  pLayers = NULL;
		hr = m_pDb->get_Layers(&pLayers);
		
		if (S_OK == hr)
		{
			CComPtr<IUnknown>                           pUnk;
			CComQIPtr<IEnumVARIANT, &IID_IEnumVARIANT>  pNewEnum;
			CComQIPtr<IAcadLayer, &IID_IAcadLayer>      pEnt;
			
			BOOL bFindLayer = FALSE;
			
			if (SUCCEEDED(pLayers->get__NewEnum(&pUnk)) && pUnk != NULL)
			{
				VARIANT   varItem;
				VariantInit(&varItem);
				
				pNewEnum = pUnk;
				while (pNewEnum->Next(1, &varItem, NULL) == S_OK)
				{
					BSTR   bName;
					
					pEnt = varItem.pdispVal;
					VariantClear(&varItem);
					
					pEnt->get_Name(&bName);
					
					CString strLayerName = WCharToChar(bName);
					
					if (strLayerName.CompareNoCase(layName) == 0)
					{
						bFindLayer = TRUE;
					}
				}
			}
			
			if (!bFindLayer)
			{
				long col = ConverttoIndexOfCAD(color);
				if (col == 0) col = 255;
				
				IAcadLayer *pLayer1;
				pLayers->Add(CharToWChar(layName),&pLayer1);

				if( pLayer1 )
				{
					pLayer1->put_color(AcColor(col));					
					pLayer1->Release();
				}				
			}
			
			pLayers->Release();
			
		}
		else
		{
			return FALSE;
		}
		
	}
	
	return TRUE;
}


void CDxfWrite::GetExportedSubsymbols(LPCTSTR layname, SubSymbols& arr)
{
	if( m_FdbLayerNameOfSubSymbols.CompareNoCase(layname)==0 )
	{
		arr.Copy(m_SubSymbols);
		return;
	}

	m_FdbLayerNameOfSubSymbols = layname;

	char code[256] = {0};
	strcpy(code,layname);

	if( m_dlgDxfExport.m_bUseIndex || m_dlgDxfExport.m_bUseLayerGroup )
	{
		__int64 nCode = 0;
		LPCTSTR name2 = layname;
		if( m_pDS->FindLayerIdx(FALSE,nCode,name2) )
		{
			sprintf(code,"%I64d", nCode);
		}
	}

	layname = code;

	for( int i=0; i<m_lstFile.GetRowCount(); i++)
	{
		CStringArray *p = m_lstFile.GetRow(i);
		if( p && p->GetSize()>=2 )
		{
			if( p->GetAt(0).CompareNoCase(layname)==0 )
			{
				SubSymbolItem item;

				if( p->GetSize()>5 )
				{
					item.index = atol(p->GetAt(2));
					strncpy(item.name,p->GetAt(3),sizeof(item.name)-1);
					item.type = atol(p->GetAt(4));
					strncpy(item.cassCode,p->GetAt(5),sizeof(item.cassCode)-1);
				}
				else
				{
					item.index = -1;
					item.type = 0;
				}
				strncpy(item.layer,p->GetAt(1),sizeof(item.layer)-1);
				arr.Add(item);
			}
		}
	}

	m_SubSymbols.Copy(arr);

	return;
}


int CDxfWrite::GetValidNumOfSubsymbols(SubSymbols& arr)
{
	int num = 0, size = arr.GetSize();
	for( int i=0; i<size; i++)
	{
		if( arr[i].index>0 && arr[i].name[0]!='*' )
			num++;
	}
	return num;
}


BOOL CDxfWrite::AddSolidLinetype(LPCTSTR name)
{
	if( name==NULL || strlen(name)<=0 )
		return FALSE;

	char name2[300];
	strcpy(name2,name);
	NormalizeName(name2);
	
	name = name2;

	if( !m_bExportDWG )
	{
		LTYPE ltype;
		memset(&ltype,0,sizeof(ltype));
		strcpy(ltype.Name, name);

		if( m_DxfDraw.FindTableType(TAB_LTYPE,FIND_BYNAME,&ltype)!=0 )
			return TRUE;

		strcpy(ltype.DescriptiveText, "Solid line");
		m_DxfDraw.AddLinetype(&ltype);
	}
	else if( m_pDb )
	{
		IAcadLineTypes *pLinetypes = NULL;
		m_pDb->get_Linetypes(&pLinetypes);

		IAcadLineType *pLinetype = NULL;

		long nCount = 0;
		pLinetypes->get_Count(&nCount);
		for( int i=0; i<nCount; i++)
		{
			pLinetypes->Item((_variant_t)(long)i,&pLinetype);

			BSTR wname = NULL;
			if( pLinetype && pLinetype->get_Name(&wname)==0 )
			{
				pLinetype->Release();

				if( stricmp(name,WCharToChar(wname))==0 )
					break;
			}
		}

		if( i<nCount )
		{
			pLinetypes->Release();
			return TRUE;
		}
		
		pLinetypes->Add(CharToWChar(name),&pLinetype);
		
		DWGdirectX::IOdaLineTypePtr pLT = (DWGdirectX::IOdaLineTypePtr)pLinetype;
		if( pLT )
		{
			pLT->put_PatternLength(0);
			pLT->put_NumDashes(0);
			
			pLT = NULL;
		}
		
		if( pLinetype )
			pLinetype->Release();

		pLinetypes->Release();
	}
	return TRUE;
}


BOOL CDxfWrite::AddDashLinetype(LPCTSTR name,CDashLinetype *pLT)
{
	if( name==NULL || strlen(name)<=0 )
		return FALSE;

	CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
	BaseLineType lt = pLineLib->GetBaseLineType(pLT->m_strBaseLinetypeName);

	char name2[300];
	strcpy(name2,name);
	NormalizeName(name2);

	name = name2;

	lt.m_nNum = (lt.m_nNum/2) * 2;

	float allLen = 0;
	for( int i=0; i<lt.m_nNum; i++)
	{
		allLen += fabs(lt.m_fLens[i]);
	}
	if( lt.m_nNum<=1 || allLen<0.1 )
		return AddSolidLinetype(name);

	float scale = m_pDS->GetScale()*0.001;

	if( !m_bExportDWG )
	{
		LTYPE ltype;
		memset(&ltype,0,sizeof(ltype));
		strcpy(ltype.Name, name);

		if( m_DxfDraw.FindTableType(TAB_LTYPE, FIND_BYNAME, &ltype)!=0 )
			return TRUE;

		strcpy(ltype.DescriptiveText, "Dash line");

		ltype.ElementsNumber = lt.m_nNum;
		for( i=0; i<lt.m_nNum; i++)
		{
			if( (i%2)==0 )
				ltype.Elements[i] = fabs(lt.m_fLens[i])*scale;
			else
				ltype.Elements[i] = -fabs(lt.m_fLens[i])*scale;

		}

		ltype.PatternLength = allLen*scale;

		m_DxfDraw.AddLinetype(&ltype);
	}
	else if( m_pDb )
	{		
		IAcadLineTypes *pLinetypes = NULL;
		m_pDb->get_Linetypes(&pLinetypes);
		
		IAcadLineType *pLinetype = NULL;
		
		long nCount = 0;
		pLinetypes->get_Count(&nCount);
		for( int i=0; i<nCount; i++)
		{
			pLinetypes->Item((_variant_t)(long)i,&pLinetype);
			
			BSTR wname = NULL;
			if( pLinetype && pLinetype->get_Name(&wname)==0 )
			{
				pLinetype->Release();
				
				if( stricmp(name,WCharToChar(wname))==0 )
					break;
			}
		}
		
		if( i<nCount )
		{
			pLinetypes->Release();
			return TRUE;
		}
		
		pLinetypes->Add(CharToWChar(name),&pLinetype);
		
		DWGdirectX::IOdaLineTypePtr pLT = (DWGdirectX::IOdaLineTypePtr)pLinetype;
		if( pLT )
		{
			pLT->put_PatternLength(allLen*scale);
			pLT->put_NumDashes(lt.m_nNum);

			for( i=0; i<lt.m_nNum; i++)
			{
				if( (i%2)==0 )
					pLT->put_DashLengthAt(i,fabs(lt.m_fLens[i])*scale);
				else
					pLT->put_DashLengthAt(i,-fabs(lt.m_fLens[i])*scale);

			}
			
			pLT = NULL;
		}
		 
		if( pLinetype )
			pLinetype->Release();
		pLinetypes->Release();
	}

	return TRUE;
}

CString CDxfWrite::AddTextStyle(LPCTSTR name,LPCTSTR font,float fWidthFactor, int inclinedtype, float fAngle)
{
	CString fontFile = m_fontFileFinder.FindFontFile(font);
	if( fontFile.IsEmpty() )
		return CString();

	CString name2 = name;
	if( name2.IsEmpty() )
	{
		if( fWidthFactor==1.0f && fAngle==0.0f )
			name2 = font;
		else
			name2.Format("%s_%d_%d",font,(int)(fWidthFactor*10),(int)fAngle);
	}

	NormalizeName(name2);

	name = name2.GetBuffer(1);

	if( inclinedtype==SHRUGN )
		fAngle = 0;
	else if( inclinedtype==SHRUGL )
	{
		fAngle = 360-fAngle;
	}

	if( !m_bExportDWG )
	{
		STYLE style;
		memset(&style,0,sizeof(style));

		strcpy(style.Name,name);

		if( m_DxfDraw.FindTableType(TAB_STYLE, FIND_BYNAME, &style)!=0 )
			return name2;

		style.ObliqueAngle = fAngle;
		style.WidthFactor = fWidthFactor;
		strcpy(style.PrimaryFontFilename,fontFile);

		m_DxfDraw.AddTextStyle(&style);
		
	}
	else if( m_pDb )
	{
		IAcadTextStyles *pStyles = NULL;
		m_pDb->get_TextStyles(&pStyles);
		
		IAcadTextStyle *pStyle = NULL;
		
		long nCount = 0;
		pStyles->get_Count(&nCount);
		for( int i=0; i<nCount; i++)
		{
			pStyles->Item((_variant_t)(long)i,&pStyle);
			
			BSTR wname = NULL;
			if( pStyle && pStyle->get_Name(&wname)==0 )
			{
				pStyle->Release();
				
				if( stricmp(name,WCharToChar(wname))==0 )
					break;
			}
		}
		
		if( i<nCount )
		{
			pStyles->Release();
			return name2;
		}
		
		pStyles->Add(CharToWChar(name),&pStyle);
		
		DWGdirectX::IAcadTextStylePtr pTS = (DWGdirectX::IAcadTextStylePtr)pStyle;
		if( pTS )
		{
			pTS->put_fontFile(CharToWChar(fontFile));
			pTS->put_ObliqueAngle((double)fAngle);
			pTS->put_Width((double)fWidthFactor);
			
			pTS = NULL;
		}
		
		if( pStyle )
			pStyle->Release();
		pStyles->Release();
	}

	name2.ReleaseBuffer();

	return name2;
}

void CDxfWrite::AddDefaultLayer_dxf()
{	
	if( !m_bExportDWG )
	{
		m_hDefLinetype = m_hDefLayer = 0;	

		LAYER Layer;
		LTYPE ltype;
		memset(&ltype,0,sizeof(ltype));
		strcpy(ltype.Name, "Continuous");
		strcpy(ltype.DescriptiveText, "Solid line");
		m_hDefLinetype = m_DxfDraw.AddLinetype(&ltype);
		
		Layer.StandardFlags = 0;
		Layer.Color = ConverttoIndexOfCAD(RGB(255,255,255));
		Layer.LineTypeObjhandle = m_hDefLinetype;
		Layer.LineWeight = 0;
		Layer.PlotFlag = FALSE;
		Layer.PlotStyleObjhandle = 0;
		strcpy(Layer.Name,"0");
		
		m_hDefLayer = m_DxfDraw.AddLayer(&Layer);
	}
}


void CDxfWrite::SavePointSubSymbol(CFeature *pFtr, SubSymbolItem *pItem, CSymbolArray& arrPSyms)
{
	if( pItem->index<0 || pItem->index>arrPSyms.GetSize() )
		return;

	//*表示该子符号不导出
	if( pItem->name[0]=='*' )
		return;

	int index = pItem->index;

	if(index<=0)
		return;

	if( index==0 )
	{
		if( arrPSyms.GetSize()==1 && arrPSyms[0]->GetType()==SYMTYPE_CELL )
			index = 1;
		else if( arrPSyms.GetSize()==0 )
		{
			SavePoint(pFtr->GetGeometry(),pItem->layer,pItem->name,pItem->cassCode);
			return;
		}
		else
			return;
	}

	CSymbol *pSym = (CSymbol*)arrPSyms[index-1];
	if( pSym->GetType()==SYMTYPE_CELL )
	{
		CGeoArray arrPGeos;
		
		CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(pFtr);
		if( !pLayer )
			pLayer = m_pCurFdbLayer;
		if( !pLayer )
			return;
		
		if( pSym->ExplodeSimple(pFtr,m_pDS->GetSymbolDrawScale(),arrPGeos) )
		{
			for( int i=0; i<arrPGeos.GetSize(); i++)
			{
				CFeature *pNewFtr = pFtr->Clone();
				pNewFtr->SetGeometry(arrPGeos[i]);

				if( pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER )
				{
					//arrPGeos[i]->SetColor(COLOUR_BYLAYER);
				}

				CString symName = pItem->name;
				
				CSymbolArray arrPSyms2;
				CPtrArray arr;
				gpCfgLibMan->GetSymbol(m_pDS,pNewFtr,arr,pLayer->GetName());
				for(int j=0; j<arr.GetSize(); j++)
				{
					arrPSyms2.Add((CSymbol*)arr[j]);
				}
				
				if( arrPSyms2.GetSize()==1 && (pItem->type==0 || pItem->type==1) )
				{
					long clr_ftr = pNewFtr->GetGeometry()->GetColor();
					if( clr_ftr==FTRCOLOR_BYLAYER )
					{
						CString strWid;
						
						CGeometry *pGeo2 = arrPGeos[i];
						if( pGeo2->GetClassType()==CLS_GEOPOINT && ((CGeoPoint*)pGeo2)->m_fWidth!=0.0f )
						{
							strWid.Format("%f",((CGeoPoint*)pGeo2)->m_fWidth);
							TrimRZero(strWid);
						}

						if( symName.GetLength()==0 || symName=="0" )
							symName = CString(m_pCurFdbLayer->GetName())+arrPSyms2[0]->GetName()+strWid;
						SetBlock(pNewFtr,pItem->layer,arrPSyms2,FALSE,symName,pLayer,pItem->cassCode);
					}
					else
					{
						CString strColor;
						strColor.Format("%02x",ConverttoIndexOfCAD(clr_ftr));

						CString strWid;
						
						CGeometry *pGeo2 = arrPGeos[i];
						if( pGeo2->GetClassType()==CLS_GEOPOINT && ((CGeoPoint*)pGeo2)->m_fWidth!=0.0f )
						{
							strWid.Format("%f",((CGeoPoint*)pGeo2)->m_fWidth);
							TrimRZero(strWid);
						}

						strColor += strWid;

						if( symName.GetLength()==0 || symName=="0" )
							symName = CString(m_pCurFdbLayer->GetName())+arrPSyms2[0]->GetName()+strColor;
						
						SetBlock(pNewFtr,pItem->layer,arrPSyms2,FALSE,symName,pLayer,pItem->cassCode);
					}
				}
				else
					SetBlock_exploded(pNewFtr,pItem->layer,arrPSyms2,pLayer,pItem->cassCode);
				
				delete pNewFtr;
			}
			return;
		}
		else
		{
			SavePoint(pFtr->GetGeometry(),pItem->layer,NULL,pItem->cassCode);
		}
	}
	else if( pSym->GetType()==SYMTYPE_ANNOTATION )
	{
		CGeoArray arrPGeos;
		
		CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(pFtr);
		if( !pLayer )
			pLayer = m_pCurFdbLayer;
		if( !pLayer )
			return;

		CValueTable tab;
		if( ((CAnnotation*)pSym)->m_nAnnoType==CAnnotation::Attribute )
		{
			m_pDS->GetAllAttribute(pFtr,tab);
		}
		
		if( ((CAnnotation*)pSym)->Explode(pFtr,tab,m_pDS->GetSymbolDrawScale(),arrPGeos) )
		{
			for( int i=0; i<arrPGeos.GetSize(); i++)
			{
				CFeature *pNewFtr = pFtr->Clone();
				pNewFtr->SetGeometry(arrPGeos[i]);

				if( arrPGeos[i]->IsKindOf(RUNTIME_CLASS(CGeoText)) )
				{
					SaveText((CGeoText*)arrPGeos[i],pItem->layer,pItem->cassCode);
				}

				delete pNewFtr;
			}
			return;
		}
	}

	CSymbolArray arr;
	arr.Add(pSym);
	SetBlock(pFtr, pItem->layer,arr,FALSE,NULL,NULL,pItem->cassCode);

}


void CDxfWrite::SaveHatch(IAcadBlock *pBlk, CGeometry *pGeoObj, LPCTSTR dxfLayName, LPCTSTR linetype_name, float wid, float thickness, LPCTSTR cassCode)
{
	if (!pGeoObj)  return;
	CGeoCurveBase *pCurve = NULL;
	if ( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )	
		pCurve = (CGeoCurveBase*)pGeoObj;
	else
		return;
	
	long color = ConvertColor(pGeoObj->GetColor(),NULL);
	
	if( !linetype_name )
		linetype_name = "Continuous";
	
	if( wid==-1 )
	{
		if ( pCurve->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )	
			wid = ((CGeoCurve*)pCurve)->m_fLineWidth;
		else
			wid = ((CGeoSurface*)pCurve)->m_fLineWidth;

		wid *= m_pDS->GetSymbolDrawScale();
	}
	
	if( thickness==-1 )
		thickness = wid;

	CComBSTR wstrName = CharToWChar(linetype_name);

	if( m_bExportDWG && m_pDb )
	{
		VARIANT varPts;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeoObj->GetShape()->GetPts(arrPts);
		int npt = GraphAPI::GKickoffSame2DPoints(arrPts.GetData(),arrPts.GetSize());

		arrPts.SetSize(npt);
		
		IAcadEntity *pEntity = NULL;
		IAcad3DPolyline *pPoly1 = NULL;
		IAcadLWPolyline *pPoly2 = NULL;
		IAcadModelSpace *pMS = NULL;
		pBlk->QueryInterface(&pMS);

		BOOL bIsBlock = TRUE;
		if(pMS)
		{
			bIsBlock = FALSE;
			pMS->Release();
		}

		// 三维输出
		if (m_dlgDxfExport.m_b3DLines)
		{
			Create3DPointArray(arrPts.GetData(),arrPts.GetSize(),&varPts,sizeof(PT_3DEX));
			
			pBlk->Add3DPoly(varPts,&pPoly1);

			if( pPoly1 )
			{
				pPoly1->put_Layer(CharToWChar(dxfLayName));
				pPoly1->put_Closed(VARIANT_TRUE);

				if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
				{
					pPoly1->put_TrueColor(TrueColor(color));
				}
				else
				{
					pPoly1->put_color(ACAD_COLOR(color));
				}
				if(bIsBlock)
				{
					pPoly1->put_color(acByBlock);
				}
				
				_variant_t  vParam;	
				vParam.vt = VT_EMPTY; 
				IAcadHatch *pHatch = NULL;
				pBlk->AddHatch(acHatchPatternTypePreDefined,L"SOLID",VARIANT_TRUE,vParam,&pHatch);

				if( pHatch )
				{				
					VARIANT var2;
					CreateEntityArray(pPoly1,&var2);
					
					HRESULT hr = pHatch->AppendOuterLoop(var2);
					hr = pHatch->Evaluate();

					ArrayClear(&var2);

					pEntity = pHatch;
				}
			}			
			
			ArrayClear(&varPts);
		}
		else
		{
			Create3DPointArray(arrPts.GetData(),arrPts.GetSize(),&varPts,sizeof(PT_3DEX),FALSE);

			//SaveLayer("HatchEdge",RGB(255,255,255),0);
			SaveLayer(dxfLayName,RGB(255,255,255),0);

			pBlk->AddLightWeightPolyline(varPts,&pPoly2);
			
			if( pPoly2 )
			{
				pPoly2->put_Layer(CharToWChar(dxfLayName));
				//pPoly2->put_Layer(CharToWChar("HatchEdge"));
				pPoly2->put_Elevation(arrPts[0].z);
				pPoly2->put_LinetypeGeneration(VARIANT_TRUE);
				
				pPoly2->put_ConstantWidth(0);
				pPoly2->put_Closed(VARIANT_TRUE);
				
				if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
				{
					pPoly2->put_TrueColor(TrueColor(color));
				}
				else
				{
					pPoly2->put_color(ACAD_COLOR(color));
				}

				if(bIsBlock)
				{
					pPoly2->put_color(acByBlock);
				}
				
				_variant_t  vParam;	
				vParam.vt = VT_EMPTY; 
				IAcadHatch *pHatch = NULL;
				pBlk->AddHatch(acHatchPatternTypePreDefined,L"SOLID",VARIANT_TRUE,vParam,&pHatch);
				
				if( pHatch )
				{
					pHatch->put_AssociativeHatch(VARIANT_TRUE);
					pHatch->put_Elevation(0.0);
					pHatch->put_HatchStyle(acHatchStyleNormal);

					VARIANT var2;
					CreateEntityArray(pPoly2,&var2);
					
					HRESULT hr = pHatch->AppendOuterLoop(var2);
					hr = pHatch->Evaluate();

					//WriteCassCode(pPoly2,"HatchEdge");
					WriteCassCode(pPoly2,dxfLayName);
					
					ArrayClear(&var2);
					
					pEntity = pHatch;					
				}
			}
			
			ArrayClear(&varPts);
		}	
		
		if (!pEntity) return;
		
		if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
		{
			pEntity->put_TrueColor(TrueColor(color));
		}
		else
		{
			pEntity->put_color(ACAD_COLOR(color));
		}

		if(bIsBlock)
		{
			pEntity->put_color(acByBlock);
		}

		pEntity->put_Linetype(wstrName);
		
		pEntity->put_Layer(CharToWChar(dxfLayName));

		WriteCassCode(pEntity,cassCode);
			
		pEntity->Release();

	}
	else if( !m_bExportDWG )
	{
		CGeometry *pNewObj = pGeoObj->Clone();
		if( pNewObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		{
			((CGeoSurface*)pNewObj)->EnableFillColor(FALSE,0);
		}
		
		SaveCurveOrSurface(pNewObj,dxfLayName,linetype_name,wid,thickness,cassCode);
		delete pNewObj;
	}
}

void CDxfWrite::SaveDCurveOrParallel(CGeometry *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR linetype_name, float wid, float thickness, LPCTSTR cassCode)
{
	if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
	{
		CGeometry *pCur1=NULL, *pCur2=NULL;
		if (pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
		{
			((CGeoParallel*)pGeoObj)->Separate(pCur1, pCur2);
		}
		else if (pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
		{
			((CGeoDCurve*)pGeoObj)->Separate(pCur1, pCur2);
		}
		
		if (pCur1 )
		{
			CGeoCurve *plineCurve1 = (CGeoCurve*)(pCur1)->Linearize();
			SaveCurveOrSurface(plineCurve1,dxfLayerName,linetype_name,wid,thickness,cassCode);
			
			if (plineCurve1)
			{
				delete plineCurve1;
				plineCurve1 = NULL;
			}
		}						
		
		if ( pCur2)
		{
			CGeoCurve *plineCurve2 = (CGeoCurve*)(pCur2)->Linearize();
			SaveCurveOrSurface(plineCurve2,dxfLayerName,linetype_name,wid,thickness,cassCode);
			
			if (plineCurve2)
			{
				delete plineCurve2;
				plineCurve2 = NULL;
			}
		}
		
		if (pCur1)
		{
			delete pCur1;
			pCur1 = NULL;
		}
		
		if (pCur2)
		{
			delete pCur2;
			pCur2 = NULL;
		}
	}
}


CGeometry *ReverseGeometry(CGeometry *pGeo)
{
	pGeo = pGeo->Clone();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);
	if(arrPts.GetSize()<=0)
		return NULL;
	
	arrPts[0].pencode = penLine;
	
	int geotype = pGeo->GetClassType();
	if(geotype==CLS_GEOCURVE || geotype==CLS_GEOPARALLEL || geotype==CLS_GEOSURFACE)
	{
		GraphAPI::ReversePoints(arrPts.GetData(),arrPts.GetSize());
		pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());

		if(geotype==CLS_GEOPARALLEL)
		{
			((CGeoParallel*)pGeo)->SetWidth(-((CGeoParallel*)pGeo)->GetWidth());
		}
	}
	else if(geotype==CLS_GEODCURVE)
	{
		GraphAPI::ReversePoints(arrPts.GetData(),arrPts.GetSize());
		for(int i=0; i<arrPts.GetSize(); i++)
		{
			if(arrPts[i].pencode==penMove && i<(arrPts.GetSize()-1))
			{
				arrPts[i].pencode==arrPts[i+1].pencode;
				arrPts[i+1].pencode = penMove;
			}
		}
		pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
	}

	return pGeo;
}


void CDxfWrite::SaveCurveSubSymbol(CFeature *pFtr, SubSymbolItem *pItem, CSymbolArray& arrPSyms)
{
	if( pItem->index<0 || pItem->index>arrPSyms.GetSize() )
		return;

	//*表示该子符号不导出
	if( pItem->name[0]=='*' )
		return;

	int index = pItem->index;
	if(index<=0)
		return;
	
	if( index==0 )
	{
		if( arrPSyms.GetSize()==1 && arrPSyms[0]->GetType()==SYMTYPE_DASHLINETYPE )
		{
			index = 1;
		}
		else if( arrPSyms.GetSize()==1 && arrPSyms[0]->GetType()==SYMTYPE_CAD )
		{
			CCADSymbol *pCadSym = (CCADSymbol*)arrPSyms[0];
			if( pCadSym->GetCADSymType()==CCADSymbol::typeLin )
			{
				index = 1;
			}
		}
		else
		{
			AddSolidLinetype(pItem->name);

			CGeometry *pGeo = pFtr->GetGeometry();

			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
				SaveDCurveOrParallel(pGeo,pItem->layer,pItem->name,-1,-1,pItem->cassCode);
			else
				SaveCurveOrSurface(pGeo,pItem->layer,pItem->name,-1,-1,pItem->cassCode);

			return;
		}
	}

	CSymbol *pSym = arrPSyms[index-1];
	
	int oldSymType = pSym->GetType();
	int newSymType = (pItem->type&0x0f);

	BOOL bCADHatch = FALSE, bCADDash = FALSE;

	//没有dwg符号名
	if( pItem->name[0]==0 || stricmp(pItem->name,"0")==0 )
	{
		//自动判断
		if( newSymType==0 )
		{
			//虚线线型，导出为线型
			if( oldSymType==SYMTYPE_DASHLINETYPE )
				newSymType = 2;
			else if( oldSymType==SYMTYPE_COLORHATCH )
				newSymType = 3;
			else if( oldSymType==SYMTYPE_ANNOTATION )
				newSymType = 4;
			else if( oldSymType==SYMTYPE_CAD )
			{
				CCADSymbol *pCadSym = (CCADSymbol*)arrPSyms[0];
				if( pCadSym->GetCADSymType()==CCADSymbol::typeLin )
				{
					bCADDash = TRUE;
					newSymType = 2;
				}
				else if( pCadSym->GetCADSymType()==CCADSymbol::typeHat )
				{
					bCADHatch = TRUE;
					newSymType = 1;
				}
			}
			//其他线型，导出为块
			else
				newSymType = 1;
		}	
	}
	else
	{
		//自动判断
		if( newSymType==0 )
		{
			//虚线线型，导出为线型
			if( oldSymType==SYMTYPE_DASHLINETYPE || oldSymType==SYMTYPE_CELLLINETYPE || oldSymType==SYMTYPE_SCALELINETYPE || oldSymType==SYMTYPE_LINEHATCH ||
				oldSymType==SYMTYPE_DIAGONAL || oldSymType==SYMTYPE_ANGBISECTORTYPE || oldSymType==SYMTYPE_SCALEARCTYPE )
				newSymType = 2;
			else if( oldSymType==SYMTYPE_ANNOTATION )
				newSymType = 4;
			else if( oldSymType==SYMTYPE_CAD )
			{
				CCADSymbol *pCadSym = (CCADSymbol*)arrPSyms[0];
				if( pCadSym->GetCADSymType()==CCADSymbol::typeLin )
				{
					bCADDash = TRUE;
					newSymType = 2;
				}
				else if( pCadSym->GetCADSymType()==CCADSymbol::typeHat )
				{
					bCADHatch = TRUE;
					newSymType = 1;
				}
			}
			//其他线型，导出为块
			else
				newSymType = 1;
		}
	}

	BOOL bReverse = ((pItem->type&0x10)!=0);
	newSymType = (newSymType&0x0f);

	//线符号；
	//由于dxfDrawing还不支持hatch元素，DWGdriectX还不支持hatch填充填充符号，所以暂时与线符号一致
	if( newSymType==2 /*&& (oldSymType==SYMTYPE_DASHLINETYPE || oldSymType==SYMTYPE_CELLLINETYPE||bCADDash)*/ )
	{
		CString symName = pItem->name;

		if( symName.GetLength()==0 || symName=="0" )
		{
			symName.Format("%s%d",m_pCurFdbLayer->GetName(),pItem->index);
			if( bCADDash )
				symName = pSym->GetName();
		}

		if( oldSymType==SYMTYPE_DASHLINETYPE )
		{
			AddDashLinetype(symName,(CDashLinetype*)pSym);
		}
		else
			AddSolidLinetype(symName);

		CGeoArray arrPGeos;		
		
		if( bCADDash )
		{
			CGeometry *pGeo = pFtr->GetGeometry();	
			if(bReverse)pGeo = ReverseGeometry(pGeo);

			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
				SaveDCurveOrParallel(pGeo,pItem->layer,symName,-1,-1,pItem->cassCode);
			else
				SaveCurveOrSurface(pGeo,pItem->layer,symName,-1,-1,pItem->cassCode);

			if(bReverse)delete pGeo;

		}
		else if( oldSymType==SYMTYPE_CELLLINETYPE )
		{
			CCellLinetype *pLT = (CCellLinetype*)pSym;
			CGeometry *pGeo = pFtr->GetGeometry();
			if(bReverse)pGeo = ReverseGeometry(pGeo);
			
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
			{
				CGeometry *pCur1=NULL, *pCur2=NULL;
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
				{
					((CGeoParallel*)pGeo)->Separate(pCur1, pCur2);
				}
				else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
				{
					((CGeoDCurve*)pGeo)->Separate(pCur1, pCur2);
				}
				
				if(pLT->m_nPlaceType==CCellLinetype::BaseLine && pCur1)
					SaveCurveOrSurface(pCur1,pItem->layer,symName,-1,-1,pItem->cassCode);
				else if(pLT->m_nPlaceType==CCellLinetype::AidLine && pCur2)
					SaveCurveOrSurface(pCur2,pItem->layer,symName,-1,-1,pItem->cassCode);
				else if(pLT->m_nPlaceType==CCellLinetype::MidLine && pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
				{
					CGeoCurve *pCenterLine = ((CGeoParallel*)pGeo)->GetCenterlineCurve();
					SaveCurveOrSurface(pCenterLine,pItem->layer,symName,-1,-1,pItem->cassCode);
					delete pCenterLine;
				}
				else
				{
					SaveCurveOrSurface(pGeo,pItem->layer,symName,-1,-1,pItem->cassCode);
				}

				if(pCur1)delete pCur1;
				if(pCur2)delete pCur2;
			}
			else
				SaveCurveOrSurface(pGeo,pItem->layer,symName,-1,-1,pItem->cassCode);

			if(bReverse)delete pGeo;
		}
		else
		{
			if( pSym->ExplodeSimple(pFtr,m_pDS->GetSymbolDrawScale(),arrPGeos) )
			{
				for( int i=0; i<arrPGeos.GetSize(); i++)
				{
					if( pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER )
					{
						//arrPGeos[i]->SetColor(COLOUR_BYLAYER);
					}

					if(bReverse)
					{
						CGeometry *p2 = ReverseGeometry(arrPGeos[i]);
						delete arrPGeos[i];
						arrPGeos[i] = p2;
					}

					SaveCurveOrSurface(arrPGeos[i],pItem->layer,symName,-1,-1,pItem->cassCode);
					delete arrPGeos[i];
				}
			}
			else
			{
				CGeometry *pGeo = pFtr->GetGeometry();	
				if(bReverse)pGeo = ReverseGeometry(pGeo);
				
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
					SaveDCurveOrParallel(pGeo,pItem->layer,NULL,-1,-1,pItem->cassCode);
				else
					SaveCurveOrSurface(pGeo,pItem->layer,NULL,-1,-1,pItem->cassCode);

				if(bReverse)delete pGeo;
			}
		}
	}
	//填充符号，
	else if( newSymType==3 )
	{
		//SaveHatch(m_pMS,pFtr->GetGeometry(),pItem->layer,pItem->name,-1,-1,pItem->cassCode);

		CGeoArray arrPGeos;
				
		if( pSym->ExplodeSimple(pFtr,m_pDS->GetSymbolDrawScale(),arrPGeos) )
		{
			for( int i=0; i<arrPGeos.GetSize(); i++)
			{
				CFeature *pNewFtr = pFtr->Clone();
				pNewFtr->SetGeometry(arrPGeos[i]);

				if( pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER )
				{
					//arrPGeos[i]->SetColor(COLOUR_BYLAYER);
				}
				
				if( arrPGeos[i]->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				{
					SaveHatch(m_pMS,arrPGeos[i],pItem->layer,pItem->name,-1,-1,pItem->cassCode);
				}
				
				delete pNewFtr;
			}
			return;
		}
	}
	//文字
	else if( newSymType==4 )
	{
		CGeoArray arrPGeos;
				
		if( pSym->ExplodeSimple(pFtr,m_pDS->GetSymbolDrawScale(),arrPGeos) )
		{
			for( int i=0; i<arrPGeos.GetSize(); i++)
			{
				CFeature *pNewFtr = pFtr->Clone();
				pNewFtr->SetGeometry(arrPGeos[i]);

				if( pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER )
				{
					//arrPGeos[i]->SetColor(COLOUR_BYLAYER);
				}
				
				if( arrPGeos[i]->IsKindOf(RUNTIME_CLASS(CGeoText)) )
				{
					SaveText((CGeoText*)arrPGeos[i],pItem->layer,pItem->cassCode);
				}
				
				delete pNewFtr;
			}
			return;
		}
		else
		{
			CGeometry *pGeo = pFtr->GetGeometry();		
			
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
				SaveDCurveOrParallel(pGeo,pItem->layer,NULL,-1,-1,pItem->cassCode);
			else
				SaveCurveOrSurface(pGeo,pItem->layer,NULL,-1,-1,pItem->cassCode);
		}
	}
	//打散导出
	else if( newSymType==8 )
	{
		CSymbolArray arrPSyms;
		arrPSyms.Add(pSym);

		SetBlock_exploded(pFtr, pItem->layer,arrPSyms,NULL,pItem->cassCode);
		return;

		CGeoArray arrPGeos;
		
		if( pSym->Explode(pFtr,m_pDS->GetSymbolDrawScale(),arrPGeos) )
		{
			for( int i=0; i<arrPGeos.GetSize(); i++)
			{
				CGeometry *pGeo = arrPGeos[i];
				
				if( pGeo->GetColor()==COLOUR_BYLAYER )
				{
					//pGeo->SetColor(COLOUR_BYLAYER);
				}
				
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
				{
					SavePoint((CGeoPoint*)pGeo,pItem->layer,NULL,pItem->cassCode);
				}
				else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
				{
					SaveText((CGeoText*)pGeo,pItem->layer,pItem->cassCode);
				}
				else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				{
					SaveCurveOrSurface((CGeoCurveBase*)pGeo,pItem->layer,NULL,-1,-1,pItem->cassCode);
				}
				
				delete pGeo;
			}
			return;
		}
		else
		{
			CGeometry *pGeo = pFtr->GetGeometry();		
			
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
				SaveDCurveOrParallel(pGeo,pItem->layer,NULL,-1,-1,pItem->cassCode);
			else
				SaveCurveOrSurface(pGeo,pItem->layer,NULL,-1,-1,pItem->cassCode);
		}
	}
	//块符号(newSymType==1 或者为无效值时)
	else
	{
		if( oldSymType==SYMTYPE_CELL || oldSymType==SYMTYPE_CELLLINETYPE || oldSymType==SYMTYPE_CELLHATCH || bCADHatch )
		{
			CGeoArray arrPGeos;
						
			if( pSym->ExplodeSimple(pFtr,m_pDS->GetSymbolDrawScale(),arrPGeos) )
			{
				for( int i=0; i<arrPGeos.GetSize(); i++)
				{
					CFeature *pNewFtr = pFtr->Clone();
					pNewFtr->SetGeometry(arrPGeos[i]);

					if( pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER )
					{
						//arrPGeos[i]->SetColor(COLOUR_BYLAYER);
					}
					
					CSymbolArray arrPSyms2;
					CPtrArray arr;
					gpCfgLibMan->GetSymbol(m_pDS,pNewFtr,arr,m_pCurFdbLayer->GetName());
					for(int j=0; j<arr.GetSize(); j++)
					{
						arrPSyms2.Add((CSymbol*)arr[j]);
					}
					
					if( arrPSyms2.GetSize()==1 )
					{
						CString symName = pItem->name;

						long clr_ftr = pNewFtr->GetGeometry()->GetColor();
						if( clr_ftr==FTRCOLOR_BYLAYER )
						{
							CString strWid;

							CGeometry *pGeo2 = arrPGeos[i];
							if( pGeo2->GetClassType()==CLS_GEOPOINT && ((CGeoPoint*)pGeo2)->m_fWidth!=0.0f )
							{
								strWid.Format("%f",((CGeoPoint*)pGeo2)->m_fWidth);
								TrimRZero(strWid);
							}							

							if( symName.GetLength()==0 || symName=="0" )
								symName = CString(m_pCurFdbLayer->GetName())+arrPSyms2[0]->GetName()+strWid;

							SetBlock(pNewFtr,pItem->layer,arrPSyms2,FALSE,symName,m_pCurFdbLayer,pItem->cassCode);
						}
						else
						{
							CString strColor;
							strColor.Format("%02x",ConverttoIndexOfCAD(clr_ftr));

							CString strWid;
							CGeometry *pGeo2 = arrPGeos[i];
							if( pGeo2->GetClassType()==CLS_GEOPOINT && ((CGeoPoint*)pGeo2)->m_fWidth!=0.0f )
							{
								strWid.Format("%f",((CGeoPoint*)pGeo2)->m_fWidth);
								TrimRZero(strWid);
							}

							strColor += strWid;

							symName = pItem->name;
							if( symName.GetLength()==0 || symName=="0" )
								symName = CString(m_pCurFdbLayer->GetName())+arrPSyms2[0]->GetName()+strColor;

							SetBlock(pNewFtr,pItem->layer,arrPSyms2,FALSE,symName,m_pCurFdbLayer,pItem->cassCode);
						}
					}
					else
						SetBlock_exploded(pNewFtr,pItem->layer,arrPSyms2,m_pCurFdbLayer,pItem->cassCode);
					
					delete pNewFtr;
				}
			}
			else
			{
				CGeometry *pGeo = pFtr->GetGeometry();		
				
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
					SaveDCurveOrParallel(pGeo,pItem->layer,NULL,-1,-1,pItem->cassCode);
				else
					SaveCurveOrSurface(pGeo,pItem->layer,NULL,-1,-1,pItem->cassCode);
			}

			return;
		}
		
		CSymbolArray arrPSyms;
		arrPSyms.Add(pSym);

		CString symName = pItem->name;
		
		if( symName.GetLength()==0 || symName=="0" )
			symName.Format("%s%d",pItem->layer,pItem->index);
		
		if( oldSymType==SYMTYPE_CELL )
		{
			long clr_ftr = pFtr->GetGeometry()->GetColor();

			if( clr_ftr==FTRCOLOR_BYLAYER )
				SetBlock(pFtr, pItem->layer,arrPSyms,FALSE,symName,m_pCurFdbLayer,pItem->cassCode);
			else
			{
				CString strColor;
				strColor.Format("%02x",ConverttoIndexOfCAD(clr_ftr));
				SetBlock(pFtr, pItem->layer,arrPSyms,FALSE,symName+strColor,m_pCurFdbLayer,pItem->cassCode);

			}
		}
		else
			SetBlock_exploded(pFtr, pItem->layer,arrPSyms,NULL,pItem->cassCode);
		
	}
}




void CDxfWrite::KickoffSymbols(CFeature *pFtr, CSymbolArray& arrPSyms, SubSymbols& arr)
{
	CSymbolArray arrPSyms2;
	arrPSyms2.Copy(arrPSyms);
	for( int i=0; i<arr.GetSize(); i++)
	{
		SubSymbolItem item = arr[i];

		//只有一个虚线线型，同时对照表中的子符号索引为0，就将它等于1处理
		if( item.index==0 && arrPSyms.GetSize()==1 )
		{
			if( ( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && arrPSyms[0]->GetType()==SYMTYPE_DASHLINETYPE ) ||
				( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) && arrPSyms[0]->GetType()==SYMTYPE_CELL ) )
				arrPSyms2[i] = NULL;
		}

		if( item.index>=1 && (item.index-1)<arrPSyms.GetSize() )
		{
			arrPSyms2[item.index-1] = NULL;
		}
	}
	for( i=arrPSyms2.GetSize()-1; i>=0; i--)
	{
		if( arrPSyms2[i]==NULL )
		{
			arrPSyms2.RemoveAt(i);
		}
	}
	arrPSyms.Copy(arrPSyms2);
}


void CDxfWrite::GetSymbols(CFeature *pFt, CSymbolArray& arrAllSyms, CSymbolArray& arrAnnoSyms, CSymbolArray& arrNotAnnoSyms)
{
	arrAllSyms.RemoveAll();
	arrAnnoSyms.RemoveAll();
	arrNotAnnoSyms.RemoveAll();

	CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(pFt);
	if( pLayer && !pLayer->IsSymbolized() )
		return;

	CPtrArray arr;

	if( pLayer==NULL )
		gpCfgLibMan->GetSymbol(m_pDS,pFt,arr,m_curFdbLayerName);
	else
		gpCfgLibMan->GetSymbol(m_pDS,pFt,arr);

	for(int j=0; j<arr.GetSize(); j++)
	{
		CSymbol *pSymbol = (CSymbol*)arr[j];
		if( pSymbol->GetType()==SYMTYPE_ANNOTATION )
			arrAnnoSyms.Add(pSymbol);
		else
			arrNotAnnoSyms.Add(pSymbol);

		arrAllSyms.Add(pSymbol);
	}
}


void CDxfWrite::ClassifySymbols(CSymbolArray& arrAllSyms, CSymbolArray& arrAnnoSyms, CSymbolArray& arrNotAnnoSyms)
{
	arrAnnoSyms.RemoveAll();
	arrNotAnnoSyms.RemoveAll();
	for(int j=0; j<arrAllSyms.GetSize(); j++)
	{
		CSymbol *pSymbol = arrAllSyms[j];
		if( pSymbol->GetType()==SYMTYPE_ANNOTATION )
			arrAnnoSyms.Add(pSymbol);
		else
			arrNotAnnoSyms.Add(pSymbol);
	}
}


long CDxfWrite::ConvertColor(COLORREF clr, CFtrLayer *pLayer)
{
	long color;

	if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
	{
		if ((long)clr==FTRCOLOR_BYLAYER)
		{
			if( pLayer!=NULL )
				color = pLayer->GetColor();
			else if( m_pCurFdbLayer!=NULL )
				color = m_pCurFdbLayer->GetColor();
			else
				color = 256;
		}
		else
		{
			color = clr;
		}		

		return color;
	}

	if ((long)clr==FTRCOLOR_BYLAYER)
	{
		if( 0 && m_bExportDWG )
		{
			return acByLayer;
		}

		if( pLayer!=NULL )
			color = ConverttoIndexOfCAD(pLayer->GetColor());
		else if( m_pCurFdbLayer!=NULL )
			color = ConverttoIndexOfCAD(m_pCurFdbLayer->GetColor());
		else
			color = 256;
	}
	else
	{
		color = ConverttoIndexOfCAD(clr);
		if (color == 0) color = 255;
	}
	return color;
}



BOOL CDxfWrite::SaveFeature(CFeature *pFt, BOOL bDem, LPCTSTR layName)
{	
	if (!m_pDS) return FALSE;	

	if (!pFt || !pFt->IsVisible())  return FALSE;

	BOOL bExportObj = TRUE;
	CGeometry *pGeoObj = pFt->GetGeometry();
	CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(pFt);

	long color = ConvertColor(pGeoObj->GetColor(),pLayer);

	CString strLayerName;

	if( pLayer==NULL )
		strLayerName = GetExportedLayerName(layName,-1);
	else
		strLayerName = GetExportedLayerName(pLayer->GetName(),-1);
	
	if (strLayerName.IsEmpty()) return FALSE;

	CString strSymLayerName = strLayerName + StrFromResID(IDS_LAYERNAME_SYM);
	if( !m_dlgDxfExport.m_bDifferBaselineSym )
		strSymLayerName = strLayerName;

	pGeoObj = pFt->GetGeometry();
	
	SubSymbols arrItems;
	GetExportedSubsymbols(m_curFdbLayerName,arrItems);
	
	int nValidSubsymbols = GetValidNumOfSubsymbols(arrItems);
	
	CSymbolArray arrAnnoPtr, arrPSyms, arrNotAnnoPtr;
	GetSymbols(pFt,arrPSyms,arrAnnoPtr,arrNotAnnoPtr);

	//打散的地物，不考虑符号对照表
	CString strGeoSymName = pGeoObj->GetSymbolName();
	if( strGeoSymName[0]=='*' || strGeoSymName[0]=='#' || strGeoSymName[0]=='@' )
	{
		arrItems.RemoveAll();
		nValidSubsymbols = 0;
	}

	CString cassCode;
	for(int n=0; n<arrItems.GetSize(); n++)
	{
		if(arrItems[n].index==0 )
		{
			cassCode = arrItems[n].cassCode;
			break;
		}
	}

	//是否导出母线
	BOOL bExportBaseObj = TRUE;
	for(n=0; n<arrItems.GetSize(); n++)
	{
		if(arrItems[n].index==0 && arrItems[n].name[0]=='*' )
		{
			bExportBaseObj = FALSE;
			break;
		}
	}

	float fBaseObjLineWid = -1;
	if(!cassCode.IsEmpty())
	{
		fBaseObjLineWid = 0;
	}

	char *pBaseObjSymName = NULL;
	char baseObjSymName[100] = {0};
	for(n=0; n<arrItems.GetSize(); n++)
	{
		if(arrItems[n].index==0 && arrItems[n].name[0]!='*' )
		{
			strncpy(baseObjSymName,arrItems[n].name,sizeof(baseObjSymName)-1);
			pBaseObjSymName = baseObjSymName;
			break;
		}
	}

	//点
	if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeoObj->GetShape(arrPts);
		
		if(arrPts.GetSize() > 0)
		{
			BOOL bDirPtAsLine = m_dlgDxfExport.m_bDirPtAsLine;
			if( !bDirPtAsLine )
			{
				//如果符号对照表中有母线项（符号编号为0），且符号类型为1（线类型），那么表明该有向点需要导出为线
				for(int i=0; i<arrItems.GetSize(); i++)
				{
					if(arrItems[i].index==0 && arrItems[i].type==1)
					{
						bDirPtAsLine = TRUE;
						break;
					}
				}
			}

			if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoDirPoint))  && bDirPtAsLine && bExportBaseObj )
			{
				double offlen = CGeoDirPoint::GetDefaultDirLen() * GetSymbolDrawScale();
				
				double ang = ((CGeoDirPoint*)pGeoObj)->GetDirection()*PI/180;
				double cosa= cos(ang), sina= sin(ang);
				PT_3DEX pt = arrPts[0], pt1; 
				pt1.x = pt.x+cosa*offlen; pt1.y = pt.y+sina*offlen; pt1.z = pt.z;
				pt1.pencode = penLine;
				
				CGeoCurve curve;
				curve.SetColor(pGeoObj->GetColor());
				
				PT_3DEX pts[2] = {pt,pt1};
				curve.CreateShape(pts,2);
				SaveCurveOrSurface(&curve,strLayerName,NULL,0,-1,cassCode);					
				
			}
			else if(pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)))
			{
				if( m_dlgDxfExport.m_bPoint || ( arrPSyms.GetSize()==0 && nValidSubsymbols==0 )  )
				{
					if(bExportBaseObj)
						SavePoint(pGeoObj,strLayerName,NULL,cassCode);
				}

				if(  m_dlgDxfExport.m_bPointSym && arrPSyms.GetSize()>0 )
				{
					CSymbolArray& arr = arrPSyms;
					for( int i=0; i<arr.GetSize(); i++)
					{
						if(arr[i]->GetType()!=SYMTYPE_CELLHATCH)
							continue;
						SubSymbolItem item;
						item.index = i+1;
						item.type = 0;
						memset(item.name,0,sizeof(item.name));
						strcpy(item.layer,strSymLayerName);
						SavePointSubSymbol(pFt,&item,arr);
						break;
					}
					if(i==arr.GetSize())
					{
						SavePoint(pGeoObj,strLayerName,NULL,cassCode);
					}
				}
			}
			else
			{
				if( m_dlgDxfExport.m_bPoint || ( arrPSyms.GetSize()==0 && nValidSubsymbols==0 )  )
				{
					if(bExportBaseObj)
						SavePoint(pGeoObj,strLayerName,NULL,cassCode);
				}
				
				if( m_dlgDxfExport.m_bExportAsBlock && m_dlgDxfExport.m_bPointSym && arrPSyms.GetSize()>0 )
				{
					if(m_dlgDxfExport.m_bAddAnnot)
					{
						BOOL bSysbolCELL = TRUE;
						for(int a = 0; a < arrNotAnnoPtr.GetSize(); a++)
						{
							int ntype = arrNotAnnoPtr.GetAt(a)->GetType();
							if( 1 != ntype ) bSysbolCELL = FALSE;
						}
						SetBlock(pFt, strSymLayerName, arrNotAnnoPtr,!bSysbolCELL );
					}
					else
					{
						BOOL bSysbolCELL = TRUE;
						for(int a = 0; a < arrPSyms.GetSize(); a++)
						{
							int ntype = arrPSyms.GetAt(a)->GetType();
							if( 1 != ntype ) bSysbolCELL = FALSE;
						}
						SetBlock(pFt, strSymLayerName, arrPSyms, !bSysbolCELL );
					}					
				}
				//如果需要导出子符号，依次生成子符号对象，并导出
				else if( nValidSubsymbols>0 )
				{
					for( int i=0; i<arrItems.GetSize(); i++)
					{
						SavePointSubSymbol(pFt,&arrItems[i],arrPSyms);
					}
					
					KickoffSymbols(pFt,arrPSyms,arrItems);
					ClassifySymbols(arrPSyms,arrAnnoPtr,arrNotAnnoPtr);
					
					if( m_dlgDxfExport.m_bPointSym && arrPSyms.GetSize()>0 )
					{
						if( m_dlgDxfExport.m_bAddAnnot )
							SetBlock(pFt, strSymLayerName, arrNotAnnoPtr, FALSE,NULL,NULL,cassCode);
						else if( arrAnnoPtr.GetSize()>0 )
							SetBlock_exploded(pFt, strSymLayerName, arrPSyms,NULL,NULL );
						else
							SetBlock(pFt, strSymLayerName, arrPSyms,FALSE,cassCode );
					}					
				}
				//导出符号: 带符号地物导出为块
				else if( m_dlgDxfExport.m_bPointSym && arrPSyms.GetSize()>0 )
				{
					KickoffSymbols(pFt,arrPSyms,arrItems);
					ClassifySymbols(arrPSyms,arrAnnoPtr,arrNotAnnoPtr);

					CSymbolArray& arr = (m_dlgDxfExport.m_bAddAnnot?arrNotAnnoPtr:arrPSyms);
					for( int i=0; i<arr.GetSize(); i++)
					{
						SubSymbolItem item;
						item.index = i+1;
						item.type = 0;
						memset(item.name,0,sizeof(item.name));
						strcpy(item.layer,strSymLayerName);
						SavePointSubSymbol(pFt,&item,arr);
					}				
				}
			}
			
		}
	}
	//线
	else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
	{
		if( (m_dlgDxfExport.m_bLines || bDem || (arrPSyms.GetSize()==0 && nValidSubsymbols==0 ) ) && bExportBaseObj )
		{
			if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
			{
				BOOL bExportSepOrCenline = FALSE;
				// 导出中心线，并将宽度写到厚度字段
				if (m_dlgDxfExport.m_bParaCenline && pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
				{
					double fWid = ((CGeoParallel*)pGeoObj)->GetWidth();

					CGeoCurve *pCenCurve = ((CGeoParallel*)pGeoObj)->GetCenterlineCurve();
					if (pCenCurve)
					{
						SaveCurveOrSurface(pCenCurve,strLayerName,NULL,fBaseObjLineWid,fabs(fWid),cassCode);
						delete pCenCurve;
						bExportSepOrCenline = TRUE;
					}
				}

				//如果符号对照表中有母线项（符号编号为0），且符号类型为5（面类型），那么表明该平行线\双线需要导出为面
				BOOL bExportAsSurface = FALSE;
				for(int i=0; i<arrItems.GetSize(); i++)
				{
					if(arrItems[i].index==0 && arrItems[i].type==5)
					{
						bExportAsSurface = TRUE;
						break;
					}
				}

				if(bExportAsSurface)
				{
					CGeometry *pCur1=NULL, *pCur2=NULL;
					if (pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
					{
						((CGeoParallel*)pGeoObj)->Separate(pCur1, pCur2);
					}
					else if (pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
					{
						((CGeoDCurve*)pGeoObj)->Separate(pCur1, pCur2);
					}
					CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
					if (pCur1 )
					{
						pCur1->GetShape(arrPts1);
						delete pCur1;
					}
					if (pCur2 )
					{
						pCur2->GetShape(arrPts2);
						if(arrPts2.GetSize()>0)
							arrPts2[0].pencode = penLine;
						delete pCur2;
					}

					arrPts1.Append(arrPts2);

					arrPts1.Add(arrPts1[0]);

					CGeometry *pSurface = new CGeoSurface();
					pSurface->CopyFrom(pGeoObj);
					pSurface->CreateShape(arrPts1.GetData(),arrPts1.GetSize());

					if( m_dlgDxfExport.m_bLinearize || bDem )
					{
						CGeometry *pSurface2 = (CGeoSurface*)(pSurface)->Linearize();
						SaveCurveOrSurface(pSurface2,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);
						delete pSurface2;
					}
					else
					{
						SaveCurveOrSurface(pSurface,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);
					}

					bExportSepOrCenline = TRUE;

				}
				else if(m_dlgDxfExport.m_bSepPara || bDem )
				{
					CGeometry *pCur1=NULL, *pCur2=NULL;
					if (pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
					{
						((CGeoParallel*)pGeoObj)->Separate(pCur1, pCur2);
					}
					else if (pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
					{
						((CGeoDCurve*)pGeoObj)->Separate(pCur1, pCur2);
					}
					
					if(m_dlgDxfExport.m_bLinearize || bDem)
					{
						if (pCur1 )
						{
							CGeoCurve *plineCurve1 = (CGeoCurve*)(pCur1)->Linearize();
							SaveCurveOrSurface(plineCurve1,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);
							
							if (plineCurve1)
							{
								delete plineCurve1;
								plineCurve1 = NULL;
							}
						}						
						
						if ( pCur2)
						{
							CGeoCurve *plineCurve2 = (CGeoCurve*)(pCur2)->Linearize();
							SaveCurveOrSurface(plineCurve2,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);
							
							if (plineCurve2)
							{
								delete plineCurve2;
								plineCurve2 = NULL;
							}
						}						
					}
					else
					{
						if( pCur1 )SaveCurveOrSurface(pCur1,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);
						if( pCur2 )SaveCurveOrSurface(pCur2,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);
					}
					
					if (pCur1)
					{
						delete pCur1;
						pCur1 = NULL;
					}
					
					if (pCur2)
					{
						delete pCur2;
						pCur2 = NULL;
					}
					
					bExportSepOrCenline = TRUE;
				}
				
				if (!bExportSepOrCenline)
				{
					SaveCurveOrSurface(pGeoObj,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);
				}
			}
			else
			{
				//如果符号对照表中有母线项（符号编号为0），且符号类型为0x10，那么表明该线需要反向
				BOOL bReverse = FALSE;
				for(int i=0; i<arrItems.GetSize(); i++)
				{
					if(arrItems[i].index==0 && (arrItems[i].type&0x10)!=0)
					{
						bReverse = TRUE;
						break;
					}
				}
				
				if(bReverse)pGeoObj = ReverseGeometry(pGeoObj);

				double fWid = ((CGeoCurve*)pGeoObj)->m_fLineWidth*m_pDS->GetSymbolDrawScale();
				if(m_dlgDxfExport.m_bLinearize || bDem )
				{
					CGeoCurve *pCurve = (CGeoCurve*)(pGeoObj)->Linearize();
					SaveCurveOrSurface(pCurve,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);

					if (pCurve)
					{
						delete pCurve;
						pCurve = NULL;
					}
					
				}
				else
					SaveCurveOrSurface(pGeoObj,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);

				if(bReverse)delete pGeoObj;

			}
		}

		if( m_dlgDxfExport.m_bExportAsBlock && m_dlgDxfExport.m_bLineSym && arrPSyms.GetSize()>0 )
		{
			if( TRUE == IsSolidLineSymbol(&arrPSyms) )		//如果是实线
			{
				CSymbolArray& arr = (m_dlgDxfExport.m_bAddAnnot?arrNotAnnoPtr:arrPSyms);
				for( int i=0; i<arr.GetSize(); i++)
				{
					SubSymbolItem item;
					item.index = i+1;
					item.type = 0;
					memset(item.name,0,sizeof(item.name));
					strcpy(item.layer,strSymLayerName);
					SaveCurveSubSymbol(pFt,&item,arr);
				}
			}
			else
			{
				if(m_dlgDxfExport.m_bAddAnnot)
					SetBlock(pFt, strSymLayerName, arrNotAnnoPtr,TRUE );
				else
					SetBlock(pFt, strSymLayerName, arrPSyms,TRUE );
			}
		}
		//带符号地物导出
		//如果需要导出子符号，依次生成子符号对象，并导出
		else if( nValidSubsymbols>0 )
		{
			for( int i=0; i<arrItems.GetSize(); i++)
			{
				SaveCurveSubSymbol(pFt,&arrItems[i],arrPSyms);
			}
			
			KickoffSymbols(pFt,arrPSyms,arrItems);
			ClassifySymbols(arrPSyms,arrAnnoPtr,arrNotAnnoPtr);
			
			if( m_dlgDxfExport.m_bLineSym && arrPSyms.GetSize()>0 )
			{
				CSymbolArray& arr = (m_dlgDxfExport.m_bAddAnnot?arrNotAnnoPtr:arrPSyms);
				for( int i=0; i<arr.GetSize(); i++)
				{
					SubSymbolItem item;
					item.index = i+1;
					item.type = 1;
					memset(item.name,0,sizeof(item.name));
					strcpy(item.layer,strSymLayerName);
					SaveCurveSubSymbol(pFt,&item,arr);
				}
			}
		}
		else if(m_dlgDxfExport.m_bLineSym && arrPSyms.GetSize()>0 )
		{			
			KickoffSymbols(pFt,arrPSyms,arrItems);
			ClassifySymbols(arrPSyms,arrAnnoPtr,arrNotAnnoPtr);

			CSymbolArray& arr = (m_dlgDxfExport.m_bAddAnnot?arrNotAnnoPtr:arrPSyms);
			for( int i=0; i<arr.GetSize(); i++)
			{
				SubSymbolItem item;
				item.index = i+1;
				item.type = 0;
				memset(item.name,0,sizeof(item.name));
				strcpy(item.layer,strSymLayerName);
				SaveCurveSubSymbol(pFt,&item,arr);
			}
		}

	}
	else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) )
	{
		if( (m_dlgDxfExport.m_bSurface || bDem || (arrPSyms.GetSize()==0 && nValidSubsymbols==0) ) && bExportBaseObj )
		{
			CGeoSurface *pSur = new CGeoSurface;
			if(!pSur) return FALSE;
			CArray<PT_3DEX,PT_3DEX> arrPts;
			if(m_dlgDxfExport.m_bLinearize || bDem )
			{
				CGeoMultiSurface *pSurface = (CGeoMultiSurface*)(pGeoObj)->Linearize();
				if(!pSurface)
				{
					delete pSur;
					return FALSE;
				}
				for (int i=pSurface->GetSurfaceNum()-1;i>=0;i--)
				{
					arrPts.RemoveAll();
					pSurface->GetSurface(i,arrPts);
					if(pSur->CreateShape(arrPts.GetData(),arrPts.GetSize()))
						SaveCurveOrSurface(pSur,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);
				}
				delete pSurface;			
			}
			else
			{
				CGeoMultiSurface *pSurface = (CGeoMultiSurface*)pGeoObj;
				for (int i=pSurface->GetSurfaceNum()-1;i>=0;i--)
				{
					arrPts.RemoveAll();
					pSurface->GetSurface(i,arrPts);
					if(pSur->CreateShape(arrPts.GetData(),arrPts.GetSize()))
						SaveCurveOrSurface(pSur,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);
				}			
			}
			delete pSur;
		}

		if( m_dlgDxfExport.m_bExportAsBlock && m_dlgDxfExport.m_bSurfaceSym && arrPSyms.GetSize() > 0 )
		{
			if( TRUE == IsSolidLineSymbol(&arrPSyms) )		//如果是实线
			{
				CSymbolArray& arr = (m_dlgDxfExport.m_bAddAnnot?arrNotAnnoPtr:arrPSyms);
				for( int i=0; i<arr.GetSize(); i++)
				{
					SubSymbolItem item;
					item.index = i+1;
					item.type = 0;
					memset(item.name,0,sizeof(item.name));
					strcpy(item.layer,strSymLayerName);
					SaveCurveSubSymbol(pFt,&item,arrPSyms);
				}
			}
			else
			{
				if(m_dlgDxfExport.m_bAddAnnot)
					SetBlock(pFt, strSymLayerName, arrNotAnnoPtr,TRUE );
				else
					SetBlock(pFt, strSymLayerName, arrPSyms,TRUE );
			}
		}		
		//如果需要导出子符号，依次生成子符号对象，并导出
		else if( nValidSubsymbols>0 )
		{
			for( int i=0; i<arrItems.GetSize(); i++)
			{
				SaveCurveSubSymbol(pFt,&arrItems[i],arrPSyms);
			}
			
			KickoffSymbols(pFt,arrPSyms,arrItems);
			ClassifySymbols(arrPSyms,arrAnnoPtr,arrNotAnnoPtr);
			
			if( m_dlgDxfExport.m_bSurfaceSym && arrPSyms.GetSize()>0 )
			{
				if( m_dlgDxfExport.m_bAddAnnot )
					SetBlock_exploded(pFt, strSymLayerName, arrNotAnnoPtr);
				else
					SetBlock_exploded(pFt, strSymLayerName, arrPSyms);
			}
		}
		else if(m_dlgDxfExport.m_bSurfaceSym )
		{
			KickoffSymbols(pFt,arrPSyms,arrItems);
			ClassifySymbols(arrPSyms,arrAnnoPtr,arrNotAnnoPtr);

			CSymbolArray& arr = (m_dlgDxfExport.m_bAddAnnot?arrNotAnnoPtr:arrPSyms);
			for( int i=0; i<arr.GetSize(); i++)
			{
				SubSymbolItem item;
				item.index = i+1;
				item.type = 0;
				memset(item.name,0,sizeof(item.name));
				strcpy(item.layer,strSymLayerName);
				SaveCurveSubSymbol(pFt,&item,arrPSyms);
			}
		}
	}
	//面
	else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		if( (m_dlgDxfExport.m_bSurface || bDem || (arrPSyms.GetSize()==0 && nValidSubsymbols==0) ) && bExportBaseObj )
		{
			//如果符号对照表中有母线项（符号编号为0），且符号类型为0x10，那么表明该线需要反向
			BOOL bReverse = FALSE;
			for(int i=0; i<arrItems.GetSize(); i++)
			{
				if(arrItems[i].index==0 && (arrItems[i].type&0x10)!=0)
				{
					bReverse = TRUE;
					break;
				}
			}
			
			if(bReverse)pGeoObj = ReverseGeometry(pGeoObj);

			if(m_dlgDxfExport.m_bLinearize || bDem )
			{
				CGeoSurface *pSurface = (CGeoSurface*)(pGeoObj)->Linearize();
				SaveCurveOrSurface(pSurface,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);

				if (pSurface)
				{
					delete pSurface;
					pSurface = NULL;
				}
				
			}
			else
				SaveCurveOrSurface(pGeoObj,strLayerName,pBaseObjSymName,fBaseObjLineWid,-1,cassCode);

			if(bReverse)delete pGeoObj;

		}

		if( m_dlgDxfExport.m_bExportAsBlock && m_dlgDxfExport.m_bSurfaceSym && arrPSyms.GetSize()>0 )
		{
			if( TRUE == IsSolidLineSymbol(&arrPSyms) )		//如果是实线
			{
				CSymbolArray& arr = (m_dlgDxfExport.m_bAddAnnot?arrNotAnnoPtr:arrPSyms);
				
				for( int i=0; i<arr.GetSize(); i++)
				{
					SubSymbolItem item;
					item.index = i+1;
					item.type = 0;
					memset(item.name,0,sizeof(item.name));
					strcpy(item.layer,strLayerName);
					SaveCurveSubSymbol(pFt,&item,arr);
				}
			}
			else
			{
				if(m_dlgDxfExport.m_bAddAnnot)
					SetBlock(pFt, strSymLayerName, arrNotAnnoPtr,TRUE );
				else
					SetBlock(pFt, strSymLayerName, arrPSyms,TRUE );
			}
		}
		//如果需要导出子符号，依次生成子符号对象，并导出
		else if( nValidSubsymbols>0 )
		{
			for( int i=0; i<arrItems.GetSize(); i++)
			{
				SaveCurveSubSymbol(pFt,&arrItems[i],arrPSyms);
			}
			
			KickoffSymbols(pFt,arrPSyms,arrItems);
			ClassifySymbols(arrPSyms,arrAnnoPtr,arrNotAnnoPtr);
			
			if( m_dlgDxfExport.m_bSurfaceSym && arrPSyms.GetSize()>0 )
			{
				if( m_dlgDxfExport.m_bAddAnnot )
					SetBlock_exploded(pFt, strSymLayerName, arrNotAnnoPtr);
				else
					SetBlock_exploded(pFt, strSymLayerName, arrPSyms);
			}
		}
		else if(m_dlgDxfExport.m_bSurfaceSym && arrPSyms.GetSize()>0 )
		{
			KickoffSymbols(pFt,arrPSyms,arrItems);
			ClassifySymbols(arrPSyms,arrAnnoPtr,arrNotAnnoPtr);

			CSymbolArray& arr = (m_dlgDxfExport.m_bAddAnnot?arrNotAnnoPtr:arrPSyms);
			
			for( int i=0; i<arr.GetSize(); i++)
			{
				SubSymbolItem item;
				item.index = i+1;
				item.type = 0;
				memset(item.name,0,sizeof(item.name));
				strcpy(item.layer,strLayerName);
				SaveCurveSubSymbol(pFt,&item,arr);
			}
		}
	}
	//文本
	else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
	{
		if( ( m_dlgDxfExport.m_bText || arrPSyms.GetSize()==0 ) && bExportBaseObj )
		{
			CGeoText *pText = (CGeoText*)pGeoObj;
		
			SaveText(pText,strLayerName,cassCode);
		}
		
		if( m_dlgDxfExport.m_bTextSym && arrPSyms.GetSize()>0 )
		{
			SetBlock_exploded( pFt, strSymLayerName, arrPSyms);
		}
	}
	// Lidar 点
	else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
	{
		//导出母线
		//if( m_dlgDxfExport.m_bPoint )
		{
			m_DxfDraw.SetLayer(strLayerName);

			// Color ByLayer
			if (pGeoObj->GetColor() == FTRCOLOR_BYLAYER)
			{
				m_DxfDraw.SetColor(256);
			}
			else
			{
				short col = ConverttoIndexOfCAD(pGeoObj->GetColor());
				if (col == 0) col = 255;
				m_DxfDraw.SetColor(col);
			}
			
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pGeoObj->GetShape(arrPts);
			for( int t=0; t<arrPts.GetSize(); t++)
			{
				CGeoPoint obj;
				obj.SetColor(pGeoObj->GetColor());
				obj.CreateShape(&arrPts[t],1);
				SavePoint(&obj,strLayerName,NULL,cassCode);
			}
		}
	}
	else
		bExportObj = FALSE;

	if(m_dlgDxfExport.m_bAddAnnot && bExportObj && arrAnnoPtr.GetSize()>0 )
	{
		GrBuffer buf;
		for(int i=0; i<arrAnnoPtr.GetSize(); i++)
		{
			CSymbol *pSymbol = arrAnnoPtr[i];

			if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			{
				SubSymbolItem item;
				item.index = i+1;
				item.type = 0;
				memset(item.name,0,sizeof(item.name));
				strcpy(item.layer,strSymLayerName);
				SavePointSubSymbol(pFt,&item,arrAnnoPtr);
			}
			else if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			{
				SubSymbolItem item;
				item.index = i+1;
				item.type = 0;
				memset(item.name,0,sizeof(item.name));
				strcpy(item.layer,strSymLayerName);
				SaveCurveSubSymbol(pFt,&item,arrAnnoPtr);
			}
			
		}

	}	
	
	return TRUE;

}


CString CDxfWrite::GetExportedBlockName(CFtrLayer *pLayer, long color_ftr)
{
	CString name = pLayer->GetName();

	char code[256] = {0};
	strcpy(code,name);
	
	if( m_dlgDxfExport.m_bUseIndex || m_dlgDxfExport.m_bUseLayerGroup )
	{
		__int64 nCode = 0;
		LPCTSTR name2 = code;
		if( m_pDS->FindLayerIdx(FALSE,nCode,name2) )
		{
			sprintf(code,"%I64d", nCode);
		}
	}

	name = m_lstFile.FindMatchItem2(code,0,"0",2,3);
	if( name.IsEmpty() )
	{
		name = pLayer->GetName();

		CString strNewBlockName;
		strNewBlockName.Format("blk_%s",(LPCTSTR)name);

		name = strNewBlockName;
	}

	NormalizeName(name);

	if( color_ftr!=FTRCOLOR_BYLAYER )
	{
		color_ftr = ConverttoIndexOfCAD(color_ftr);
		CString strColor;
		strColor.Format("%02x",(int)color_ftr);
		name += strColor;
	}

	return name;
}

void CDxfWrite::SetBlock(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, BOOL bNewBlock, LPCTSTR blkName, CFtrLayer *pLayer,LPCTSTR cassCode)
{
	char name2[300];

	if( blkName!=NULL )
	{
		strcpy(name2,blkName);
		NormalizeName(name2);
		blkName = name2;
	}

	CLinearizerPrecisionChange change(0.001);

	if (m_bExportDWG)
	{
		SetBlock_dwg(pFtr,dxfLayerName,arrPSyms,bNewBlock,blkName,pLayer,cassCode);
		return;
	}
		
	if( pLayer==NULL )
	{
		pLayer = m_pDS->GetFtrLayerOfObject(pFtr);
		if( pLayer==NULL )
			pLayer = m_pCurFdbLayer;
	}

	CGeometry *pObj = pFtr->GetGeometry();

	PBLOCKHEADER ph = NULL;
	OBJHANDLE hBlock = NULL;

	long color_ftr = pObj->GetColor();
	long color = ConvertColor(color_ftr,pLayer);

	//块头
	BLOCKHEADER blkHead;
	memset(&blkHead,0,sizeof(BLOCKHEADER));
	blkHead.LayerObjhandle = m_hDefLayer;
	
	ph = &blkHead;

	CString strNewBlockName;
	
	if( !bNewBlock )
	{		
		if( blkName==NULL && pLayer )
			strNewBlockName = GetExportedBlockName(pLayer,pObj->GetColor());
		else
			strNewBlockName = blkName;	

		memset(ph->Name,0,sizeof(ph->Name));
		strncpy(ph->Name,strNewBlockName,sizeof(ph->Name)-1);
		TruncateStringHead(ph->Name,32);

		hBlock = m_DxfDraw.FindBlock(FIND_BYNAME,ph);
	}
	else
	{
		//防止多文件合并时，块冲突的问题
		strNewBlockName.Format("%sb%d",(LPCTSTR)m_docName,m_nBlockNo++);

		memset(ph->Name,0,sizeof(ph->Name));
		strncpy(ph->Name,strNewBlockName,sizeof(ph->Name)-1);
		TruncateStringHead(ph->Name,32);
	}

	if( hBlock!=NULL )
	{
		double ang = 0;
		float kx = 1, ky = 1;

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{
			CGeoPoint *pPoint = (CGeoPoint*)pObj;
			kx = pPoint->m_fKx;
			ky = pPoint->m_fKy;

			ang = pPoint->m_lfAngle;
		}
			
		PT_3DEX base;
		base = pObj->GetDataPoint(0);

		m_DxfDraw.SetActiveBlock((OBJHANDLE)0);
		m_DxfDraw.SetLayer(dxfLayerName);
		m_DxfDraw.InsertBlock(ph->Name,base.x,base.y,base.z,kx,ky,ang);

		return;
	}

	NormalizeName(ph->Name);

	//添加新块
	//if( arrPSyms.GetSize()<=0 )
	//	return;

	{
		double ang = 0;
		CGeoPoint save_obj;
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{
			CGeoPoint *pPoint = (CGeoPoint*)pObj;
			save_obj.CopyFrom(pObj);

			ang = pPoint->m_lfAngle;
			pPoint->m_lfAngle = 0;
			pPoint->m_fKx = 1;
			pPoint->m_fKy = 1;
			pPoint->m_nCoverType = 0;
			pPoint->m_fExtendDis = 0;
		}

		float scale = m_pDS->GetSymbolDrawScale();
		GrBuffer buf,buf1;
		for(int i=0; i<arrPSyms.GetSize(); i++)
		{
			CSymbol *pSymbol = arrPSyms[i];
			
			if (pSymbol->GetType() == SYMTYPE_ANNOTATION)
			{
				CValueTable tab;
				m_pDS->GetAllAttribute(pFtr,tab);
				((CAnnotation*)pSymbol)->Draw(pFtr,&buf1,tab,scale,0,1);
			}
			else
				pSymbol->Draw(pFtr,&buf1,scale);
		}

		buf1.GetAllGraph(&buf);
		if(buf.HeadGraph()==NULL)
		{
			return;
		}

		PT_3DEX base;
		Envelope enp = buf.GetEnvelope();
		base = pObj->GetDataPoint(0);
		
		buf.Move(-base.x,-base.y,-base.z);
		
		Graph *pGr = buf.HeadGraph();

		m_DxfDraw.SetLayer(_T("0"));
		hBlock = m_DxfDraw.AddBlock(ph);
		m_DxfDraw.SetActiveBlock(hBlock);
		
		while(pGr)
		{
			if(pGr->type == GRAPH_TYPE_POINT)
			{
				GrPoint *pP = (GrPoint*)pGr;
				if( pP->bGrdSize )
				{
					if( pP->bFlat )
					{
						ENTVERTEX pts[4];
						memset(pts,0,sizeof(pts));
						pts[0].Point.x = pP->pt.x-pP->kx*0.5;  pts[0].Point.y = pP->pt.y-pP->ky*0.5;  pts[0].Point.z = pP->pt.z;
						pts[1].Point.x = pP->pt.x+pP->kx*0.5;  pts[1].Point.y = pP->pt.y-pP->ky*0.5;  pts[1].Point.z = pP->pt.z;
						pts[2].Point.x = pP->pt.x+pP->kx*0.5;  pts[2].Point.y = pP->pt.y+pP->ky*0.5;  pts[2].Point.z = pP->pt.z;
						pts[3].Point.x = pP->pt.x-pP->kx*0.5;  pts[3].Point.y = pP->pt.y+pP->ky*0.5;  pts[3].Point.z = pP->pt.z;	
						
// 						short col = ConverttoIndexOfCAD(pObj->GetColor()==FTRCOLOR_BYLAYER?pLayer->GetColor():pObj->GetColor());
						if (pObj->GetColor() == FTRCOLOR_BYLAYER)
						{
							m_DxfDraw.SetColor(256);
						}
						else
						{
							short col = ConverttoIndexOfCAD(pObj->GetColor());
							if (col == 0) col = 255;
							m_DxfDraw.SetColor(col);
						}
						m_DxfDraw.PolyLine(pts,4,1);
					}
					else
					{
						PT_3D pt;
						pt.x = pP->pt.x;
						pt.y = pP->pt.y;
						pt.z = pP->pt.z;
						if (pObj->GetColor() == FTRCOLOR_BYLAYER)
						{
							m_DxfDraw.SetColor(256);
						}
						else
						{
							short col = ConverttoIndexOfCAD(pObj->GetColor());
							if (col == 0) col = 255;
							m_DxfDraw.SetColor(col);
						}
						m_DxfDraw.Circle(pt.x,pt.y,pt.z,pP->kx*0.5);
					}
					
				}
				else
				{
// 					short col = ConverttoIndexOfCAD(pObj->GetColor()==FTRCOLOR_BYLAYER?pLayer->GetColor():pObj->GetColor());
					if (pObj->GetColor() == FTRCOLOR_BYLAYER)
					{
						m_DxfDraw.SetColor(256);
					}
					else
					{
						short col = ConverttoIndexOfCAD(pObj->GetColor());
						if (col == 0) col = 255;
						m_DxfDraw.SetColor(col);
					}
					m_DxfDraw.Point(pP->pt.x,pP->pt.y,pP->pt.z,0);
				}
				
			}
			else if(pGr->type == GRAPH_TYPE_POINTSTRING )
			{
				GrPointString *p = (GrPointString*)pGr;
// 				short col = ConverttoIndexOfCAD(pObj->GetColor()==FTRCOLOR_BYLAYER?pLayer->GetColor():pObj->GetColor());
				if (pObj->GetColor() == FTRCOLOR_BYLAYER)
				{
					m_DxfDraw.SetColor(256);
				}
				else
				{
					short col = ConverttoIndexOfCAD(pObj->GetColor());
				if (col == 0) col = 255;
					m_DxfDraw.SetColor(col);
				}
				for( i=0; i<p->ptlist.nuse; i++)
				{
					m_DxfDraw.Point(p->ptlist.pts[i].x,p->ptlist.pts[i].y,p->ptlist.pts[i].z,0);
				}
			}
			else if(pGr->type == GRAPH_TYPE_LINESTRING)
			{
				GrLineString *pLine = (GrLineString*)pGr;
				GrVertexList ptlist = pLine->ptlist;
				GrVertex *ptNodes = ptlist.pts;
				int ptnum = ptlist.nuse;
				
				ENTVERTEX *pv = new ENTVERTEX[ptnum];
				memset(pv,0,sizeof(ENTVERTEX)*ptnum);

				float wid = 0;
				if( pLine->bGrdWid )
					wid = pLine->width;
				
				int pos = 0;
				for( int i=0; i<ptlist.nuse; i++)
				{
					//去除相同点
					//如果和以前的两断线是紧密衔接的，这样可以继续读取断线，以便于将紧致的断线衔接起来
					if( i>0 && i!=(ptlist.nuse-1) &&
						fabs(ptNodes[i].x-ptNodes[i-1].x)<1e-4 && 
						fabs(ptNodes[i].y-ptNodes[i-1].y)<1e-4 &&
						fabs(ptNodes[i].z-ptNodes[i-1].z)<1e-4 )
					{
						continue;
					}
					
					pv[pos].Point.x = ptNodes[i].x;
					pv[pos].Point.y = ptNodes[i].y;
					pv[pos].Point.z = ptNodes[i].z;
					pv[pos].StartWidth = wid;
					pv[pos].EndWidth = wid;
					
					//当开始另外一段线或者要结束时，就导出已经积累的线串
					if( i>0 && ( (ptNodes[i].code&1)==GRBUFFER_PTCODE_MOVETO || i==(ptnum-1) ) )
					{
						//如果要结束，并且当前点是LINETO模式，导出点中应该包含当前点，
						//而且宽度应该更新一下（避免起点宽度为无效的值）
						if( pos>=1 && (ptNodes[i].code&1)==GRBUFFER_PTCODE_LINETO )
						{
							pv[0].StartWidth = wid;
							pv[0].EndWidth = wid;
							
							if (pObj->GetColor() == FTRCOLOR_BYLAYER)
							{
								m_DxfDraw.SetColor(256);
							}
							else
							{
								short col = ConverttoIndexOfCAD(pObj->GetColor());
								if (col == 0) col = 255;
								m_DxfDraw.SetColor(col);
							}
							m_DxfDraw.PolyLine(pv,pos+1);
						}
						else if( pos>=2 )
						{
							if (pObj->GetColor() == FTRCOLOR_BYLAYER)
							{
								m_DxfDraw.SetColor(256);
							}
							else
							{
								short col = ConverttoIndexOfCAD(pObj->GetColor());
								if (col == 0) col = 255;
								m_DxfDraw.SetColor(col);
							}
							m_DxfDraw.PolyLine(pv,pos);
						}
						pos = 0;
						
						pv[pos].Point.x = ptNodes[i].x;
						pv[pos].Point.y = ptNodes[i].y;
						pv[pos].Point.z = ptNodes[i].z;
						pv[pos].StartWidth = wid;
						pv[pos].EndWidth = wid;	
					}
					//更新宽度，避免起点宽度为无效的值
					else
					{
						pv[0].StartWidth = wid;
						pv[0].EndWidth = wid;
					}
					
					pos++;
				}

				delete pv;
			}
			else if(pGr->type == GRAPH_TYPE_POLYGON)
			{
				GrPolygon *pPolygon = (GrPolygon*)pGr;
				GrVertexList ptlist = pPolygon->ptlist;
				GrVertex *ptNodes = ptlist.pts;
				int ptnum = ptlist.nuse;
				
				ENTVERTEX *pv = new ENTVERTEX[ptnum];
				memset(pv,0,sizeof(ENTVERTEX)*ptnum);
				
				int pos = 0;
				for( int i=0; i<ptnum; i++)
				{
					//去除相同点
					//如果和以前的两断线是紧密衔接的，这样可以继续读取断线，以便于将紧致的断线衔接起来
					if( i>0 && i!=(ptnum-1) &&
						fabs(ptNodes[i].x-ptNodes[i-1].x)<1e-4 && 
						fabs(ptNodes[i].y-ptNodes[i-1].y)<1e-4 &&
						fabs(ptNodes[i].z-ptNodes[i-1].z)<1e-4 )
					{
						continue;
					}
					
					pv[pos].Point.x = ptNodes[i].x;
					pv[pos].Point.y = ptNodes[i].y;
					pv[pos].Point.z = ptNodes[i].z;
					pv[pos].StartWidth = 0;
					pv[pos].EndWidth = 0;
					
					//当开始另外一段线或者要结束时，就导出已经积累的线串
					if( i>0 && ( (ptNodes[i].code&1)==GRBUFFER_PTCODE_MOVETO || i==(ptnum-1) ) )
					{
						//如果要结束，并且当前点是LINETO模式，导出点中应该包含当前点，
						//而且宽度应该更新一下（避免起点宽度为无效的值）
						if( pos>=1 && (ptNodes[i].code&1)==GRBUFFER_PTCODE_LINETO )
						{
							pv[0].StartWidth = 0;
							pv[0].EndWidth = 0;
							
// 							short col = ConverttoIndexOfCAD(pObj->GetColor()==FTRCOLOR_BYLAYER?pLayer->GetColor():pObj->GetColor());
							if (pObj->GetColor() == FTRCOLOR_BYLAYER)
							{
								m_DxfDraw.SetColor(256);
							}
							else
							{
								short col = ConverttoIndexOfCAD(pObj->GetColor());
							if (col == 0) col = 255;
							m_DxfDraw.SetColor(col);
							}
							m_DxfDraw.PolyLine(pv,pos+1,1);
						}
						else if( pos>=2 )
						{
// 							short col = ConverttoIndexOfCAD(pObj->GetColor()==FTRCOLOR_BYLAYER?pLayer->GetColor():pObj->GetColor());
							if (pObj->GetColor() == FTRCOLOR_BYLAYER)
							{
								m_DxfDraw.SetColor(256);
							}
							else
							{
								short col = ConverttoIndexOfCAD(pObj->GetColor());
							if (col == 0) col = 255;
							m_DxfDraw.SetColor(col);
							}
							m_DxfDraw.PolyLine(pv,pos,1);
						}
						pos = 0;
						
						pv[pos].Point.x = ptNodes[i].x;
						pv[pos].Point.y = ptNodes[i].y;
						pv[pos].Point.z = ptNodes[i].z;
						pv[pos].StartWidth = 0;
						pv[pos].EndWidth = 0;	
					}
					//更新宽度，避免起点宽度为无效的值
					else
					{
						pv[0].StartWidth = 0;
						pv[0].EndWidth = 0;
					}
					
					pos++;
				}

				delete pv;
			}
			else if(pGr->type == GRAPH_TYPE_TEXT)
			{
				GrText *pText = (GrText*)pGr;
// 				short col = ConverttoIndexOfCAD(pObj->GetColor()==FTRCOLOR_BYLAYER?pLayer->GetColor():pObj->GetColor());
				if (pObj->GetColor() == FTRCOLOR_BYLAYER)
				{
					m_DxfDraw.SetColor(256);
				}
				else
				{
					short col = ConverttoIndexOfCAD(pObj->GetColor());
				if (col == 0) col = 255;
				m_DxfDraw.SetColor(col);
				}
				// 过滤换行符
				CString strText = pText->text;
				RemoveChar(strText,"\r\n\t,，");

				int nJustification = ConvertAlignmentAndJustification(pText->settings.nAlignment,TRUE);
				m_DxfDraw.Text(strText,pText->pt.x,pText->pt.y,pText->pt.x,pText->pt.y,pText->pt.z,pText->settings.fHeight*0.6/**GetSymbolDrawScale()*/,nJustification,
					0,1,0);
			}
			
			pGr = pGr->next;
			
		}
		
		m_DxfDraw.SetActiveBlock((OBJHANDLE)0);
		m_DxfDraw.SetLayer(dxfLayerName);

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{
			m_DxfDraw.InsertBlock(ph->Name,base.x,base.y,base.z,save_obj.m_fKx,save_obj.m_fKy,ang);
		}
		else		
			m_DxfDraw.InsertBlock(ph->Name,base.x,base.y,base.z);	
		
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{
			CGeoPoint *pPoint = (CGeoPoint*)pObj;

			pPoint->CopyFrom(&save_obj);
		}
		
	}

	return;
}


void CDxfWrite::SetBlock_exploded(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, CFtrLayer *pLayer,LPCTSTR cassCode)
{
	if (0 && m_bExportDWG)
	{
		SetBlock_exploded_dwg(pFtr,dxfLayerName,arrPSyms,pLayer,cassCode);
		return;
	}
		
	if( pLayer==NULL )
		pLayer = m_pDS->GetFtrLayerOfObject(pFtr);

	CGeometry *pObj = pFtr->GetGeometry();

	long color_ftr = pObj->GetColor();
	long color = ConvertColor(color_ftr,pLayer);
	
	{
		float scale = m_pDS->GetSymbolDrawScale();

		GrBuffer buf,buf1;
		for(int i=0; i<arrPSyms.GetSize(); i++)
		{
			CSymbol *pSymbol = arrPSyms[i];
			
			if (pSymbol->GetType() == SYMTYPE_ANNOTATION)
			{
				CValueTable tab;
				m_pDS->GetAllAttribute(pFtr,tab);
				((CAnnotation*)pSymbol)->Draw(pFtr,&buf1,tab,scale,0,1);
			}
			else
				pSymbol->Draw(pFtr,&buf1,scale);
		}
		buf1.GetAllGraph(&buf);

		if(arrPSyms.GetSize()==0)
		{
			pFtr->Draw(&buf);
		}

		{
			CGeoArray arr;
			ConvertGrBufferToGeos(&buf,scale,arr);
			for( int i=0; i<arr.GetSize(); i++)
			{
				CGeometry *pGeo = arr[i];
				pGeo->SetColor(color_ftr);
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
				{
					SavePoint(pGeo,dxfLayerName,NULL,cassCode);
				}
				else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				{
					SaveCurveOrSurface(pGeo,dxfLayerName,NULL,-1,-1,cassCode);
				}
				else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
				{
					SaveText((CGeoText*)pGeo,dxfLayerName,cassCode);
				}
				delete pGeo;
			}
			return;
		}		

		Graph *pGr = buf.HeadGraph();

		m_DxfDraw.SetLayer(dxfLayerName);
		
		while(pGr)
		{
			if(pGr->type == GRAPH_TYPE_POINT)
			{
				GrPoint *pP = (GrPoint*)pGr;
				if( pP->bGrdSize )
				{
					if( pP->bFlat )
					{
						ENTVERTEX pts[4];
						memset(pts,0,sizeof(pts));
						pts[0].Point.x = pP->pt.x-pP->kx*0.5;  pts[0].Point.y = pP->pt.y-pP->ky*0.5;  pts[0].Point.z = pP->pt.z;
						pts[1].Point.x = pP->pt.x+pP->kx*0.5;  pts[1].Point.y = pP->pt.y-pP->ky*0.5;  pts[1].Point.z = pP->pt.z;
						pts[2].Point.x = pP->pt.x+pP->kx*0.5;  pts[2].Point.y = pP->pt.y+pP->ky*0.5;  pts[2].Point.z = pP->pt.z;
						pts[3].Point.x = pP->pt.x-pP->kx*0.5;  pts[3].Point.y = pP->pt.y+pP->ky*0.5;  pts[3].Point.z = pP->pt.z;	
						if (color_ftr == FTRCOLOR_BYLAYER)
						{
							m_DxfDraw.SetColor(256);
						}
						else
						{
							if (color == 0)
							{
								color = 255;
							}
							m_DxfDraw.SetColor(color);
						}
						m_DxfDraw.PolyLine(pts,4,1);
					}
					else
					{
						PT_3D pt;
						pt.x = pP->pt.x;
						pt.y = pP->pt.y;
						pt.z = pP->pt.z;

						if (color_ftr == FTRCOLOR_BYLAYER)
						{
							m_DxfDraw.SetColor(256);
						}
						else
						{
							if (color == 0)
							{
								color = 255;
							}
						m_DxfDraw.SetColor(color);
						}
						m_DxfDraw.Circle(pt.x,pt.y,pt.z,pP->kx*0.5);
					}
					
				}
				else
				{
					if (color_ftr == FTRCOLOR_BYLAYER)
					{
						m_DxfDraw.SetColor(256);
					}
					else
					{
						if (color == 0)
						{
							color = 255;
						}
					m_DxfDraw.SetColor(color);
					}
					m_DxfDraw.Point(pP->pt.x,pP->pt.y,pP->pt.z,0);
				}
				
			}
			else if(pGr->type == GRAPH_TYPE_POINTSTRING )
			{
				GrPointString *p = (GrPointString*)pGr;
				if (color_ftr == FTRCOLOR_BYLAYER)
				{
					m_DxfDraw.SetColor(256);
				}
				else
				{
					if (color == 0)
					{
						color = 255;
					}
				m_DxfDraw.SetColor(color);
				}

				for( i=0; i<p->ptlist.nuse; i++)
				{
					m_DxfDraw.Point(p->ptlist.pts[i].x,p->ptlist.pts[i].y,p->ptlist.pts[i].z,0);
				}
			}
			else if(pGr->type == GRAPH_TYPE_LINESTRING)
			{
				GrLineString *pLine = (GrLineString*)pGr;
				GrVertexList ptlist = pLine->ptlist;
				GrVertex *ptNodes = ptlist.pts;
				int ptnum = ptlist.nuse;
				
				ENTVERTEX *pv = new ENTVERTEX[ptnum];
				memset(pv,0,sizeof(ENTVERTEX)*ptnum);

				float wid = 0;
				if( pLine->bGrdWid )
					wid = pLine->width;
				
				int pos = 0;
				for( int i=0; i<ptlist.nuse; i++)
				{
					//去除相同点
					//如果和以前的两断线是紧密衔接的，这样可以继续读取断线，以便于将紧致的断线衔接起来
					if( i>0 && i!=(ptlist.nuse-1) &&
						fabs(ptNodes[i].x-ptNodes[i-1].x)<1e-4 && 
						fabs(ptNodes[i].y-ptNodes[i-1].y)<1e-4 &&
						fabs(ptNodes[i].z-ptNodes[i-1].z)<1e-4 )
					{
						continue;
					}
					
					pv[pos].Point.x = ptNodes[i].x;
					pv[pos].Point.y = ptNodes[i].y;
					pv[pos].Point.z = ptNodes[i].z;
					pv[pos].StartWidth = wid;
					pv[pos].EndWidth = wid;
					
					//当开始另外一段线或者要结束时，就导出已经积累的线串
					if( i>0 && ( (ptNodes[i].code&1)==GRBUFFER_PTCODE_MOVETO || i==(ptnum-1) ) )
					{
						//如果要结束，并且当前点是LINETO模式，导出点中应该包含当前点，
						//而且宽度应该更新一下（避免起点宽度为无效的值）
						if( pos>=1 && (ptNodes[i].code&1)==GRBUFFER_PTCODE_LINETO )
						{
							pv[0].StartWidth = wid;
							pv[0].EndWidth = wid;
							
							if (color_ftr == FTRCOLOR_BYLAYER)
							{
								m_DxfDraw.SetColor(256);
							}
							else
							{
								if (color == 0)
								{
									color = 255;
								}
							m_DxfDraw.SetColor(color);
							}
							m_DxfDraw.PolyLine(pv,pos+1);
						}
						else if( pos>=2 )
						{
							if (color_ftr == FTRCOLOR_BYLAYER)
							{
								m_DxfDraw.SetColor(256);
							}
							else
							{
								if (color == 0)
								{
									color = 255;
								}
							m_DxfDraw.SetColor(color);
							}
							m_DxfDraw.PolyLine(pv,pos);
						}
						pos = 0;
						
						pv[pos].Point.x = ptNodes[i].x;
						pv[pos].Point.y = ptNodes[i].y;
						pv[pos].Point.z = ptNodes[i].z;
						pv[pos].StartWidth = wid;
						pv[pos].EndWidth = wid;	
					}
					//更新宽度，避免起点宽度为无效的值
					else
					{
						pv[0].StartWidth = wid;
						pv[0].EndWidth = wid;
					}
					
					pos++;
				}

				delete pv;
			}
			else if(pGr->type == GRAPH_TYPE_POLYGON)
			{
				GrPolygon *pPolygon = (GrPolygon*)pGr;
				GrVertexList ptlist = pPolygon->ptlist;
				GrVertex *ptNodes = ptlist.pts;
				int ptnum = ptlist.nuse;
				
				ENTVERTEX *pv = new ENTVERTEX[ptnum];
				memset(pv,0,sizeof(ENTVERTEX)*ptnum);
				
				int pos = 0;
				for( int i=0; i<ptnum; i++)
				{
					//去除相同点
					//如果和以前的两断线是紧密衔接的，这样可以继续读取断线，以便于将紧致的断线衔接起来
					if( i>0 && i!=(ptnum-1) &&
						fabs(ptNodes[i].x-ptNodes[i-1].x)<1e-4 && 
						fabs(ptNodes[i].y-ptNodes[i-1].y)<1e-4 &&
						fabs(ptNodes[i].z-ptNodes[i-1].z)<1e-4 )
					{
						continue;
					}
					
					pv[pos].Point.x = ptNodes[i].x;
					pv[pos].Point.y = ptNodes[i].y;
					pv[pos].Point.z = ptNodes[i].z;
					pv[pos].StartWidth = 0;
					pv[pos].EndWidth = 0;
					
					//当开始另外一段线或者要结束时，就导出已经积累的线串
					if( i>0 && ( (ptNodes[i].code&1)==GRBUFFER_PTCODE_MOVETO || i==(ptnum-1) ) )
					{
						//如果要结束，并且当前点是LINETO模式，导出点中应该包含当前点，
						//而且宽度应该更新一下（避免起点宽度为无效的值）
						if( pos>=1 && (ptNodes[i].code&1)==GRBUFFER_PTCODE_LINETO )
						{
							pv[0].StartWidth = 0;
							pv[0].EndWidth = 0;
							
							if (color_ftr == FTRCOLOR_BYLAYER)
							{
								m_DxfDraw.SetColor(256);
							}
							else
							{
								if (color == 0)
								{
									color = 255;
								}
							m_DxfDraw.SetColor(color);
							}
							m_DxfDraw.PolyLine(pv,pos+1,1);
						}
						else if( pos>=2 )
						{
							if (color_ftr == FTRCOLOR_BYLAYER)
							{
								m_DxfDraw.SetColor(256);
							}
							else
							{
								if (color == 0)
								{
									color = 255;
								}
							m_DxfDraw.SetColor(color);
							}
							m_DxfDraw.PolyLine(pv,pos,1);
						}
						pos = 0;
						
						pv[pos].Point.x = ptNodes[i].x;
						pv[pos].Point.y = ptNodes[i].y;
						pv[pos].Point.z = ptNodes[i].z;
						pv[pos].StartWidth = 0;
						pv[pos].EndWidth = 0;	
					}
					//更新宽度，避免起点宽度为无效的值
					else
					{
						pv[0].StartWidth = 0;
						pv[0].EndWidth = 0;
					}
					
					pos++;
				}

				delete pv;
			}
			else if(pGr->type == GRAPH_TYPE_TEXT)
			{
				GrText *pText = (GrText*)pGr;
				if (color_ftr == FTRCOLOR_BYLAYER)
				{
					m_DxfDraw.SetColor(256);
				}
				else
				{
					if (color == 0)
					{
						color = 255;
					}
				m_DxfDraw.SetColor(color);
				}

				// 过滤换行符
				CString strText = pText->text;
				RemoveChar(strText,"\r\n\t,，");

				int nJustification = ConvertAlignmentAndJustification(pText->settings.nAlignment,TRUE);
				m_DxfDraw.Text(strText,pText->pt.x,pText->pt.y,pText->pt.x,pText->pt.y,pText->pt.z,pText->settings.fHeight*0.6/**GetSymbolDrawScale()*/,nJustification,
					0,1,0);
			}
			
			pGr = pGr->next;
			
		}	
	}

	return;
}

void CDxfWrite::SetBlock_dwg(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, BOOL bNewBlock, LPCTSTR blkName, CFtrLayer *pLayer,LPCTSTR cassCode)
{
	if( !m_pDb )
		return;

	if( pLayer==NULL )
	{
		pLayer = m_pDS->GetFtrLayerOfObject(pFtr);
		if( pLayer==NULL )
			pLayer = m_pCurFdbLayer;
	}
	
	CGeometry *pObj = pFtr->GetGeometry();	
	
	PBLOCKHEADER ph = NULL;
	OBJHANDLE hBlock = NULL;
	
	long color = ConvertColor(pObj->GetColor(),pLayer);

	HRESULT     hr;
	CString strBlockName;
	IAcadBlock *pBlock = NULL;
	VARIANT varPts;
	IAcadBlocks  *pBlocksTable = NULL;
	hr = m_pDb->get_Blocks(&pBlocksTable);
	if (FAILED(hr))  return;

	float fBlockScale = 1.0f;
	if( m_dlgDxfExport.m_bBlockScale )
		fBlockScale = m_dlgDxfExport.m_fBlockScale;

	VariantInit(&varPts);
	
	if( !bNewBlock )
	{
		CString strNewBlockName;
		
		if( blkName!=NULL )
			strNewBlockName = blkName;
		else if( pLayer!=NULL )
			strNewBlockName = GetExportedBlockName(pLayer,pObj->GetColor());

		if( strNewBlockName.IsEmpty() )
		{
			pBlocksTable->Release();
			return;
		}

		strBlockName = strNewBlockName;		

		if( m_arrDWGBlockNames.GetSize()<=0 )
		{
			long nCount = 0;
			pBlocksTable->get_Count(&nCount);
			for (long i=0; i<nCount; i++)
			{
				IAcadBlock *pObject = NULL;
				_variant_t  vItem(i,VT_I4);

				pBlocksTable->Item(vItem, &pObject);
				if (S_OK == hr)
				{
					BSTR    bName;
					pObject->get_Name(&bName);

					_bstr_t tmpstr(bName,FALSE);

					m_arrDWGBlockNames.Add((LPCTSTR)tmpstr);
					
					if (strBlockName.CompareNoCase((LPCTSTR)tmpstr) == 0)
					{
						pBlock = (IAcadBlock*)(1);
					}
					
					pObject->Release();
				}
			}
		}
		else
		{
			long nCount = m_arrDWGBlockNames.GetSize();
			for (long i=0; i<nCount; i++)
			{
				if (strBlockName.CompareNoCase((LPCTSTR)m_arrDWGBlockNames[i]) == 0)
				{
					pBlock = (IAcadBlock*)(1);
					break;
				}
			}
		}
	}
	else
	{
		//防止多文件合并时，块冲突的问题
		strBlockName.Format("%sb%d",(LPCTSTR)m_docName,m_nBlockNo++);
	}

	if( pBlock!=NULL )
	{
		double ang = 0;
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{			
			ang = ((CGeoPoint*)pObj)->m_lfAngle*PI/180;				
		}

		float kx = 1, ky = 1;
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{
			CGeoPoint *pPoint = (CGeoPoint*)pObj;
			kx = pPoint->m_fKx;
			ky = pPoint->m_fKy;
		}

		BOOL bDWGBlock = TRUE;
		for( int i=0; i<m_arrNewBlockNames.GetSize(); i++)
		{
			if( m_arrNewBlockNames[i].CompareNoCase(strBlockName)==0 )
			{
				bDWGBlock = FALSE;
				break;
			}
		}

		PT_3DEX base = pObj->GetDataPoint(0);
		Create3DPointArray(&base,1,&varPts,sizeof(PT_3DEX));
		
		IAcadBlockReference *pBlkRef = NULL;
		_variant_t  vParam;	
		vParam.vt = VT_ERROR; 

		if( bDWGBlock )
			m_pMS->InsertBlock(varPts,CharToWChar(strBlockName),fBlockScale,fBlockScale,1,ang,vParam,&pBlkRef);
		else
			m_pMS->InsertBlock(varPts,CharToWChar(strBlockName),kx,ky,1,ang,vParam,&pBlkRef);

		if( pBlkRef )
		{
			pBlkRef->put_Layer(CharToWChar(dxfLayerName));

			if(m_dlgDxfExport.m_bUseRGBTrueColor)
			{
				pBlkRef->put_TrueColor(TrueColor(color));
			}
			else
			{
				pBlkRef->put_color(ACAD_COLOR(color));
			}

			WriteCassCode(pBlkRef,cassCode);

			pBlkRef->Release();
		}

		pBlocksTable->Release();

		ArrayClear(&varPts);

		return;
	}

	NormalizeName(strBlockName);
	
	//添加新块
	//if (arrPSyms.GetSize()>0)
	{
		double ang = 0;
		CGeoPoint save_obj;
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{
			CGeoPoint *pPoint = (CGeoPoint*)pObj;
			save_obj.CopyFrom(pObj);

			ang = pPoint->m_lfAngle*PI/180;
			pPoint->m_lfAngle = 0;
			pPoint->m_fKx = 1;
			pPoint->m_fKy = 1;
			pPoint->m_nCoverType = 0;
			pPoint->m_fExtendDis = 0;
		}

		float scale = m_pDS->GetSymbolDrawScale();
		GrBuffer buf,buf1;
		for(int i=0; i<arrPSyms.GetSize(); i++)
		{
			CSymbol *pSymbol = arrPSyms[i];

			if (pSymbol->GetType() == SYMTYPE_ANNOTATION)
			{
				CValueTable tab;
				m_pDS->GetAllAttribute(pFtr,tab);
				((CAnnotation*)pSymbol)->Draw(pFtr,&buf1,tab,scale,0,1);
			}			
			else
				pSymbol->Draw(pFtr,&buf1,scale);
		}

		if( arrPSyms.GetSize()==0 )
		{
			pFtr->Draw(&buf1,scale);
		}

		buf1.GetAllGraph(&buf);
		if(buf.HeadGraph()==NULL)
		{
			pBlocksTable->Release();
			return;
		}

		PT_3DEX base;
		Envelope enp = buf.GetEnvelope();
		base = pObj->GetDataPoint(0);
		
		buf.Move(-base.x,-base.y,-base.z);
		
		Graph *pGr = buf.HeadGraph();

		Create3DPointArray(&PT_3D(0,0,0),1,&varPts,sizeof(PT_3D));
		IAcadBlock *pBk = NULL;
		hr = pBlocksTable->Add(varPts,CharToWChar(strBlockName),&pBk);

		ArrayClear(&varPts);

		if (FAILED(hr))
		{
			pBlocksTable->Release();
			return;
		}

		m_arrNewBlockNames.Add(strBlockName);
		m_arrDWGBlockNames.Add(strBlockName);
		
		while(pGr)
		{
			if(pGr->type == GRAPH_TYPE_POINT)
			{
				GrPoint *pP = (GrPoint*)pGr;

				if( pP->bGrdSize )
				{
					//导出边界线（空心）
					if( 0 )
					{
						if( pP->bFlat )
						{
							IAcadLWPolyline *pPoly = NULL;
							PT_3D pts[4];
							pts[0].x = pP->pt.x-pP->kx*0.5;  pts[0].y = pP->pt.y-pP->ky*0.5;  pts[0].z = pP->pt.z;
							pts[1].x = pP->pt.x+pP->kx*0.5;  pts[1].y = pP->pt.y-pP->ky*0.5;  pts[1].z = pP->pt.z;
							pts[2].x = pP->pt.x+pP->kx*0.5;  pts[2].y = pP->pt.y+pP->ky*0.5;  pts[2].z = pP->pt.z;
							pts[3].x = pP->pt.x-pP->kx*0.5;  pts[3].y = pP->pt.y+pP->ky*0.5;  pts[3].z = pP->pt.z;

							Create3DPointArray(pts,4,&varPts,sizeof(PT_3D),FALSE);
							pBk->AddLightWeightPolyline(varPts,&pPoly);	
							
							if( pPoly )
							{							
								// 设置线宽
								for (int w=0; w<4; w++)
								{
									pPoly->SetWidth(w,0,0);
								}
								
								pPoly->put_ConstantWidth(0);

								if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
								{
									pPoly->put_TrueColor(TrueColor(color));
								}
								else
								{
									pPoly->put_color(ACAD_COLOR(color));
								}

								pPoly->put_color(acByBlock);								
								pPoly->Release();
							}
							
							ArrayClear(&varPts);
						}
						else
						{
							PT_3D pt;
							pt.x = pP->pt.x;
							pt.y = pP->pt.y;
							pt.z = pP->pt.z;
							Create3DPointArray(&pt,1,&varPts);	

							IAcadArc *pArc = NULL;
							pBk->AddArc(varPts,pP->kx*0.5,0,2*PI,&pArc);
							
							if( pArc )
							{	
								if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
								{
									pArc->put_TrueColor(TrueColor(color));
								}
								else
								{
									pArc->put_color(ACAD_COLOR(color));
								}
								pArc->put_color(acByBlock);
								pArc->Release();
							}
						}
					}
					//导出Hatch（还存在两个问题：1、dxf格式Hatch还不支持；2、dwg格式的Hatch，边界和填充之间不能联动）
					else
					{
						CGeoSurface geo;
						geo.SetColor(pObj->GetColor());
						if( pP->bFlat )
						{
							PT_3DEX pts[5];
							pts[0].x = pP->pt.x-pP->kx*0.5;  pts[0].y = pP->pt.y-pP->ky*0.5;  pts[0].z = pP->pt.z; pts[0].pencode = penLine;
							pts[1].x = pP->pt.x+pP->kx*0.5;  pts[1].y = pP->pt.y-pP->ky*0.5;  pts[1].z = pP->pt.z; pts[1].pencode = penLine;
							pts[2].x = pP->pt.x+pP->kx*0.5;  pts[2].y = pP->pt.y+pP->ky*0.5;  pts[2].z = pP->pt.z; pts[2].pencode = penLine;
							pts[3].x = pP->pt.x-pP->kx*0.5;  pts[3].y = pP->pt.y+pP->ky*0.5;  pts[3].z = pP->pt.z; pts[3].pencode = penLine;
							pts[4] = pts[0];
							geo.CreateShape(pts,5);							
						}
						else
						{
							PT_3DEX pts[4];
							for( int j=0; j<3; j++)
							{
								pts[j].x = pP->pt.x+pP->kx*0.5*cos(PI*j*0.5);  
								pts[j].y = pP->pt.y+pP->kx*0.5*sin(PI*j*0.5);  
								pts[j].z = pP->pt.z; 
								pts[j].pencode = penArc;
							}
							pts[3] = pts[0];
							geo.CreateShape(pts,4);
						}

						SaveHatch(pBk,&geo,dxfLayerName,NULL,0,0);
					}

				}
				else
				{
					IAcadPoint *pPoint = NULL;
					Create3DPointArray(&pP->pt,1,&varPts,sizeof(PT_3D));
					pBk->AddPoint(varPts,&pPoint);
					
					if( pPoint )
					{
						if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
						{
							pPoint->put_TrueColor(TrueColor(color));
						}
						else
						{
							pPoint->put_color(ACAD_COLOR(color));
						}
						pPoint->put_color(acByBlock);
						pPoint->Release();
					}
					
					ArrayClear(&varPts);
				}
				
			}
			else if(pGr->type == GRAPH_TYPE_POINTSTRING )
			{
				GrPointString *p = (GrPointString*)pGr;
				
				for( i=0; i<p->ptlist.nuse; i++)
				{
					IAcadPoint *pPoint = NULL;
					Create3DPointArray(p->ptlist.pts+i,1,&varPts,sizeof(PT_3D));
					pBk->AddPoint(varPts,&pPoint);
					
					if( pPoint )
					{
						if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
						{
							pPoint->put_TrueColor(TrueColor(color));
						}
						else
						{
							pPoint->put_color(ACAD_COLOR(color));
						}
						pPoint->put_color(acByBlock);
						pPoint->Release();
					}					
					ArrayClear(&varPts);
				}
			}
			else if(pGr->type == GRAPH_TYPE_LINESTRING)
			{
				GrLineString *pLine = (GrLineString*)pGr;
				GrVertexList ptlist = pLine->ptlist;
				GrVertex *ptNodes = ptlist.pts;
				int ptnum = ptlist.nuse;
				
				PT_3D *pv = new PT_3D[ptnum];
				memset(pv,0,sizeof(PT_3D)*ptnum);

				float wid = 0;
				if( pLine->bGrdWid )
					wid = pLine->width;
				
				int pos = 0;
				for( int i=0; i<ptlist.nuse; i++)
				{
					//去除相同点
					//如果和以前的两断线是紧密衔接的，这样可以继续读取断线，以便于将紧致的断线衔接起来
					if( i>0 && i!=(ptlist.nuse-1) &&
						fabs(ptNodes[i].x-ptNodes[i-1].x)<1e-4 && 
						fabs(ptNodes[i].y-ptNodes[i-1].y)<1e-4 &&
						fabs(ptNodes[i].z-ptNodes[i-1].z)<1e-4 )
					{
						continue;
					}
					
					pv[pos].x = ptNodes[i].x;
					pv[pos].y = ptNodes[i].y;
					pv[pos].z = ptNodes[i].z;
// 					pv[pos].StartWidth = pLine->width;
// 					pv[pos].EndWidth = pLine->width;
					
					//当开始另外一段线或者要结束时，就导出已经积累的线串
					if( i>0 && ( (ptNodes[i].code&1)==GRBUFFER_PTCODE_MOVETO || i==(ptnum-1) ) )
					{
						//如果要结束，并且当前点是LINETO模式，导出点中应该包含当前点，
						//而且宽度应该更新一下（避免起点宽度为无效的值）
						if( pos>=1 && (ptNodes[i].code&1)==GRBUFFER_PTCODE_LINETO )
						{
// 							pv[0].StartWidth = pLine->width;
// 							pv[0].EndWidth = pLine->width;

							IAcadLWPolyline *pPoly = NULL;
							Create3DPointArray(pv,pos+1,&varPts,sizeof(PT_3D),FALSE);
							pBk->AddLightWeightPolyline(varPts,&pPoly);

							ArrayClear(&varPts);

							if( pPoly )
							{
								// 设置线宽
								for (int w=0; w<pos+1; w++)
								{
									pPoly->SetWidth(w,wid,wid);
								}

								pPoly->put_ConstantWidth(wid);
								
								if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
								{
									pPoly->put_TrueColor(TrueColor(color));
								}
								else
								{
									pPoly->put_color(ACAD_COLOR(color));
								}
								pPoly->put_color(acByBlock);
								
								pPoly->Release();
							}
						}
						else if( pos>=2 )
						{
							IAcadLWPolyline *pPoly = NULL;
							Create3DPointArray(pv,pos,&varPts,sizeof(PT_3D),FALSE);
							pBk->AddLightWeightPolyline(varPts,&pPoly);
							
							if( pPoly )
							{
								// 设置线宽
								for (int w=0; w<pos; w++)
								{
									pPoly->SetWidth(w,wid,wid);
								}

								pPoly->put_ConstantWidth(wid);

								if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
								{
									pPoly->put_TrueColor(TrueColor(color));
								}
								else
								{
									pPoly->put_color(ACAD_COLOR(color));
								}
								pPoly->put_color(acByBlock);
								pPoly->Release();
							}

							ArrayClear(&varPts);
						}
						pos = 0;
						
						pv[pos].x = ptNodes[i].x;
						pv[pos].y = ptNodes[i].y;
						pv[pos].z = ptNodes[i].z;
// 						pv[pos].StartWidth = pLine->width;
// 						pv[pos].EndWidth = pLine->width;	
					}
					//更新宽度，避免起点宽度为无效的值
					else
					{
// 						pv[0].StartWidth = pLine->width;
// 						pv[0].EndWidth = pLine->width;
					}
					
					pos++;
				}

				delete pv;
			}
			else if(pGr->type == GRAPH_TYPE_POLYGON)
			{
				GrPolygon *pPolygon = (GrPolygon*)pGr;
				GrVertexList ptlist = pPolygon->ptlist;
				GrVertex *ptNodes = ptlist.pts;
				int ptnum = ptlist.nuse;
				
				PT_3DEX *pv = new PT_3DEX[ptnum];
				memset(pv,0,sizeof(PT_3DEX)*ptnum);
				
				int pos = 0;
				for( int i=0; i<ptnum; i++)
				{
					//去除相同点
					//如果和以前的两断线是紧密衔接的，这样可以继续读取断线，以便于将紧致的断线衔接起来
					if( i>0 && i!=(ptnum-1) &&
						fabs(ptNodes[i].x-ptNodes[i-1].x)<1e-4 && 
						fabs(ptNodes[i].y-ptNodes[i-1].y)<1e-4 &&
						fabs(ptNodes[i].z-ptNodes[i-1].z)<1e-4 )
					{
						continue;
					}
					
					pv[pos].x = ptNodes[i].x;
					pv[pos].y = ptNodes[i].y;
					pv[pos].z = ptNodes[i].z;
					pv[pos].pencode = penLine;
					
					//当开始另外一段线或者要结束时，就导出已经积累的线串
					if( i>0 && ( (ptNodes[i].code&1)==GRBUFFER_PTCODE_MOVETO || i==(ptnum-1) ) )
					{
						//如果要结束，并且当前点是LINETO模式，导出点中应该包含当前点，
						//而且宽度应该更新一下（避免起点宽度为无效的值）
						if( pos>=1 && (ptNodes[i].code&1)==GRBUFFER_PTCODE_LINETO )
						{
							CGeoSurface geo;
							geo.CreateShape(pv,pos+1);
							geo.SetColor(pObj->GetColor());

							geo.EnableFillColor(TRUE,pObj->GetColor());
							SaveHatch(pBk,&geo,dxfLayerName,NULL,0,0);
							
						}
						//GRBUFFER_PTCODE_MOVETO 的情况
						else if( pos>=2 )
						{
							CGeoSurface geo;
							geo.CreateShape(pv,pos);
							geo.SetColor(pObj->GetColor());
							
							geo.EnableFillColor(TRUE,pObj->GetColor());
							SaveHatch(pBk,&geo,dxfLayerName,NULL,0,0);
						}
						pos = 0;
						
						pv[pos].x = ptNodes[i].x;
						pv[pos].y = ptNodes[i].y;
						pv[pos].z = ptNodes[i].z;
					}
					
					pos++;
				}

				delete pv;
			}
			else if(pGr->type == GRAPH_TYPE_TEXT)
			{
				GrText *pText = (GrText*)pGr;

				// 过滤换行符
				CString strText = pText->text;

				RemoveChar(strText,"\r\n\t,，");

				IAcadText *pText2 = NULL;
				Create3DPointArray(&pText->pt,1,&varPts,sizeof(PT_3D));

				pBk->AddText(CharToWChar(strText),varPts,pText->settings.fHeight*0.6/**GetSymbolDrawScale()*/,&pText2);

				if( pText2 )
				{
					if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
					{
						pText2->put_TrueColor(TrueColor(color));
					}
					else
					{
						pText2->put_color(ACAD_COLOR(color));
					}
					pText2->put_color(acByBlock);
					pText2->put_TextAlignmentPoint(varPts);
					
					if( pText->settings.nAlignment&TAH_LEFT )
						pText2->put_HorizontalAlignment(acHorizontalAlignmentLeft);
					else if( pText->settings.nAlignment&TAH_MID )
						pText2->put_HorizontalAlignment(acHorizontalAlignmentCenter);
					else if( pText->settings.nAlignment&TAH_RIGHT )
						pText2->put_HorizontalAlignment(acHorizontalAlignmentRight);

					if( pText->settings.nAlignment&TAV_TOP )
						pText2->put_VerticalAlignment(acVerticalAlignmentTop);
					else if( pText->settings.nAlignment&TAV_MID )
						pText2->put_VerticalAlignment(acVerticalAlignmentMiddle);
					else if( pText->settings.nAlignment&TAV_BOTTOM )
						pText2->put_VerticalAlignment(acVerticalAlignmentBottom);
					
					pText2->Release();
				}

				ArrayClear(&varPts);
			}
			
			pGr = pGr->next;
			
		}

		pBk->Release();

		IAcadPoint *pPoint = NULL;
		Create3DPointArray(&base,1,&varPts,sizeof(PT_3DEX));
		
		IAcadBlockReference *pBlkRef = NULL;
		_variant_t  vParam;	
		vParam.vt = VT_ERROR; 

		m_pMS->InsertBlock(varPts,CharToWChar(strBlockName),save_obj.m_fKx,save_obj.m_fKy,1,ang,vParam,&pBlkRef);

		if( pBlkRef )
		{
			pBlkRef->put_Layer(CharToWChar(dxfLayerName));

			if(m_dlgDxfExport.m_bUseRGBTrueColor)
			{
				pBlkRef->put_TrueColor(TrueColor(color));
			}
			else
			{
				pBlkRef->put_color(ACAD_COLOR(color));
			}

			WriteCassCode(pBlkRef,cassCode);

			pBlkRef->Release();
		}
	
		ArrayClear(&varPts);

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{
			CGeoPoint *pPoint = (CGeoPoint*)pObj;
			
			pPoint->CopyFrom(&save_obj);
		}
		
	}

	pBlocksTable->Release();
}



void CDxfWrite::SetBlock_exploded_dwg(CFeature *pFtr, LPCTSTR dxfLayerName, CSymbolArray& arrPSyms, CFtrLayer *pLayer,LPCTSTR cassCode)
{
	if( !m_pDb )
		return;

	if( pLayer==NULL )
		pLayer = m_pDS->GetFtrLayerOfObject(pFtr);
	
	CGeometry *pObj = pFtr->GetGeometry();	
		
	long color = ConvertColor(pObj->GetColor(),pLayer);

	HRESULT     hr;
	VARIANT varPts;

	VariantInit(&varPts);
	
	//if (arrPSyms.GetSize()>0)
	{

		float scale = m_pDS->GetSymbolDrawScale();
		GrBuffer buf,buf1;
		for(int i=0; i<arrPSyms.GetSize(); i++)
		{
			CSymbol *pSymbol = arrPSyms[i];

			if (pSymbol->GetType() == SYMTYPE_ANNOTATION)
			{
				CValueTable tab;
				m_pDS->GetAllAttribute(pFtr,tab);
				((CAnnotation*)pSymbol)->Draw(pFtr,&buf1,tab,scale,0,1);
			}			
			else
				pSymbol->Draw(pFtr,&buf1,scale);
		}

		if( arrPSyms.GetSize()==0 )
		{
			pFtr->Draw(&buf1,scale);
		}

		buf1.GetAllGraph(&buf);
		
		Graph *pGr = buf.HeadGraph();

		IAcadBlock *pBk = m_pMS;

		ArrayClear(&varPts);
		
		while(pGr)
		{
			if(pGr->type == GRAPH_TYPE_POINT)
			{
				GrPoint *pP = (GrPoint*)pGr;

				if( pP->bGrdSize )
				{
					//导出边界线（空心）
					if( 0 )
					{
						if( pP->bFlat )
						{
							IAcadLWPolyline *pPoly = NULL;
							PT_3D pts[4];
							pts[0].x = pP->pt.x-pP->kx*0.5;  pts[0].y = pP->pt.y-pP->ky*0.5;  pts[0].z = pP->pt.z;
							pts[1].x = pP->pt.x+pP->kx*0.5;  pts[1].y = pP->pt.y-pP->ky*0.5;  pts[1].z = pP->pt.z;
							pts[2].x = pP->pt.x+pP->kx*0.5;  pts[2].y = pP->pt.y+pP->ky*0.5;  pts[2].z = pP->pt.z;
							pts[3].x = pP->pt.x-pP->kx*0.5;  pts[3].y = pP->pt.y+pP->ky*0.5;  pts[3].z = pP->pt.z;

							Create3DPointArray(pts,4,&varPts,sizeof(PT_3D),FALSE);
							pBk->AddLightWeightPolyline(varPts,&pPoly);	
							
							if( pPoly )
							{							
								// 设置线宽
								for (int w=0; w<4; w++)
								{
									pPoly->SetWidth(w,0,0);
								}
								
								pPoly->put_ConstantWidth(0);
								pPoly->put_Layer(CharToWChar(dxfLayerName));
								if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
								{
									pPoly->put_TrueColor(TrueColor(color));
								}
								else
								{
									pPoly->put_color(ACAD_COLOR(color));
								}
								WriteCassCode(pPoly,cassCode);
								pPoly->Release();
							}
							
							ArrayClear(&varPts);
						}
						else
						{
							PT_3D pt;
							pt.x = pP->pt.x;
							pt.y = pP->pt.y;
							pt.z = pP->pt.z;
							Create3DPointArray(&pt,1,&varPts);	

							IAcadArc *pArc = NULL;
							pBk->AddArc(varPts,pP->kx*0.5,0,2*PI,&pArc);
							
							if( pArc )
							{
								if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
								{
									pArc->put_TrueColor(TrueColor(color));
								}
								else
								{
									pArc->put_color(ACAD_COLOR(color));
								}
								pArc->put_Layer(CharToWChar(dxfLayerName));
								WriteCassCode(pArc,cassCode);
								pArc->Release();
							}
						}
					}
					//导出Hatch（还存在两个问题：1、dxf格式Hatch还不支持；2、dwg格式的Hatch，边界和填充之间不能联动）
					else
					{
						CGeoSurface geo;
						geo.SetColor(pObj->GetColor());
						if( pP->bFlat )
						{
							PT_3DEX pts[5];
							pts[0].x = pP->pt.x-pP->kx*0.5;  pts[0].y = pP->pt.y-pP->ky*0.5;  pts[0].z = pP->pt.z; pts[0].pencode = penLine;
							pts[1].x = pP->pt.x+pP->kx*0.5;  pts[1].y = pP->pt.y-pP->ky*0.5;  pts[1].z = pP->pt.z; pts[1].pencode = penLine;
							pts[2].x = pP->pt.x+pP->kx*0.5;  pts[2].y = pP->pt.y+pP->ky*0.5;  pts[2].z = pP->pt.z; pts[2].pencode = penLine;
							pts[3].x = pP->pt.x-pP->kx*0.5;  pts[3].y = pP->pt.y+pP->ky*0.5;  pts[3].z = pP->pt.z; pts[3].pencode = penLine;
							pts[4] = pts[0];
							geo.CreateShape(pts,5);							
						}
						else
						{
							PT_3DEX pts[4];
							for( int j=0; j<3; j++)
							{
								pts[j].x = pP->pt.x+pP->kx*0.5*cos(PI*j*0.5);  
								pts[j].y = pP->pt.y+pP->kx*0.5*sin(PI*j*0.5);  
								pts[j].z = pP->pt.z; 
								pts[j].pencode = penArc;
							}
							pts[3] = pts[0];
							geo.CreateShape(pts,4);
						}

						SaveHatch(pBk,&geo,dxfLayerName,NULL,0,0);
					}

				}
				else
				{
					IAcadPoint *pPoint = NULL;
					Create3DPointArray(&pP->pt,1,&varPts,sizeof(PT_3D));
					pBk->AddPoint(varPts,&pPoint);
					
					if( pPoint )
					{
						if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
						{
							pPoint->put_TrueColor(TrueColor(color));
						}
						else
						{
							pPoint->put_color(ACAD_COLOR(color));
						}
						pPoint->put_Layer(CharToWChar(dxfLayerName));
						WriteCassCode(pPoint,cassCode);
						pPoint->Release();
					}
					
					ArrayClear(&varPts);
				}
				
			}
			else if(pGr->type == GRAPH_TYPE_POINTSTRING )
			{
				GrPointString *p = (GrPointString*)pGr;
				
				for( i=0; i<p->ptlist.nuse; i++)
				{
					IAcadPoint *pPoint = NULL;
					Create3DPointArray(p->ptlist.pts+i,1,&varPts,sizeof(PT_3D));
					pBk->AddPoint(varPts,&pPoint);
					
					if( pPoint )
					{
						if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
						{
							pPoint->put_TrueColor(TrueColor(color));
						}
						else
						{
							pPoint->put_color(ACAD_COLOR(color));
						}
						pPoint->put_Layer(CharToWChar(dxfLayerName));
						WriteCassCode(pPoint,cassCode);
						pPoint->Release();
					}					
					ArrayClear(&varPts);
				}
			}
			else if(pGr->type == GRAPH_TYPE_LINESTRING)
			{
				GrLineString *pLine = (GrLineString*)pGr;
				GrVertexList ptlist = pLine->ptlist;
				GrVertex *ptNodes = ptlist.pts;
				int ptnum = ptlist.nuse;
				
				PT_3D *pv = new PT_3D[ptnum];
				memset(pv,0,sizeof(PT_3D)*ptnum);

				float wid = 0;
				if( pLine->bGrdWid )
					wid = pLine->width;
				
				int pos = 0;
				for( int i=0; i<ptlist.nuse; i++)
				{
					//去除相同点
					//如果和以前的两断线是紧密衔接的，这样可以继续读取断线，以便于将紧致的断线衔接起来
					if( i>0 && i!=(ptlist.nuse-1) &&
						fabs(ptNodes[i].x-ptNodes[i-1].x)<1e-4 && 
						fabs(ptNodes[i].y-ptNodes[i-1].y)<1e-4 &&
						fabs(ptNodes[i].z-ptNodes[i-1].z)<1e-4 )
					{
						continue;
					}
					
					pv[pos].x = ptNodes[i].x;
					pv[pos].y = ptNodes[i].y;
					pv[pos].z = ptNodes[i].z;
// 					pv[pos].StartWidth = pLine->width;
// 					pv[pos].EndWidth = pLine->width;
					
					//当开始另外一段线或者要结束时，就导出已经积累的线串
					if( i>0 && ( (ptNodes[i].code&1)==GRBUFFER_PTCODE_MOVETO || i==(ptnum-1) ) )
					{
						//如果要结束，并且当前点是LINETO模式，导出点中应该包含当前点，
						//而且宽度应该更新一下（避免起点宽度为无效的值）
						if( pos>=1 && (ptNodes[i].code&1)==GRBUFFER_PTCODE_LINETO )
						{
// 							pv[0].StartWidth = pLine->width;
// 							pv[0].EndWidth = pLine->width;

							IAcadLWPolyline *pPoly = NULL;
							Create3DPointArray(pv,pos+1,&varPts,sizeof(PT_3D),FALSE);
							pBk->AddLightWeightPolyline(varPts,&pPoly);

							ArrayClear(&varPts);

							if( pPoly )
							{
								// 设置线宽
								for (int w=0; w<pos+1; w++)
								{
									pPoly->SetWidth(w,wid,wid);
								}

								pPoly->put_ConstantWidth(wid);
								pPoly->put_Layer(CharToWChar(dxfLayerName));
								if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
								{
									pPoly->put_TrueColor(TrueColor(color));
								}
								else
								{
									pPoly->put_color(ACAD_COLOR(color));
								}

								WriteCassCode(pPoly,cassCode);
								
								pPoly->Release();
							}
						}
						else if( pos>=2 )
						{
							IAcadLWPolyline *pPoly = NULL;
							Create3DPointArray(pv,pos,&varPts,sizeof(PT_3D),FALSE);
							pBk->AddLightWeightPolyline(varPts,&pPoly);
							
							if( pPoly )
							{
								// 设置线宽
								for (int w=0; w<pos; w++)
								{
									pPoly->SetWidth(w,wid,wid);
								}

								pPoly->put_ConstantWidth(wid);
								pPoly->put_Layer(CharToWChar(dxfLayerName));
								if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
								{
									pPoly->put_TrueColor(TrueColor(color));
								}
								else
								{
									pPoly->put_color(ACAD_COLOR(color));
								}


								WriteCassCode(pPoly,cassCode);
								
								pPoly->Release();
							}

							ArrayClear(&varPts);
						}
						pos = 0;
						
						pv[pos].x = ptNodes[i].x;
						pv[pos].y = ptNodes[i].y;
						pv[pos].z = ptNodes[i].z;
// 						pv[pos].StartWidth = pLine->width;
// 						pv[pos].EndWidth = pLine->width;	
					}
					//更新宽度，避免起点宽度为无效的值
					else
					{
// 						pv[0].StartWidth = pLine->width;
// 						pv[0].EndWidth = pLine->width;
					}
					
					pos++;
				}

				delete pv;
			}
			else if(pGr->type == GRAPH_TYPE_POLYGON)
			{
				GrPolygon *pPolygon = (GrPolygon*)pGr;
				GrVertexList ptlist = pPolygon->ptlist;
				GrVertex *ptNodes = ptlist.pts;
				int ptnum = ptlist.nuse;
				
				PT_3DEX *pv = new PT_3DEX[ptnum];
				memset(pv,0,sizeof(PT_3DEX)*ptnum);
				
				int pos = 0;
				for( int i=0; i<ptnum; i++)
				{
					//去除相同点
					//如果和以前的两断线是紧密衔接的，这样可以继续读取断线，以便于将紧致的断线衔接起来
					if( i>0 && i!=(ptnum-1) &&
						fabs(ptNodes[i].x-ptNodes[i-1].x)<1e-4 && 
						fabs(ptNodes[i].y-ptNodes[i-1].y)<1e-4 &&
						fabs(ptNodes[i].z-ptNodes[i-1].z)<1e-4 )
					{
						continue;
					}
					
					pv[pos].x = ptNodes[i].x;
					pv[pos].y = ptNodes[i].y;
					pv[pos].z = ptNodes[i].z;
					pv[pos].pencode = penLine;
					
					//当开始另外一段线或者要结束时，就导出已经积累的线串
					if( i>0 && ( (ptNodes[i].code&1)==GRBUFFER_PTCODE_MOVETO || i==(ptnum-1) ) )
					{
						//如果要结束，并且当前点是LINETO模式，导出点中应该包含当前点，
						//而且宽度应该更新一下（避免起点宽度为无效的值）
						if( pos>=1 && (ptNodes[i].code&1)==GRBUFFER_PTCODE_LINETO )
						{
							CGeoSurface geo;
							geo.CreateShape(pv,pos+1);
							geo.SetColor(pObj->GetColor());

							geo.EnableFillColor(TRUE,pObj->GetColor());
							SaveHatch(pBk,&geo,dxfLayerName,NULL,0,0);
							
						}
						//GRBUFFER_PTCODE_MOVETO 的情况
						else if( pos>=2 )
						{
							CGeoSurface geo;
							geo.CreateShape(pv,pos);
							geo.SetColor(pObj->GetColor());
							
							geo.EnableFillColor(TRUE,pObj->GetColor());
							SaveHatch(pBk,&geo,dxfLayerName,NULL,0,0);
						}
						pos = 0;
						
						pv[pos].x = ptNodes[i].x;
						pv[pos].y = ptNodes[i].y;
						pv[pos].z = ptNodes[i].z;
					}
					
					pos++;
				}

				delete pv;
			}
			else if(pGr->type == GRAPH_TYPE_TEXT)
			{
				GrText *pText = (GrText*)pGr;

				// 过滤换行符
				CString strText = pText->text;

				RemoveChar(strText,"\r\n\t,，");

				IAcadText *pText2 = NULL;
				Create3DPointArray(&pText->pt,1,&varPts,sizeof(PT_3D));

				pBk->AddText(CharToWChar(strText),varPts,pText->settings.fHeight*0.6/**GetSymbolDrawScale()*/,&pText2);
				
				if( pText2 )
				{
					if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
					{
						pText2->put_TrueColor(TrueColor(color));
					}
					else
					{
						pText2->put_color(ACAD_COLOR(color));
					}
					pText2->put_TextAlignmentPoint(varPts);
					
					if( pText->settings.nAlignment&TAH_LEFT )
						pText2->put_HorizontalAlignment(acHorizontalAlignmentLeft);
					else if( pText->settings.nAlignment&TAH_MID )
						pText2->put_HorizontalAlignment(acHorizontalAlignmentCenter);
					else if( pText->settings.nAlignment&TAH_RIGHT )
						pText2->put_HorizontalAlignment(acHorizontalAlignmentRight);

					if( pText->settings.nAlignment&TAV_TOP )
						pText2->put_VerticalAlignment(acVerticalAlignmentTop);
					else if( pText->settings.nAlignment&TAV_MID )
						pText2->put_VerticalAlignment(acVerticalAlignmentMiddle);
					else if( pText->settings.nAlignment&TAV_BOTTOM )
						pText2->put_VerticalAlignment(acVerticalAlignmentBottom);
					
					pText2->put_Layer(CharToWChar(dxfLayerName));
					WriteCassCode(pText2,cassCode);
					pText2->Release();
				}

				ArrayClear(&varPts);
			}
			
			pGr = pGr->next;
			
		}

		ArrayClear(&varPts);		
	}

}


void CDxfWrite::SetAsExportBaselines()
{
	m_dlgDxfExport.m_bUseIndex = FALSE;
	m_dlgDxfExport.m_bAddAnnot = FALSE;
	m_dlgDxfExport.m_bMapFrame = FALSE;
	m_dlgDxfExport.m_bLinearize = TRUE;
	m_dlgDxfExport.m_bLines = TRUE;
	m_dlgDxfExport.m_bLineSym = FALSE;
	m_dlgDxfExport.m_bPoint = TRUE;
	m_dlgDxfExport.m_bPointSym = FALSE;
	m_dlgDxfExport.m_bSepPara = TRUE;
	m_dlgDxfExport.m_bParaCenline = FALSE;
	m_dlgDxfExport.m_bSurface = TRUE;
	m_dlgDxfExport.m_bSurfaceSym = FALSE;
	m_dlgDxfExport.m_bText = FALSE;
	m_dlgDxfExport.m_b3DLines = TRUE;
	m_dlgDxfExport.m_bDirPtAsLine = FALSE;
	m_dlgDxfExport.m_bClosedCurve = FALSE;
	m_dlgDxfExport.m_bTextSym = FALSE;
	m_dlgDxfExport.m_bDifferBaselineSym = FALSE;
}

void CDxfWrite::SetDlgDataSource(CDlgDataSource *pDS)
{
	if(!pDS)
	{
		CString strText;
		strText.LoadString(IDS_DATASOURCE_EMPTY);
		AfxMessageBox(strText);
		return;
	}
	
	m_pDS = pDS;
	
	AddDefaultLayer_dxf();
}

//从 str 中删除字符串 c 中包含的每一个字符
void RemoveChar(char *str, const char *c)
{
	if (strlen(str) <= 0) return;

	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

	wstring wstr0;
	ConvertCharToWstring(str,wstr0,codepage);

	wstring wc0;
	ConvertCharToWstring(c,wc0,codepage);

	wchar_t *wstr = (wchar_t*)wstr0.c_str();
	wchar_t *wc = (wchar_t*)wc0.c_str();

	wchar_t *pos = wstr, *wsrc = wstr;

	wchar_t *check = wc;
	while( *check!=0 )
	{
		pos = wstr = wsrc;

		while(*pos!=0)
		{
			if( *pos!=*check )
			{
				if( wstr!=pos )
					*wstr= *pos;
				wstr++;
			}
			pos++;
		}
		*wstr = 0;
		check++;
	}

	string ret0;
	ConvertWcharToString(wsrc,ret0,codepage,NULL,0);
	const char *ret = ret0.c_str();
	strcpy(str,ret);

}

void RemoveChar(CString& str, const char *c)
{
	char *buf = str.GetBuffer(str.GetLength()+1);
	RemoveChar(buf,c);
	str.ReleaseBuffer();
}


void ReplaceChar(CString& str, const char *c1, const char *c2)
{
	if (strlen(str) <= 0) return;
	
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	
	wstring wstr0;
	ConvertCharToWstring(str,wstr0,codepage);
	
	wstring wc1;
	ConvertCharToWstring(c1,wc1,codepage);
	int len1 = wc1.length();

	wstring wc2;
	ConvertCharToWstring(c2,wc2,codepage);
	int len2 = wc2.length();
	
	int pos = wstr0.find(wc1,0);
	while( pos>=0 )
	{
		wstr0.replace(pos,len1,wc2);

		pos = wstr0.find(wc1,pos + len2);
	}	
	
	string ret0;
	ConvertWcharToString(wstr0.data(),ret0,codepage,NULL,0);
	const char *ret = ret0.c_str();
	str = ret;
}


void ReplaceChar(char *str, const char *c1, const char *c2)
{
	CString str1 = str;
	ReplaceChar(str1,c1,c2);
	strcpy(str,str1);
}

static void ConvertGrBuffer2dToFtrs(GrBuffer2d *pBuf, CFtrLayer *pFtrLay, int nScale, CPtrArray &arr)
{
	if (!pBuf) return;

	const Graph2d *pGr = pBuf->HeadGraph(); 

	if (!pGr) return;
	
	while(pGr)
	{
		if (IsGrPoint2d(pGr))
		{
			const GrPoint2d *cgr = (const GrPoint2d*)pGr;			
			PT_3D pt;
			COPY_2DPT(pt,cgr->pt);

			CFeature *pFtr = pFtrLay->CreateDefaultFeature(nScale,CLS_GEOPOINT);
			if (!pFtr)
			{
				pGr = pGr->next;
				continue;
			}
			CGeoPoint *pGeo = (CGeoPoint*)pFtr->GetGeometry();
			if (!pGeo) 
			{
				delete pFtr;
				pGr = pGr->next;
				continue;
			}
			pGeo->CreateShape(&PT_3DEX(pt,penLine),1);
			if(pGeo->GetColor()==COLOUR_BYLAYER)
			{
				pGeo->SetColor(pFtrLay->GetColor());
			}
			else
			{
				pGeo->SetColor(cgr->color);
			}

			pGeo->m_fKx = cgr->kx;
			pGeo->m_fKy = cgr->ky;
			pGeo->m_fWidth = cgr->width;
			pGeo->m_lfAngle = cgr->angle*180/PI;

			arr.Add(pFtr);
		}
		else if( IsGrLineString2d(pGr) || IsGrPolygon2d(pGr) )
		{
			GrVertexList2d *ptlist;
			BOOL bCurve = TRUE;
			if( IsGrLineString2d(pGr) )
				ptlist = &((GrLineString2d*)pGr)->ptlist;
			else
			{
				ptlist = &((GrPolygon2d*)pGr)->ptlist;
				bCurve = FALSE;
			}
			
			GrVertex2d *ptNodes = ptlist->pts;
			int ptnum = ptlist->nuse;
			
			CArray<PT_3DEX,PT_3DEX> arrpts;
			arrpts.SetSize(ptnum);

			int pos = 0;
			for (int i=0; i<ptnum; i++)
			{
				if( i>0 && i!=(ptlist->nuse-1) &&
					fabs(ptNodes[i].x-ptNodes[i-1].x)<1e-4 && 
					fabs(ptNodes[i].y-ptNodes[i-1].y)<1e-4 )
				{
					continue;
				}

				COPY_2DPT(arrpts[pos],ptNodes[i]);
				arrpts[pos].pencode = penLine;

				if (i > 0 && (IsGrPtCodeMoveTo(ptNodes+i) || i==(ptnum-1)))
				{
					CFeature *pFtr = pFtrLay->CreateDefaultFeature(nScale,bCurve?CLS_GEOCURVE:CLS_GEOSURFACE);
					if (pFtr)
					{
						CGeometry *pGeo = pFtr->GetGeometry();
						if (pGeo)
						{
							
							int size = -1;
							if (pos >= 1 && IsGrPtCodeLineTo(ptNodes+i))
							{
								size = pos + 1;
							}
							else if (pos >= 2)
							{
								size = pos;
							}

							if (size > 0 && pGeo->CreateShape(arrpts.GetData(),size))
							{
								pGeo->SetColor(pGr->color);
								
								if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
								{
									if (((GrLineString2d*)pGr)->bGrdWid)
									{
										((CGeoCurve*)pGeo)->m_fLineWidth = ((GrLineString2d*)pGr)->width;
									}
									else
									{
										((CGeoCurve*)pGeo)->m_fLinewidthScale = ((GrLineString2d*)pGr)->width;
									}
									
								}
								else 
								{
									//((CGeoSurface*)pGeo)->m_fLineWidth = ((GrLineString2d*)pGr)->width;
								}
								arr.Add(pFtr);
							}
							else
							{
								delete pFtr;
							}
							
						}
					}	
					
					pos = 0;

					COPY_2DPT(arrpts[pos],ptNodes[i]);
					arrpts[pos].pencode = IsGrPtCodeMoveTo(&ptNodes[i])?penMove:penLine;
				}

				pos++;
				
			}			
			
		}
		else if (IsGrText2d(pGr))
		{
			GrText2d *pText = (GrText2d*)pGr;
			
			CFeature *pFtr = pFtrLay->CreateDefaultFeature(nScale,CLS_GEOTEXT);
			if (!pFtr)
			{
				pGr = pGr->next;
				continue;
			}
			CGeoText *pGeo = (CGeoText*)pFtr->GetGeometry();
			if (!pGeo)
			{
				pGr = pGr->next;
				continue;
			}

			int nPlaceType = singPt;
			{
				pGeo->CreateShape(&PT_3DEX(pText->pt.x,pText->pt.y,0,penLine),1);
			}

			
			if(pGeo->GetColor()==COLOUR_BYLAYER)
			{
				pGeo->SetColor(pFtrLay->GetColor());
			}
			else
			{
				pGeo->SetColor(pText->color);
			}
			pGeo->SetText(pText->text);
			
			TEXT_SETTINGS0 settings;
			settings.fHeight = pText->settings.fHeight;
			settings.fWidScale = pText->settings.fWidScale;
			settings.fCharIntervalScale = pText->settings.fCharIntervalScale;
			settings.fLineSpacingScale = pText->settings.fLineSpacingScale;
			settings.fTextAngle = pText->settings.fTextAngle;
			settings.fCharAngle = pText->settings.fCharAngle;			
			settings.nAlignment = pText->settings.nAlignment;
			settings.nInclineType = pText->settings.nInclineType;
			settings.fInclineAngle = pText->settings.fInclineAngle;	
			settings.nPlaceType = nPlaceType;
			_tcscpy(settings.strFontName,pText->settings.tcFaceName);		
			pGeo->SetSettings(&settings);

			arr.Add(pFtr);

		}

		pGr = pGr->next;
	}
}

void CDxfWrite::GrBuffer2dToDxf(GrBuffer2d *buf,LPCTSTR strDxf)
{
	CPtrArray arr, arrLayers;
	CFtrLayer ftrLayer;
	ftrLayer.SetName("0");
	ConvertGrBuffer2dToFtrs(buf,&ftrLayer,0,arr);	

	ObjectsToDxf(arr,arrLayers,strDxf);

	for (int i=0; i<arr.GetSize(); i++)
	{
		if (arr[i])
		{
			delete (CFeature*)arr[i];
		}
	}
}

void CDxfWrite::ObjectsToDxf(CPtrArray& arrFtrs, CPtrArray &arrLayer, LPCTSTR strDxf)
{
	m_bExportDWG = FALSE;

	ObjectsToDxf0(arrFtrs,arrLayer,strDxf);
	
	return;
}



void CDxfWrite::ObjectsToDwg(CPtrArray& arrFtrs, CPtrArray &arrLayer, LPCTSTR strDxf)
{
	m_bExportDWG = TRUE;

	ObjectsToDxf0(arrFtrs,arrLayer,strDxf);
	
	return;
}


void CDxfWrite::ObjectsToDxf0(CPtrArray& arrFtrs, CPtrArray &arrLayer, LPCTSTR strDxf)
{
	int num = arrFtrs.GetSize();	
	if (num <= 0)  return;

	OpenWrite(strDxf);

	BOOL bUseDSLayer = (num==arrLayer.GetSize());
	
	CString docName = strDxf;
	int findpos = docName.ReverseFind('\\');
	if( findpos>=0 )docName = docName.Mid(findpos+1);
	findpos = docName.ReverseFind('.');
	if( findpos>=0 )docName = docName.Left(findpos);
	
	NormalizeName(docName);
	
	if( docName.GetLength()>10 )docName = docName.Left(10);
	
	m_docName = docName;
	
	AddDefaultLayer_dxf();	
	
	GProgressStart(arrFtrs.GetSize());
	
	for(int i=0; i<arrFtrs.GetSize(); i++)
	{
		GProgressStep();

		CFeature *pFtr = (CFeature*)arrFtrs.GetAt(i);
		
		CString strLayName;
		if( bUseDSLayer )
		{
			CFtrLayer *pLayer = (CFtrLayer*)arrLayer[i];
			SaveFtrLayer(pLayer);		
			m_pCurFdbLayer = pLayer;
			strLayName = pLayer->GetName();

			// 如果基线和符号需要放在不同层
			if (m_dlgDxfExport.m_bDifferBaselineSym)
			{
				SaveLayer(GetExportedLayerName(strLayName,-1)+StrFromResID(IDS_LAYERNAME_SYM),255,0);
			}
		}
		else
		{
			strLayName = pFtr->GetRuntimeClass()->m_lpszClassName;
			strLayName.Replace("class","");
			strLayName.Replace("Geo","");
			SaveLayer(strLayName,255,0);
		}

		SaveFeature(pFtr,FALSE,strLayName);
	}
	
	CloseWrite();
	
	GProgressEnd();
	
	return;
}

void CDxfWrite::SetAsExportSymbols()
{
	m_dlgDxfExport.m_bUseIndex = FALSE;
	m_dlgDxfExport.m_bAddAnnot = TRUE;
	m_dlgDxfExport.m_bMapFrame = FALSE;
	m_dlgDxfExport.m_bLinearize = TRUE;
	m_dlgDxfExport.m_bLines = FALSE;
	m_dlgDxfExport.m_bLineSym = TRUE;
	m_dlgDxfExport.m_bPoint = FALSE;
	m_dlgDxfExport.m_bPointSym = TRUE;
	m_dlgDxfExport.m_bSepPara = TRUE;
	m_dlgDxfExport.m_bParaCenline = FALSE;
	m_dlgDxfExport.m_bSurface = FALSE;
	m_dlgDxfExport.m_bSurfaceSym = TRUE;
	m_dlgDxfExport.m_bText = FALSE;
	m_dlgDxfExport.m_b3DLines = TRUE;
	m_dlgDxfExport.m_bDirPtAsLine = FALSE;
	m_dlgDxfExport.m_bClosedCurve = FALSE;
	m_dlgDxfExport.m_bTextSym = TRUE;
	m_dlgDxfExport.m_bDifferBaselineSym = FALSE;
}



BOOL CDxfWrite::SilenceExportDxf(CDlgDoc *pDoc,CString pathName)
{
	if (!pDoc) m_pDS = NULL;
	else
		m_pDS = pDoc->GetDlgDataSource();
		
	if(!m_pDS)
	{
		CString strText;
		strText.LoadString(IDS_DATASOURCE_EMPTY);
		AfxMessageBox(strText);
		return FALSE;
	}
	
	OpenWrite(pathName);

	CString docName = pDoc->GetPathName();
	docName = pathName;
	int findpos = docName.ReverseFind('\\');
	if( findpos>=0 )docName = docName.Mid(findpos+1);
	findpos = docName.ReverseFind('.');
	if( findpos>=0 )docName = docName.Left(findpos);

	NormalizeName(docName);

	if( docName.GetLength()>10 )docName = docName.Left(10);

	m_docName = docName;
	
	AddDefaultLayer_dxf();	
	
	int lObjSum = 0;
	for(int i=0; i<m_pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = m_pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())continue;
		lObjSum += pLayer->GetObjectCount();
	}
	
	GProgressStart(lObjSum);
	
	for(i=0; i<m_pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = m_pDS->GetFtrLayerByIndex(i);

		if (!pLayer ||!pLayer->IsVisible()|| pLayer->GetEditableObjsCount() <= 0)
		{
			if (pLayer)
			{
				GProgressStep(pLayer->GetObjectCount());
			}
			continue;
		}

		if (!SaveFtrLayer(pLayer))
			continue;

		// 如果基线和符号需要放在不同层
		if (m_dlgDxfExport.m_bDifferBaselineSym)
		{
			SaveLayer(GetExportedLayerName(pLayer->GetName(),-1)+StrFromResID(IDS_LAYERNAME_SYM),255,0);
		}
		
		for(int j=0; j<pLayer->GetObjectCount(); j++)
		{
			GProgressStep();
			
			CFeature *pFt = pLayer->GetObject(j);
			SaveFeature(pFt,TRUE);
		}
	}
	
	
	//导出图廓
	if( 0/*m_dlgDxfExport.m_bMapFrame*/ )
	{
		COLORREF color = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_BOUNDCOLOR,255);
		SaveLayer(StrFromResID(IDS_DOC_EXPORT_BOUND),color,2);

		CGeoCurve curve;
		PT_3DEX pts[4];
		memset(pts,0,sizeof(pts));
		
		PT_3D ptsBound[4];
		m_pDS->GetBound(ptsBound,NULL,NULL);
		for( int i=0; i<4; i++)
		{
			COPY_3DPT(pts[i],ptsBound[i]);
			pts[i].wid = 2.0;
			pts[i].pencode = penLine;
		}

		curve.CreateShape(pts,4);
		curve.SetColor(color);
		curve.EnableClose(TRUE);
		SaveCurveOrSurface(&curve,StrFromResID(IDS_DOC_EXPORT_BOUND),NULL,2.0,0);
	}
	
	CloseWrite();
	
	GProgressEnd();
	
	return TRUE;


}


//工具菜单下的批量导出
BOOL CDxfWrite::OnExportDxf_Batch()
{
	m_dlgDxfExport.m_Type = 0;
	if(m_dlgDxfExport.DoModal() != IDOK) 
		return FALSE;

	int nsize = m_dlgDxfExport.m_arrFdbStr.GetSize();
	if (nsize<=0)
	{
		return FALSE;
	}
	CString str;
	str.Format(StrFromResID(IDS_TOTAL_FDB_NUMBRE),nsize);
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)str);

	CDataQueryEx *pDQ = new CDataQueryEx();	

	int i, j, k;
	for (i = 0;i<nsize;i++)
	{
		CAccessModify *pAccess = new CSQLiteAccess();
		if( !pAccess )
		{
			continue;
		}
		CString FdbFileName = m_dlgDxfExport.m_arrFdbStr.GetAt(i);
		if( !pAccess->Attach(FdbFileName) )
		{
			CString str = FdbFileName;
			str = str+_T(" ")+StrFromResID(IDS_FILE_OPEN_ERR);
			AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)str);
			delete pAccess;
			continue;
		}
		
		CDlgDataSource *pDataSource = new CDlgDataSource(pDQ);	
		if( !pDataSource )
		{
			
			CString str = FdbFileName;
			str = str+_T(" ")+StrFromResID(IDS_INVALID_FILE);
			AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)str);
			delete pAccess;
			continue;
		}

		pDataSource->SetAccessObject(pAccess);
		pAccess->ReadDataSourceInfo(pDataSource);
		//设置缺省图元库和线型库
		SetDataSourceScale(pDataSource->GetScale());
		CConfigLibManager *pCfgLibManager = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
		ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(pDataSource->GetScale());
		SetCellDefLib(pCfgLibManager->GetCellDefLib(pDataSource->GetScale()));
		SetBaseLineTypeLib(pCfgLibManager->GetBaseLineTypeLib(pDataSource->GetScale()));

		pAccess->BatchUpdateBegin();	
		pDataSource->LoadAll(NULL);
		pAccess->BatchUpdateEnd();

		m_pDS = pDataSource;

		CString str_tmp;
		str_tmp.Format("(%d\\%d)",i+1,nsize);

		CString str = StrFromResID(IDS_PROCESS_FILE)+str_tmp + _T(": ") + FdbFileName + _T("\n");
		AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)str);

		CString DxfFileName =  m_dlgDxfExport.m_arrDxfStr.GetAt(i);
	
		CString docName = DxfFileName;
		int findpos = docName.ReverseFind('\\');
		if( findpos>=0 )docName = docName.Mid(findpos+1);
		findpos = docName.ReverseFind('.');
		if( findpos>=0 )docName = docName.Left(findpos);
		
		LPTSTR pchData = docName.GetBuffer(docName.GetLength());
		NormalizeName(pchData);
		docName.ReleaseBuffer();
		
		if( docName.GetLength()>10 )docName = docName.Left(10);
		
		m_docName = docName;
		
		CString pathName = DxfFileName;

		// 判断导出dxf还是dwg
		if (pathName.Right(4).CompareNoCase(".dwg") == 0)
			m_bExportDWG = TRUE;
		else
			m_bExportDWG = FALSE;

		OpenWrite(pathName);
		
		LAYER Layer;
		
		//配置线型
		AddDefaultLayer_dxf();
		
		int lObjSum = 0;
		for(j=0; j<m_pDS->GetFtrLayerCount(); j++)
		{
			CFtrLayer *pLayer = m_pDS->GetFtrLayerByIndex(j);
			if (!pLayer||!pLayer->IsVisible())  continue;
			lObjSum += pLayer->GetObjectCount();
		}
		
		GProgressStart(lObjSum);
		
		for(j=0; j<m_pDS->GetFtrLayerCount(); j++)
		{
			CFtrLayer *pLayer = m_pDS->GetFtrLayerByIndex(j);
			
			if (!pLayer ||!pLayer->IsVisible()|| pLayer->GetEditableObjsCount() <= 0)
			{
				if (pLayer)
				{
					GProgressStep(pLayer->GetObjectCount());
				}
				
				continue;
			}
			
			if (!SaveFtrLayer(pLayer))
				continue;

			// 如果基线和符号需要放在不同层
			if (m_dlgDxfExport.m_bDifferBaselineSym)
			{
				SaveLayer(GetExportedLayerName(pLayer->GetName(),-1)+StrFromResID(IDS_LAYERNAME_SYM),255,0);
			}
			
			for(k=0; k<pLayer->GetObjectCount(); k++)
			{
				GProgressStep();
				
				CFeature *pFt = pLayer->GetObject(k);
				SaveFeature(pFt);
			}
		}
		
		//导出图廓
		if( m_dlgDxfExport.m_bMapFrame )
		{
			COLORREF color = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_BOUNDCOLOR,255);
			SaveLayer(StrFromResID(IDS_DOC_EXPORT_BOUND),color,2);

			CGeoCurve curve;
			PT_3DEX pts[4];
			memset(pts,0,sizeof(pts));
			
			PT_3D ptsBound[4];
			m_pDS->GetBound(ptsBound,NULL,NULL);
			for( j=0; j<4; j++)
			{
				COPY_3DPT(pts[i],ptsBound[j]);
				pts[j].wid = 2.0;
				pts[j].pencode = penLine;
			}

			curve.CreateShape(pts,4);
			curve.SetColor(color);
			curve.EnableClose(TRUE);
			SaveCurveOrSurface(&curve,StrFromResID(IDS_DOC_EXPORT_BOUND),NULL,2.0,0);		
		}
		
		CloseWrite();
	
		GProgressEnd();
	
		delete pDataSource;
		pDataSource = NULL;
	
	}

	delete pDQ;

	CString str_end = StrFromResID(IDS_PROCESS_END) ;
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)str_end);
	return TRUE;
}



//工作区导出菜单下的DXF/DWG导出
BOOL CDxfWrite::OnExportDxf()
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (!pDoc) 
		m_pDS = NULL;
	else
		m_pDS = pDoc->GetDlgDataSource();
	
	if (!m_pDS)
	{
		CString strText;
		strText.LoadString(IDS_DATASOURCE_EMPTY);
		AfxMessageBox(strText);
		return FALSE;
	}

	m_dlgDxfExport.m_strFdbFile = pDoc->GetPathName();
	m_dlgDxfExport.m_strDxfFile = m_dlgDxfExport.m_strFdbFile.Left(m_dlgDxfExport.m_strFdbFile.GetLength()-4) + ".dxf";
	m_dlgDxfExport.m_Type = CDlgExportDxf::EXPORTEXF;
	
	if(m_dlgDxfExport.DoModal() != IDOK) 
		return FALSE;

	CString docName = pDoc->GetPathName();
	docName = m_dlgDxfExport.m_strDxfFile;
	int findpos = docName.ReverseFind('\\');
	if( findpos>=0 )docName = docName.Mid(findpos+1);
	findpos = docName.ReverseFind('.');
	if( findpos>=0 )docName = docName.Left(findpos);

	NormalizeName(docName);

	if( docName.GetLength()>10 )docName = docName.Left(10);
	
	m_docName = docName;

	CString pathName = m_dlgDxfExport.m_strDxfFile;

	// 判断导出dxf还是dwg
	if (pathName.Right(4).CompareNoCase(".dwg") == 0)
	{
		m_bExportDWG = TRUE;
	}

	if (!OpenWrite(pathName)) return FALSE;

	AddDefaultLayer_dxf();
		
	int lObjSum = 0;
	for(int i=0; i<m_pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = m_pDS->GetFtrLayerByIndex(i);
		if (!pLayer||!pLayer->IsVisible())  continue;
		lObjSum += pLayer->GetObjectCount();
	}
	
	GProgressStart(lObjSum);

	for(i=0; i<m_pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = m_pDS->GetFtrLayerByIndex(i);
		
		if (!pLayer ||!pLayer->IsVisible()|| pLayer->GetEditableObjsCount() <= 0)
		{
			if (pLayer)
			{
				GProgressStep(pLayer->GetObjectCount());
			}
			
			continue;
		}

		if (!SaveFtrLayer(pLayer))
			continue;

		// 如果基线和符号需要放在不同层
		if (m_dlgDxfExport.m_bDifferBaselineSym)
		{
			SaveLayer(GetExportedLayerName(pLayer->GetName(),-1)+StrFromResID(IDS_LAYERNAME_SYM),255,0);
		}

		for(int j=0; j<pLayer->GetObjectCount(); j++)
		{
			GProgressStep();

			CFeature *pFt = pLayer->GetObject(j);
			SaveFeature(pFt);
		}
	}	

	//导出图廓
	if( m_dlgDxfExport.m_bMapFrame )
	{
		COLORREF color = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_BOUNDCOLOR,255);
		SaveLayer(StrFromResID(IDS_DOC_EXPORT_BOUND),color,2);

		CGeoCurve curve;
		PT_3DEX pts[4];
		memset(pts,0,sizeof(pts));
		
		PT_3D ptsBound[4];
		m_pDS->GetBound(ptsBound,NULL,NULL);
		for( int i=0; i<4; i++)
		{
			COPY_3DPT(pts[i],ptsBound[i]);
			pts[i].wid = 2.0;
			pts[i].pencode = penLine;
		}

		curve.CreateShape(pts,4);
		curve.SetColor(color);
		curve.EnableClose(TRUE);
		SaveCurveOrSurface(&curve,StrFromResID(IDS_DOC_EXPORT_BOUND),NULL,2.0,0);		
	}

	CloseWrite();

	GProgressEnd();

	return TRUE;

}




void CDxfWrite::SavePoint(CGeometry *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR blkName, LPCTSTR cassCode)
{
	long color = ConvertColor(pGeoObj->GetColor(),NULL);

	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeoObj->GetShape(arrPts);

	if( blkName!=NULL && strlen(blkName)>0 )
	{
		CFeature ftr;
		ftr.SetGeometry(pGeoObj->Clone());

		CSymbolArray arr;
		SetBlock(&ftr,dxfLayerName,arr,FALSE,blkName,NULL);

		return;
	}

	if( !m_bExportDWG )
	{
		m_DxfDraw.SetLayer(dxfLayerName);
		if (pGeoObj->GetColor() == FTRCOLOR_BYLAYER)
		{
			m_DxfDraw.SetColor(256);
		}
		else
		{
			short col = ConverttoIndexOfCAD(pGeoObj->GetColor());
			if (col == 0) col = 255;
			m_DxfDraw.SetColor(col);
		}
		
		double ang = 0;
		if (pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		{
			ang = ((CGeoPoint*)pGeoObj)->m_lfAngle;
		}
		
		m_DxfDraw.Point(arrPts[0].x,arrPts[0].y,arrPts[0].z,ang);
	}
	else if( m_pDb )
	{
		VARIANT varPts;
		VariantInit(&varPts);

		IAcadPoint *pPoint = NULL;
		Create3DPointArray(arrPts.GetData(),1,&varPts,sizeof(PT_3DEX));					
		m_pMS->AddPoint(varPts,&pPoint);
		
		if( pPoint )
		{
			if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
			{
				pPoint->put_TrueColor(TrueColor(color));
			}
			else
			{
				pPoint->put_color(ACAD_COLOR(color));
			}
			
			pPoint->put_Layer(CharToWChar(dxfLayerName));
			WriteCassCode(pPoint,cassCode);

			pPoint->Release();
		}
		
		ArrayClear(&varPts);
	}
	
}


void CDxfWrite::SaveText(CGeoText *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR cassCode)
{	
	CGeoText *pText = pGeoObj;

	TEXT_SETTINGS0 setting;
	pText->GetSettings(&setting);

	//dxf/dwg不支持的文字排列方式，就打散这个文字
	if( setting.nPlaceType==byLineV || setting.nPlaceType==byLineGridV || setting.nPlaceType==byLineGridH || setting.nPlaceType==mutiPt )
	{
		GrBuffer buf;
		pText->SeparateText(&buf,m_pDS->GetSymbolDrawScale());
		
		CGeoArray arrPGeos;
		ConvertGrBufferToGeos(&buf,m_pDS->GetSymbolDrawScale(),arrPGeos);
		
		CString strNewSymName = "*";
		
		for (int i=arrPGeos.GetSize()-1; i>=0; i--)
		{
			CGeometry *pGeo = arrPGeos[i];
			pGeo->SetSymbolName(strNewSymName);

			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				SaveText_core((CGeoText*)pGeo,dxfLayerName,cassCode);
			}
			delete pGeo;
		}
	}
	else
	{
		SaveText_core(pGeoObj,dxfLayerName,cassCode);
	}
}


void CDxfWrite::SaveText_core(CGeoText *pGeoObj, LPCTSTR dxfLayerName, LPCTSTR cassCode)
{	
	long color = ConvertColor(pGeoObj->GetColor(),m_pCurFdbLayer);

	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeoObj->GetShape(arrPts);
	if( arrPts.GetSize()<=0 )
		return;

	CGeoText *pText = pGeoObj;
	
	TEXT_SETTINGS0 setting;
	pText->GetSettings(&setting);

	double angle = pText->GetTextAngle();

	TextStyle style;
	style = GetUsedTextStyles()->GetTextStyleByName(setting.strFontName);
	if( style.IsValid() )
		strcpy(style.name,setting.strFontName);
	else
		strcpy(style.font,setting.strFontName);

	style.fWidScale = setting.fWidScale;
	style.nInclineType = setting.nInclineType;
	style.fInclinedAngle = setting.fInclineAngle;
	
	if( style.nInclineType==SHRUGN )
		setting.fInclineAngle = 0;
	else if( style.nInclineType==SHRUGL )
	{
		setting.fInclineAngle = 360-setting.fInclineAngle;
		style.nInclineType = SHRUGR;
	}	

	CString styleName = AddTextStyle(style.name,style.font,style.fWidScale,style.nInclineType,style.fInclinedAngle);

	if( !m_bExportDWG )
	{
		if (pGeoObj->GetColor() == FTRCOLOR_BYLAYER)
		{
			m_DxfDraw.SetColor(256);
		}
		else
		{
			short col = ConverttoIndexOfCAD(pGeoObj->GetColor());
			if (col == 0) col = 255;
			m_DxfDraw.SetColor(col);
		}
		
		// 过滤换行符
		CString strText = pText->GetText();
		
		RemoveChar(strText,"\r\n\t,，");

		if( setting.nPlaceType>=byLineH )
			setting.nAlignment = ((TAH_CLEAR&setting.nAlignment)|TAH_LEFT);
		
		int nJustification = ConvertAlignmentAndJustification(setting.nAlignment,TRUE);

		if( !styleName.IsEmpty() )
			m_DxfDraw.SetTextStyle(styleName);
		else
			m_DxfDraw.SetTextStyle("Standard");

		m_DxfDraw.Text(strText,arrPts[0].x,arrPts[0].y,arrPts[0].x,arrPts[0].y,arrPts[0].z,
			setting.fHeight*cadTextScale*m_pDS->GetSymbolDrawScale(),nJustification,angle,setting.fWidScale,setting.fInclineAngle);
	}
	else if( m_pDb )
	{

		VARIANT varPts;
		VariantInit(&varPts);	
		
		// 过滤换行符
		CString strText = pText->GetText();
		RemoveChar(strText,"\r\n\t,，");
		
		Create3DPointArray(&arrPts[0],1,&varPts,sizeof(PT_3DEX));
		
		IAcadText *pCadText;
		_bstr_t tmpstr = _bstr_t((const char*)(LPCTSTR)strText);
		m_pMS->AddText(tmpstr,varPts,_variant_t(setting.fHeight*cadTextScale*m_pDS->GetSymbolDrawScale()),&pCadText);

		if( setting.nPlaceType>=byLineH )
			setting.nAlignment = ((TAH_CLEAR&setting.nAlignment)|TAH_LEFT);

		if( pCadText )
		{
			pCadText->put_Alignment((AcAlignment)ConvertAlignment_DWG(setting.nAlignment,TRUE));
			pCadText->put_TextAlignmentPoint(varPts);
			pCadText->put_ScaleFactor(setting.fWidScale);
			pCadText->put_Rotation(angle);
			if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
			{
				pCadText->put_TrueColor(TrueColor(color));
			}
			else
			{
				pCadText->put_color(ACAD_COLOR(color));
			}
			pCadText->put_ObliqueAngle(setting.fInclineAngle);

			if( !styleName.IsEmpty() )
				pCadText->put_StyleName(CharToWChar(styleName));
			
			pCadText->put_Layer(CharToWChar(dxfLayerName));

			WriteCassCode(pCadText,cassCode);

			pCadText->Release();
		}
		
		ArrayClear(&varPts);
	}	
}

void CDxfWrite::SaveCurveOrSurface(CGeometry *pGeoObj, LPCTSTR dxfLayName, LPCTSTR linetype_name, float wid, float thickness, LPCTSTR cassCode)
{
	if (!pGeoObj)  return;
	CGeoCurveBase *pCurve = NULL;
	if ( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )	
		pCurve = (CGeoCurveBase*)pGeoObj;
	else
		return;

	if( pGeoObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) && ((CGeoSurface*)pGeoObj)->IsFillColor() )
	{
		SaveHatch(m_pMS,pGeoObj,dxfLayName,linetype_name,wid,thickness);
		return;
	}

	long color = ConvertColor(pGeoObj->GetColor(),NULL);

	if( !linetype_name || strlen(linetype_name)<=0 )
		linetype_name = "Continuous";
	else
	{
		AddSolidLinetype(linetype_name);
	}

	if( wid==-1 )
	{
		if ( pCurve->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )	
			wid = ((CGeoCurve*)pCurve)->m_fLineWidth;
		else
			wid = ((CGeoSurface*)pCurve)->m_fLineWidth;

		if( wid>0 )
			wid *= m_pDS->GetSymbolDrawScale();
		else
			wid = 0;
	}

	if( thickness==-1 )
		thickness = wid;

	ENTITYHEADER header;
	m_DxfDraw.GetCurEntityHeader(&header);

	CComBSTR wstrName;

	if( !m_bExportDWG )
	{
		m_DxfDraw.SetLayer(dxfLayName);
		m_DxfDraw.SetLineType(linetype_name);
	}
	else
	{
		wstrName = CharToWChar(linetype_name);
	}
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeoObj->GetShape(arrPts);
	
/*
	if( 面 ）
	{
		总是带闭合属性导出 
	}
	if( 线 ）
	{
		if( closed )
		{
			if( 闭合属性选乡勾上 ）
			{
				带闭合属性导出
			}
			else
			{
				补充一个尾点（等于首点）
			}
		}
		else
		{
			正常导出 
		}
	}
	if( 面/线 ）
	{
		if( 等高 )
		{
			仍然用二唯导出，只是给一个标高
		}
		else
		{
			if( 三维选项 )
			{
				三维导出
			}
			else
			{
				二维导出，标高为平均高程
			}
		}
	}					
*/
	
	int num = arrPts.GetSize();
	if( num>0 )
	{
		//缺省输出三维地物	
		int flag = 8;
		
		PT_3DEX *expts = (PT_3DEX*)arrPts.GetData();
		
		//判断是否共面和是否闭合
		BOOL bSurface = FALSE, bClose = FALSE;
		for( int i=0; i<num-1; i++)
		{
			if( fabs(expts[i].z-expts[i+1].z)>=1e-4 )break;
		}
		
		bSurface = (i>=num-1);
		
		PT_3DEX pt0 = expts[0], pt1 = expts[num-1];
		if( fabs(pt0.x-pt1.x)<1e-4 && fabs(pt0.y-pt1.y)<1e-4 )
			bClose = TRUE;
		
		// 闭合属性
		if (pCurve != NULL)
		{
			if( bClose && pCurve->IsClosed()) 
			{
				if (m_dlgDxfExport.m_bClosedCurve) 
				{
					flag |= 1;
					// 去掉尾点
					arrPts.RemoveAt(num-1);
					expts = (PT_3DEX*)arrPts.GetData();
					num = arrPts.GetSize();
				}
				/*else
				{
					//补充一个尾点
					arrPts.Add(arrPts.GetAt(0));
					expts = (PT_3DEX*)arrPts.GetData();
					num = arrPts.GetSize();
					
				}*/
			}
		}

		if( !m_dlgDxfExport.m_b3DLines )flag = (flag&1);

		if (!bSurface && !m_dlgDxfExport.m_b3DLines)
		{
			double z1 = 0;
			
			if( num>1 )
			{
				for( i=0; i<num; i++)z1 += expts[i].z;
				z1 = z1/num;
			}
			
			for( i=0; i<num; i++)expts[i].z = z1;
		}
		//三维选项（等高）
/*		if (bSurface)
		{
			flag = (flag&1);
			double z1 = 0;
			
			if( num>1 )
			{
				for( i=0; i<num; i++)z1 += expts[i].z;
				z1 = z1/num;
			}
			
			for( i=0; i<num; i++)expts[i].z = z1;
		}
		else
		{
			if (m_dlgDxfExport.m_b3DLines)
			{
				flag |= 8;
			}
			else
			{
				flag = (flag&1);
				double z1 = 0;
				
				if( num>1 )
				{
					for( i=0; i<num; i++)z1 += expts[i].z;
					z1 = z1/num;
				}
				
				
				for( i=0; i<num; i++)expts[i].z = z1;
				
			}
			
		}
*/		
		//判断是否含有圆弧
		for( i=0; i<num && expts[i].pencode==penArc; i++);
		
		BOOL bOK = FALSE;
		//全为圆弧而且共面
		if( i>=num && bSurface && (num==3||(num==4&&bClose)) )
		{
			PT_2D pt2ds[3],c;
			double r,ang[3];
			int blockwise;
			for( i=0; i<3; i++ )
			{
				COPY_2DPT(pt2ds[i],expts[i]);
			}
			
			if( CalcArcParamFrom3P(pt2ds,&c,&r,ang,&blockwise) )
			{
				VARIANT varPts;
				PT_3D pt;
				pt.x = c.x;
				pt.y = c.y;
				pt.z = expts[0].z;
				Create3DPointArray(&pt,1,&varPts);	

				if( bClose )
				{
					if (!m_bExportDWG)
					{
						m_DxfDraw.SetColor(color);

						m_DxfDraw.Circle(c.x,c.y,expts[0].z,r);
					}
					else if( m_pDb )
					{	
						IAcadCircle *pCircle = NULL;
						m_pMS->AddCircle(varPts,r,&pCircle);
						if( pCircle )
						{
							pCircle->put_Linetype(wstrName);
							
							if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
							{
								pCircle->put_TrueColor(TrueColor(color));
							}
							else
							{
								pCircle->put_color(ACAD_COLOR(color));
							}
							
							pCircle->put_Layer(CharToWChar(dxfLayName));

							WriteCassCode(pCircle,cassCode);

							pCircle->Release();
						}
					}					
					
				}
				else
				{
					if (!m_bExportDWG)
					{
						m_DxfDraw.SetLayer(dxfLayName);
						if( blockwise )
						{
							m_DxfDraw.SetColor(color);
							m_DxfDraw.Arc(c.x,c.y,expts[0].z,r,
								ang[2]*180/PI,ang[0]*180/PI);
							
						}
						else
						{
							m_DxfDraw.SetColor(color);
							m_DxfDraw.Arc(c.x,c.y,expts[0].z,r,
								ang[0]*180/PI,ang[2]*180/PI);
							
						}
					}
					else if( m_pDb )
					{

						IAcadArc *pArc = NULL;
						if( blockwise )
						{							
							m_pMS->AddArc(varPts,r,ang[2]/**180/PI*/,ang[0]/**180/PI*/,&pArc);
						}
						else
						{
							m_pMS->AddArc(varPts,r,ang[0]/**180/PI*/,ang[2]/**180/PI*/,&pArc);
						}
						
						if( pArc )
						{
							if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
							{
								pArc->put_TrueColor(TrueColor(color));
							}
							else
							{
								pArc->put_color(ACAD_COLOR(color));
							}
							pArc->put_Linetype(wstrName);
							
							pArc->put_Layer(CharToWChar(dxfLayName));

							WriteCassCode(pArc,cassCode);

							pArc->Release();
						}
					}
					
				}

				ArrayClear(&varPts);
				
				bOK = TRUE;
			}
		}
		
		if( !bOK )
		{	
			if (!m_bExportDWG)
			{
				PENTVERTEX pts = new ENTVERTEX[arrPts.GetSize()];
				ZeroMemory(pts,sizeof(ENTVERTEX)*arrPts.GetSize());
				for( i=0; i<arrPts.GetSize(); i++ )
				{
					COPY_3DPT(pts[i].Point,arrPts[i]);

					if( wid!=0 )
					{
						pts[i].StartWidth = wid;
						pts[i].EndWidth = wid;
					}
				}
				
				m_DxfDraw.SetThickness(thickness);
				if (pGeoObj->GetColor() == FTRCOLOR_BYLAYER)
				{
					m_DxfDraw.SetColor(256);
				}
				else
				{
					short col = ConverttoIndexOfCAD(pGeoObj->GetColor());
					if (col == 0) col = 255;
					m_DxfDraw.SetColor(col);
				}
				m_DxfDraw.PolyLine(pts,arrPts.GetSize(),flag|128);  //128，线型生成标志
				if (pts)
				{
					delete[] pts;
					pts = NULL;
				}

				m_DxfDraw.SetThickness(header.Thickness);
			}
			else if( m_pDb )
			{
				VARIANT varPts;

				IAcadEntity *pEntity = NULL;
				// 三维输出
				if (flag&8)
				{
					IAcad3DPolyline *pPoly = NULL;
					Create3DPointArray(arrPts.GetData(),arrPts.GetSize(),&varPts,sizeof(PT_3DEX));
					
					m_pMS->Add3DPoly(varPts,&pPoly);
					
					if( pPoly )
					{
						if (flag&1)
						{
							pPoly->put_Closed(VARIANT_TRUE);
						}
					}

					pEntity = pPoly;

					ArrayClear(&varPts);
				}
				else
				{
					IAcadLWPolyline *pPoly = NULL;
					Create3DPointArray(arrPts.GetData(),arrPts.GetSize(),&varPts,sizeof(PT_3DEX),FALSE);
			
					m_pMS->AddLightWeightPolyline(varPts,&pPoly);

					if( pPoly )
					{
						if (flag&1)
						{
							pPoly->put_Closed(VARIANT_TRUE);
						}

						pPoly->put_Elevation(arrPts[0].z);
						pPoly->put_LinetypeGeneration(VARIANT_TRUE);

						if( wid!=0 )
						{
							for( i=0; i<arrPts.GetSize(); i++)
							{
								pPoly->SetWidth(i,wid,wid);
							}

							pPoly->put_ConstantWidth(wid);
						}
						pPoly->put_Thickness(thickness);
					}

					pEntity = pPoly;

					ArrayClear(&varPts);
				}	
				
				if (!pEntity) return;

				if(m_bExportDWG && m_dlgDxfExport.m_bUseRGBTrueColor)
				{
					pEntity->put_TrueColor(TrueColor(color));
				}
				else
				{
					pEntity->put_color(ACAD_COLOR(color));
				}
				pEntity->put_Linetype(wstrName);

				pEntity->put_Layer(CharToWChar(dxfLayName));

				WriteCassCode(pEntity,cassCode);
				
				pEntity->Release();
			}
			
		}
	}
}

BOOL CDxfWrite::IsSolidLineSymbol(CSymbolArray* arrPSyms)
{//判断符号是否唯一且为实线
	int num = arrPSyms->GetSize();
	if(1!=num) return FALSE;

	CDashLinetype* pDashLine = NULL;
	if(	arrPSyms->GetAt(0)->GetType() == SYMTYPE_DASHLINETYPE )
	{
		pDashLine = (CDashLinetype*)(arrPSyms->GetAt(0));
	}
	if( !pDashLine ) return FALSE;

	if( fabs(pDashLine->m_fBaseOffset) > 1e-6 ) return FALSE;		//偏移必须为0

	CBaseLineTypeLib* Lib = GetBaseLineTypeLib();
	BaseLineType blType = Lib->GetBaseLineType( pDashLine->GetName() );

	if( blType.IsSolidLine() )
		return TRUE;
	else
		return FALSE;
}