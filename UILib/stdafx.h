// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <windows.h>
#include <shellapi.h>
#include <map>
#include "UILib.h"

using namespace UILib;

#define FRIEND(p)	(decltype(this)(p))