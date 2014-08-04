// stdafx.h : 可在此標頭檔中包含標準的系統 Include 檔，
// 或是經常使用卻很少變更的
// 專案專用 Include 檔案
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

struct Rect {
	int x1, y1, x2, y2, w, h;
	// w = x2 - x1,   h = y2 - y1;
};



// TODO:  在此參考您的程式所需要的其他標頭
