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

#include "editorconfigdialog.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QSpacerItem>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <cassert>
#include "datasync.h"

EditorConfigDialog::EditorConfigDialog(QWidget* parent, bool useTabs)
: QDialog(parent, Qt::MSWindowsFixedSizeDialogHint)
{
	setupWidgets(useTabs);
	setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
	setWindowTitle(tr("Configure Editor"));
}

void EditorConfigDialog::setupWidgets(bool useTabs)
{
	struct WidgetGroupSetupFunctions {
		QString name;
		void (EditorConfigDialog::*setupWidgetFn)(QWidget*);
	};
	WidgetGroupSetupFunctions widgetSetupFunctions[5] = {
		{ tr("&Comments"), &EditorConfigDialog::setupCommentWidgets },
		{ tr("&Paths"), &EditorConfigDialog::setupPathWidgets },
		{ tr("&Screen"), &EditorConfigDialog::setupScreenWidgets },
		{ tr("Sp&eeds"), &EditorConfigDialog::setupSpeedWidgets },
		{ tr("&Misc"), &EditorConfigDialog::setupMiscWidgets }
	};

	QVBoxLayout* topLevelLayout = new QVBoxLayout(this); // top-level layout
	topLevelLayout->setSpacing(6);
	topLevelLayout->setMargin(9);

	if (useTabs)
	{
		QTabWidget* tabWidget = new QTabWidget(this);
		for (int i = 0; i < 5; ++i)
		{
			QWidget* tabPage = new QWidget;
			QVBoxLayout* pageLayout = new QVBoxLayout(tabPage);
			QWidget* groupWidget = new QWidget(tabPage);
			pageLayout->addWidget(groupWidget);
			pageLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
			(this->*(widgetSetupFunctions[i].setupWidgetFn))(groupWidget);
			tabWidget->addTab(tabPage, widgetSetupFunctions[i].name);
		}
		topLevelLayout->addWidget(tabWidget);
	}
	else
	{
		for (int i = 0; i < 5; ++i)
		{
			QGroupBox* groupBox = new QGroupBox(this);
			groupBox->setTitle(widgetSetupFunctions[i].name);
			(this->*(widgetSetupFunctions[i].setupWidgetFn))(groupBox);
			topLevelLayout->addWidget(groupBox);
		}
	}

	buttonBox = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults);
	topLevelLayout->addWidget(buttonBox);
	buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);

	connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), SLOT(restoreDefaults()));
	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

	preserveComments->setEnabled(false);
	loadEditorConfig();
	if (preserveComments->isEnabled())
		preserveComments->setChecked(true);
	else
		resetComments->setChecked(true);

	// Minimize size
	resize(topLevelLayout->minimumSize());
}

void EditorConfigDialog::setupCommentWidgets(QWidget* parent)
{
	QHBoxLayout* hboxLayout = new QHBoxLayout(parent);
	hboxLayout->setSpacing(4);
	hboxLayout->setMargin(4);

	removeComments = new QRadioButton(parent);
	removeComments->setText(tr("Remove comments"));
	hboxLayout->addWidget(removeComments);

	resetComments = new QRadioButton(parent);
	resetComments->setText(tr("Reset comments"));
	hboxLayout->addWidget(resetComments);

	preserveComments = new QRadioButton(parent);
	preserveComments->setText(tr("Preserve comments"));
	hboxLayout->addWidget(preserveComments);
}

void EditorConfigDialog::setupPathWidgets(QWidget* parent)
{
	QGridLayout* gridLayout = new QGridLayout(parent);
	gridLayout->setSpacing(4);
	gridLayout->setMargin(4);

	QLabel* label;
	QPushButton* pushButton;
#define ADDPATHLABEL(labelText, row) \
	label = new QLabel(parent); \
	label->setText(labelText); \
	label->setAlignment(Qt::AlignTrailing|Qt::AlignVCenter); \
	gridLayout->addWidget(label, row, 0, 1, 1); \

#define ADDPATHLINEEDIT(enum, row) \
	gridLayout->addWidget( \
		(paths[ enum ] = new QLineEdit(parent)), \
		row, 1, 1, 1); \

#define ADDPATHBROWSEBUTTON(slot, row) \
	pushButton = new QPushButton(parent); \
	pushButton->setText("..."); \
	pushButton->setMaximumWidth(30); \
	connect(pushButton, SIGNAL(clicked()), SLOT(slot)); \
	gridLayout->addWidget(pushButton, row, 2, 1, 1); \

#define ADDPATH(name, row) \
	ADDPATHLABEL(#name ":", row ); \
	ADDPATHLINEEDIT(name##Path, row ); \
	ADDPATHBROWSEBUTTON(name##Browse(), row ); \

	ADDPATH(d2char, 0);
	ADDPATH(d2data, 1);
	ADDPATH(d2exp, 2);
	ADDPATH(patch_d2, 3);
	ADDPATH(mod_dir, 4);
#undef ADDPATHLABEL
#undef ADDPATHLINEEDIT
#undef ADDPATHBROWSEBUTTON
}

void EditorConfigDialog::setupScreenWidgets(QWidget* parent)
{
	QGridLayout* gridLayout = new QGridLayout(parent);
	gridLayout->setSpacing(4);
	gridLayout->setMargin(4);

	useFullscreen = new QCheckBox(parent);
	useFullscreen->setText(tr("Fullscreen"));
	gridLayout->addWidget(useFullscreen, 0, 0, 2, 1);

	QLabel* label;
#define ADDSCREENLABEL(text, col) \
	label = new QLabel(parent); \
	label->setText(text); \
	label->setAlignment(Qt::AlignCenter); \
	gridLayout->addWidget(label, 0, col, 1, 1); \

	ADDSCREENLABEL("Width:", 1);
	ADDSCREENLABEL("Height:", 2);
	ADDSCREENLABEL("Depth:", 3);
#undef ADDSCREENLABEL

	screenWidth = new QLineEdit(parent);
	screenWidth->setValidator(new QIntValidator(0, 99999, this));
	gridLayout->addWidget(screenWidth, 1, 1, 1, 1);

	screenHeight = new QLineEdit(parent);
	screenHeight->setValidator(new QIntValidator(0, 99999, this));
	gridLayout->addWidget(screenHeight, 1, 2, 1, 1);

	screenDepth = new QComboBox(parent);
	screenDepth->addItems(QStringList()<<"8"<<"15"<<"16"<<"24"<<"32");
	gridLayout->addWidget(screenDepth, 1, 3, 1, 1);

	QHBoxLayout* hboxLayout = new QHBoxLayout;
	hboxLayout->setSpacing(4);
	hboxLayout->setMargin(0);
	label = new QLabel(tr("Gamma Correction:"), parent);
	label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	hboxLayout->addWidget(label);
	gammaCorrection = new QComboBox(parent);
	for (int i = 60; i < 100; i+=2)
		gammaCorrection->addItem("0." + QString::number(i));
	for (int i = 100; i <= 300; i+=10)
		gammaCorrection->addItem(QString::number(i).insert(1, '.'));
	hboxLayout->addWidget(gammaCorrection);
	gridLayout->addLayout(hboxLayout, 2, 0, 1, 2);

	label = new QLabel(parent);
	label->setText("Refresh Rate:");
	label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	gridLayout->addWidget(label, 2, 2, 1, 1);

	screenRefresh = new QLineEdit(parent);
	screenRefresh->setValidator(new QIntValidator(0, 999, this));
	gridLayout->addWidget(screenRefresh, 2, 3, 1, 1);
}

void EditorConfigDialog::setupSpeedWidgets(QWidget* parent)
{
	QIntValidator* allInts = new QIntValidator(this);

	QHBoxLayout* hboxLayout = new QHBoxLayout(parent);
	hboxLayout->setSpacing(4);
	hboxLayout->setMargin(4);

	hboxLayout->addItem(
		new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Ignored));

	QGridLayout* gridLayout = new QGridLayout;
	gridLayout->setSpacing(1);
	gridLayout->setMargin(0);

	gridLayout->addItem(new QSpacerItem(
		0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),
		0, 0);

	QLabel* label;
#define ADDSPEEDLABEL(text, row) \
	label = new QLabel(tr(text), parent); \
	label->setAlignment(Qt::AlignTrailing|Qt::AlignVCenter); \
	gridLayout->addWidget(label, row, 0, 1, 1); \

	ADDSPEEDLABEL("Scrolling Speeds:", 1);
	ADDSPEEDLABEL("Keyboard Scroll:", 2);
	ADDSPEEDLABEL("Mouse Scroll:", 3);
	ADDSPEEDLABEL("Tile Edit Window Scroll:", 4);
#undef ADDSPEEDLABEL

	label = new QLabel(tr("X"), parent);
	label->setMinimumSize(QSize(30, 0));
	label->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(label, 1, 1, 1, 1);

	label = new QLabel(tr("Y"), parent);
	label->setMinimumSize(QSize(30, 0));
	label->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(label, 1, 2, 1, 1);

#define ADDSPEEDLINEEDIT(var, row, col) \
	var = new QLineEdit(parent); \
	var ->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed); \
	var ->setValidator(allInts); \
	gridLayout->addWidget( var , row , col , 1, 1); \

#define ADDSPEEDXYPAIR(enum, row) \
	ADDSPEEDLINEEDIT(scrollSpeeds[ enum ][0], row , 1) \
	ADDSPEEDLINEEDIT(scrollSpeeds[ enum ][1], row , 2) \

	ADDSPEEDXYPAIR(keyboardScroll, 2);
	ADDSPEEDXYPAIR(mouseScroll, 3);
	ADDSPEEDXYPAIR(tileeditScroll, 4);
#undef ADDSPEEDXYPAIR
#undef ADDSPEEDLINEEDIT

	gridLayout->addItem(new QSpacerItem(
		0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),
		5, 0);

	hboxLayout->addLayout(gridLayout);

	hboxLayout->addItem(
		new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Ignored));

	QVBoxLayout* vboxLayout = new QVBoxLayout;

	vboxLayout->addItem(
		new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Expanding));

	label = new QLabel(tr("Object Edit Scroll:"), parent);
	label->setAlignment(Qt::AlignCenter);
	vboxLayout->addWidget(label);

	editScrollSpeed = new QLineEdit(parent);
	editScrollSpeed->setValidator(allInts);
	vboxLayout->addWidget(editScrollSpeed);

	vboxLayout->addItem(
		new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Expanding));

	lowZoomedScrollSpeed = new QCheckBox(tr("Low scroll speed\nwhile zoomed out"), parent);
	vboxLayout->addWidget(lowZoomedScrollSpeed);

	vboxLayout->addItem(
		new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Expanding));

	hboxLayout->addLayout(vboxLayout);

	hboxLayout->addItem(
		new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Ignored));

	vboxLayout = new QVBoxLayout;

	vboxLayout->addItem(
		new QSpacerItem(20, 40, QSizePolicy::Ignored, QSizePolicy::Preferred));

	label = new QLabel(tr("Mouse Speed:"), parent);
	label->setAlignment(Qt::AlignCenter);
	vboxLayout->addWidget(label);

	QHBoxLayout* hboxLayout2;
#define ADDMOUSESPEED(text, index) \
	hboxLayout2 = new QHBoxLayout; \
	label = new QLabel(tr(text), parent); \
	label->setAlignment(Qt::AlignRight|Qt::AlignVCenter); \
	label->setMaximumWidth(20); \
	hboxLayout2->addWidget(label); \
	mouseSpeeds[index] = new QLineEdit(parent); \
	mouseSpeeds[index]->setValidator(allInts); \
	mouseSpeeds[index]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed); \
	hboxLayout2->addWidget(mouseSpeeds[index]); \
	vboxLayout->addLayout(hboxLayout2); \

	ADDMOUSESPEED("X:", 0);
	ADDMOUSESPEED("Y:", 1);
#undef ADDMOUSESPEED

	vboxLayout->addItem(
		new QSpacerItem(20, 40, QSizePolicy::Ignored, QSizePolicy::Preferred));

	hboxLayout->addLayout(vboxLayout);

	hboxLayout->addItem(
		new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Ignored));
}

void EditorConfigDialog::setupMiscWidgets(QWidget* parent)
{
	QIntValidator* allInts = new QIntValidator(this);

	QHBoxLayout* hboxLayout = new QHBoxLayout(parent);
	hboxLayout->setSpacing(6);
	hboxLayout->setMargin(2);

	QVBoxLayout* vboxLayout = new QVBoxLayout;
	vboxLayout->setSpacing(4);
	vboxLayout->setMargin(0);

#define ADDMISCCHECK(var, text) \
	var = new QCheckBox(tr( text ), parent); \
	vboxLayout->addWidget( var ); \

	ADDMISCCHECK(enableWorkspace, "Enable workspaces");
	ADDMISCCHECK(allowHigherType2, "Allow type 2 objects of higher Acts");
	ADDMISCCHECK(alwaysMaxLayerCount, "Always maximize layer count");
	ADDMISCCHECK(stretchSpritesOnZoom, "Stretch sprites when zooming");
	ADDMISCCHECK(minimizeSavedDs1, "Minimize file size of saved .ds1");
#undef ADDMISCCHECK

	QHBoxLayout* hboxLayout2 = new QHBoxLayout;
	QLabel* label = new QLabel(tr("Zoom after centering:"), parent);
	label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	hboxLayout2->addWidget(label);
	zoomAfterCentering = new QComboBox(parent);
	zoomAfterCentering->addItems(QStringList()
		<<"NO_CHANGE"<<"1:1"<<"1:2"<<"1:4"<<"1:8"<<"1:16");
	hboxLayout2->addWidget(zoomAfterCentering);
	vboxLayout->addLayout(hboxLayout2);

	hboxLayout->addLayout(vboxLayout);

	vboxLayout = new QVBoxLayout;
	vboxLayout->setSpacing(4);
	vboxLayout->setMargin(0);

	label = new QLabel(tr("When objects editing window is open:"), parent);
	vboxLayout->addWidget(label);

	mouseScrollsMain = new QCheckBox(tr("Mouse scrolls main screen"), parent);
	vboxLayout->addWidget(mouseScrollsMain);

	keybScrollsMain = new QCheckBox(tr("Keyboard scrolls main screen"), parent);
	vboxLayout->addWidget(keybScrollsMain);

	vboxLayout->addItem(
		new QSpacerItem(20, 16, QSizePolicy::Ignored, QSizePolicy::Expanding));

	QGridLayout* gridLayout = new QGridLayout;
	label = new QLabel(tr("type 1 objects:"), parent);
	gridLayout->addWidget(label, 0, 0, 1, 1);
	label = new QLabel(tr("type 2 objects:"), parent);
	gridLayout->addWidget(label, 1, 0, 1, 1);
	type1TableSize = new QLineEdit(parent);
	type1TableSize->setValidator(allInts);
	gridLayout->addWidget(type1TableSize, 0, 1, 1, 1);
	type2TableSize = new QLineEdit(parent);
	type2TableSize->setValidator(allInts);
	gridLayout->addWidget(type2TableSize, 1, 1, 1, 1);
	QPushButton* pushButton = new QPushButton(tr("Revert"), parent);
	gridLayout->addWidget(pushButton, 0, 2, 2, 1);
	connect(pushButton, SIGNAL(clicked()), SLOT(revertTableSizes()));

	vboxLayout->addLayout(gridLayout);

	hboxLayout->addLayout(vboxLayout);
}

void EditorConfigDialog::loadEditorConfig()
{
	ini::AttributeMap data;
	try {
		ini::editor::readData(data);
		preserveComments->setEnabled(true);
	} catch (Exception& e) {
		QMessageBox::warning(this, "Warning", e.what());
		preserveComments->setEnabled(false);
	}
	syncData(data, false);
}

// boolean return value no longer required
// was used to detect whether all settings were present in the ini,
// since writing in 'preserved' mode would not create settings not present.
// that is now fixed, so return value redundant.
void EditorConfigDialog::syncData(ini::AttributeMap& data, bool saveDialogData)
{
	bool /*preserve = true,*/ read = !saveDialogData;

	QString pathDefaults[pathSize] = {
		paths[d2charPath]->text(),
		paths[d2dataPath]->text(),
		paths[d2expPath]->text(),
		paths[patch_d2Path]->text(),
		paths[mod_dirPath]->text()
	};
#ifdef Q_WS_WIN
	// get default mpq path based on diablo install path in registry
	QSettings diabloSettings("HKEY_CURRENT_USER\\Software\\Blizzard Entertainment\\Diablo II",
		QSettings::NativeFormat);
	if (diabloSettings.contains("InstallPath"))
	{
		QString installPath = diabloSettings.value("InstallPath").toString();
		if (installPath.length())
		{
			QDir installDir(installPath);
#define DEFPATHFROMINST(name) pathDefaults[( name##Path )] = installDir.filePath( #name ".mpq" )
			DEFPATHFROMINST(d2char);
			DEFPATHFROMINST(d2data);
			DEFPATHFROMINST(d2exp);
			DEFPATHFROMINST(patch_d2);
		}
	}
#undef DEFPATHFROMINST
#endif

//#define SYNCPATH(name) preserve *= lineedit(#name, paths[ name##Path ], \ /**/
#define SYNCPATH(name) lineedit(#name, paths[ name##Path ], \
		pathDefaults[ name##Path ], read, data)
	SYNCPATH(d2char);
	SYNCPATH(d2data);
	SYNCPATH(d2exp);
	SYNCPATH(patch_d2);
	SYNCPATH(mod_dir);
#undef SYNCPATH
	/*preserve *=*/ checkbox("full_screen", useFullscreen, true, false, read, data);
	/*preserve *=*/ lineedit("screen_width", screenWidth, "800", read, data);
	/*preserve *=*/ lineedit("screen_height", screenHeight, "600", read, data);
	/*preserve *=*/ combobox("screen_depth", screenDepth, 4, read, data);
	/*preserve *=*/ lineedit("refresh_rate", screenRefresh, "60", read, data);
	/*preserve *=*/ lineedit("keyb_scroll_x", scrollSpeeds[keyboardScroll][0], "40", read, data);
	/*preserve *=*/ lineedit("keyb_scroll_y", scrollSpeeds[keyboardScroll][1], "20", read, data);
	/*preserve *=*/ lineedit("mouse_scroll_x", scrollSpeeds[mouseScroll][0], "20", read, data);
	/*preserve *=*/ lineedit("mouse_scroll_y", scrollSpeeds[mouseScroll][1], "10", read, data);
	/*preserve *=*/ lineedit("edit_scroll_x", scrollSpeeds[tileeditScroll][0], "20", read, data);
	/*preserve *=*/ lineedit("edit_scroll_y", scrollSpeeds[tileeditScroll][1], "20", read, data);
	/*preserve *=*/ lineedit("obj_edit_scroll", editScrollSpeed, "2", read, data);
	/*preserve *=*/ lineedit("mouse_speed_x", mouseSpeeds[0], "2", read, data);
	/*preserve *=*/ lineedit("mouse_speed_y", mouseSpeeds[1], "2", read, data);
	/*preserve *=*/ combobox("gamma_correction", gammaCorrection, 23, read, data);
	/*preserve *=*/ checkbox("only_normal_type2", allowHigherType2, false, true, read, data);
	/*preserve *=*/ checkbox("always_max_layers", alwaysMaxLayerCount, true, false, read, data);
	/*preserve *=*/ checkbox("stretch_sprite", stretchSpritesOnZoom, true, false, read, data);
	/*preserve *=*/ checkbox("ds1_saved_minimize", minimizeSavedDs1, true, false, read, data);
	/*preserve *=*/ combobox("center_zoom", zoomAfterCentering, 1, read, data);
	/*preserve *=*/ checkbox("winobj_can_scroll_keyb", mouseScrollsMain, true, false, read, data);
	/*preserve *=*/ checkbox("winobj_can_scroll_mouse", keybScrollsMain, false, false, read, data);
	/*preserve *=*/ lineedit("nb_type1_per_act", type1TableSize, "60", read, data);
	/*preserve *=*/ lineedit("nb_type2_per_act", type2TableSize, "150", read, data);
	/*preserve *=*/ checkbox("lower_speed_zoom_out", lowZoomedScrollSpeed, false, false, read, data);
	/*preserve *=*/ checkbox("workspace_enable", enableWorkspace, true, false, read, data);
	//return preserve;
}

void EditorConfigDialog::browseForMpq(QLineEdit* dest, const QString& name)
{
	QString mpq = QFileDialog::getOpenFileName(this,
		QString("Browse for %1.mpq").arg(name), dest->text(),
		"Blizzard MoPaQs (*.mpq);;All files (*.*)");
	if (mpq.length())
		dest->setText(mpq);
}

void EditorConfigDialog::mod_dirBrowse()
{
	QString mod_dir = QFileDialog::getExistingDirectory(this,
		tr("Select your mod_dir"), paths[mod_dirPath]->text());
	if (mod_dir.length())
		paths[mod_dirPath]->setText(mod_dir);
}

void EditorConfigDialog::revertTableSizes()
{
	type1TableSize->setText("60");
	type2TableSize->setText("150");
}

void EditorConfigDialog::restoreDefaults()
{
	ini::AttributeMap data;
	syncData(data, false);
}

void EditorConfigDialog::accept()
{
	try {
		ini::AttributeMap data;
		syncData(data, true);
		ini::writeMode mode;
		if (preserveComments->isChecked())
			mode = ini::preserve;
		else if (resetComments->isChecked())
			mode = ini::full;
		else
			mode = ini::sparse;
		ini::editor::writeData(data, mode);
	} catch (Exception& e) {
		QMessageBox::critical(this, "Error", e.what());
	}
	QDialog::accept();
}
