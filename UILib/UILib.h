#pragma once

#pragma warning(disable: 4251)

#ifdef UILIB_EXPORTS
#define UILIB_API __declspec(dllexport)
#elif defined(UILIB_IMPORTS)
#define UILIB_API __declspec(dllimport)
#else
#define UILIB_API
#endif

#include <atlimage.h>
#include <atlwin.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

using std::function;
using std::vector;
using std::string;
using std::wstring;

#ifndef MAXINT16
#define MAXINT16	0x7fff
#endif

#include "Imagex.h"
#include "UIDC.h"
#include "UIResPath.h"
#include "UIStream.h"
#include "UIMenu.h"
#include "UIButtonEx.h"
#include "UIMenuButton.h"
#include "UICheckButton.h"
#include "UIRadioButton.h"
#include "UIComboButton.h"
#include "UIStateButton.h"
#include "UICheckBox.h"
#include "UIRadioBox.h"
#include "UITimer.h"
#include "UIAnimation.h"
#include "UIHotKey.h"
#include "UIEdit.h"
#include "UIGif.h"
#include "UIImage.h"
#include "UILabel.h"
#include "UILine.h"
#include "UIProgress.h"
#include "UIPageCtrl.h"
#include "UICarousel.h"
#include "UIScrollView.h"
#include "UISlider.h"
#include "UIScrollBar.h"
#include "UIToolBar.h"
#include "UIWebTabBar.h"
#include "UIRootView.h"
#include "UILoader.h"
#include "UIUtility.h"
