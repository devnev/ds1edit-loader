/***************************************************************************
 *   Copyright (C) 2007 by Mark Nevill                                     *
 *   mark.nevill@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef EDITORCONFIG_H_INCLUDED
#define EDITORCONFIG_H_INCLUDED

#include <QDialog>

//class QGroupBox;
class QRadioButton;
//class QLabel;
class QLineEdit;
//class QPushButton;
class QCheckBox;
class QComboBox;
//class QSpacerItem;
class QDialogButtonBox;

#include "iniparser.h"

class EditorConfigDialog : public QDialog
{
	Q_OBJECT

private:
	//QGroupBox* commentsBox;
	//QHBoxLayout* commentsLayout;
	QRadioButton* removeComments;
	QRadioButton* resetComments;
	QRadioButton* preserveComments;

	//QGroupBox* pathsBox;
	//QGridLayout* pathsLayout;
	enum Path { d2charPath, d2dataPath, d2expPath, patch_d2Path, mod_dirPath, lastPath=mod_dirPath };
	static const int pathSize = lastPath+1;
	/*struct {
		//QLabel* name;
		QLineEdit* path;
		//QPushButton* browse;
	} paths[pathSize];*/
	QLineEdit* paths[pathSize];

	//QGroupBox* screenBox;
	//QGridLayout* screenLayout;
	QCheckBox* useFullscreen;
	/*QLabel* screenWidth;
	QLabel* screenHeight;
	QLabel* screenDepth;
	QLabel* screenRefresh;*/
	QLineEdit* screenWidth;
	QLineEdit* screenHeight;
	QComboBox* screenDepth;
	QLineEdit* screenRefresh;
	QComboBox* gammaCorrection;

	//QGroupBox* speedsBox;
	//QHBoxLayout* speedsLayout;
	//QGridLayout* scrollSpeedsLayout;
	enum Scrolling { keyboardScroll, mouseScroll, tileeditScroll, lastScroll=tileeditScroll };
	static const int scrollSize = lastScroll + 1;
	QLineEdit* scrollSpeeds[scrollSize][2];
	//QVBoxLayout* editScrollLayout;
	QLineEdit* editScrollSpeed;
	//QVBoxLayout* mouseSpeedLayout;
	//QHBoxLayout perMouseSpeedLayout[2];
	QLineEdit* mouseSpeeds[2];
	QCheckBox* lowZoomedScrollSpeed;

	//QGroupBox* miscBox;
	//QHBoxLayout* miscLayout;
	//QVBoxLayout* miscLayoutLeft;
	QCheckBox* allowHigherType2;
	QCheckBox* alwaysMaxLayerCount;
	QCheckBox* stretchSpritesOnZoom;
	QCheckBox* minimizeSavedDs1;
	QComboBox* zoomAfterCentering;
	//QVBoxLayout* miscLayoutRight;
	QCheckBox* mouseScrollsMain;
	QCheckBox* keybScrollsMain;
	QLineEdit* type1TableSize;
	QLineEdit* type2TableSize;
	//QPushButton* revertTables;
	QCheckBox* enableWorkspace;

	QDialogButtonBox* buttonBox;

	void browseForMpq(QLineEdit* dest, const QString& name);
	void setupWidgets();
	void loadEditorConfig();
	void syncData(ini::AttributeMap& data, bool saveDialogData);
	void setupCommentWidgets(QWidget* parent);
	void setupPathWidgets(QWidget* parent);
	void setupScreenWidgets(QWidget* parent);
	void setupSpeedWidgets(QWidget* parent);
	void setupMiscWidgets(QWidget* parent);

public:
	EditorConfigDialog(QWidget* parent = 0);
	virtual ~EditorConfigDialog() { }

public slots:
	void restoreDefaults();
	void d2charBrowse() { browseForMpq(paths[d2charPath], "d2char"); }
	void d2dataBrowse() { browseForMpq(paths[d2dataPath], "d2data"); }
	void d2expBrowse() { browseForMpq(paths[d2expPath], "d2exp"); }
	void patch_d2Browse() { browseForMpq(paths[patch_d2Path], "patch_d2"); }
	void mod_dirBrowse();
	void revertTableSizes();

	virtual void accept();
};

#endif //EDITORCONFIG_H_INCLUDED
