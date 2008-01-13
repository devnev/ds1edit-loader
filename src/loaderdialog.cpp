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

#include "loaderdialog.h"
#include <QSpacerItem>
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QApplication>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QTreeWidget>
#include <QGroupBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QSplitter>
#include <QIntValidator>
#include <QAction>
#include <QShowEvent>
#include <QTextStream>
#include <QList>
#include <QUrl>
#include <QSettings>
#include "stacktrace.h"
#include "exception.h"
#include "selectwantedfiledialog.h"
#include "editorinterface.h"
#include "dt1listdialog.h"
#include "editorconfigdialog.h"
#include "loaderconfigdialog.h"
#include "aboutdialog.h"
#include "datasync.h"
#include "mapsnotfounddialog.h"
#include "loaderparams.h"
#ifdef Q_WS_WIN
#include "dde.h"
#endif
#include <cassert>
#ifndef NDEBUG
#include <iostream>
#endif

#ifndef DEBUG_ONLY
#ifdef NDEBUG
#define DEBUG_ONLY(exp) ((void)0)
#else
#define DEBUG_ONLY(exp) exp
#endif
#endif

#ifndef NDEBUG
using std::cout;
#endif

LoaderDialog::LoaderDialog(QWidget* parent)
: QMainWindow(parent, Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMinMaxButtonsHint)
{
	FNTRACE("", "LoaderDialog", "LoaderDialog", "parent");

	resizeValidator = new QIntValidator(1, 255, this);
	actValidator = new QIntValidator(1, 5, this);

	bool configure = false;
	try {
		reloadSettings();
		if (ds1editPath.isEmpty() || ds1editName.isEmpty())
			configure = true;
	} catch (Exception& e) {
		// fail quietly the first time
		configure = true;
	}
	if (configure)
	{
		if (QMessageBox::warning(this, tr("Unconfigured Loader"),
			tr("The loader does not seem to be configured properly.\n"
			"Would you like to configure it now?\n"),
			QMessageBox::Yes | QMessageBox::No)
			== QMessageBox::No)
			ETHROW(Exception("Loader not configured properly"));
		// ask the user to configure the loader then reload settings again
		try {
			LoaderConfigDialog dlg(this);
			if (dlg.exec() == QDialog::Accepted)
				reloadSettings();
			else
				ETHROW(Exception("Loader not configured properly"));
		} catch (Exception& e) {
			QMessageBox::critical(this, "Error", e.what());
			throw;
			//ETHROW(Exception(e.what()));
		}
	}

	setupWidgets();
	setupActions();
	setupMenu();

	setWindowIcon(QIcon(":/res/ds1edit loader.png"));
	setWindowTitle(tr("ds1edit Loader"));
	QWidget::resize(600, 300);

	setAcceptDrops(true);
	updateEnabledStates();

#ifdef Q_WS_WIN
	ddeServer = 0;
	enableSingleInstance(true);
	/*ddeServer = new DdeInterface(this);
	connect(ddeServer, SIGNAL(open(const QString&)), SLOT(addFile(const QString&)));

	// running from dde command should probably be done asynchronously... so
	// leave this disabled till that's implemented
	//connect(ddeServer, SIGNAL(run()), SLOT(run()));
	// according to the spec, commands that make the server shutdown should be
	// performed *after* the callback has returned...
	//connect(ddeServer, SIGNAL(exit()), SLOT(close()));*/
#endif
}

void LoaderDialog::setupWidgets()
{
	FNTRACE("", "LoaderDialog", "setupWidgets", "");

	resizeXLabel = new QLabel(tr("&X:"));
	resizeX = new QLineEdit("250");
	resizeXLabel->setBuddy(resizeX);
	resizeYLabel = new QLabel(tr("&Y:"));
	resizeY = new QLineEdit("250");
	resizeYLabel->setBuddy(resizeY);
	forcePalActLabel = new QLabel(tr("&Act:"));
	forcePalAct = new QLineEdit("1");
	forcePalActLabel->setBuddy(forcePalAct);
	resizeX->setValidator(resizeValidator);
	resizeY->setValidator(resizeValidator);
	forcePalAct->setValidator(actValidator);

	resizeParamLayout = new QHBoxLayout;
	resizeParamLayout->addWidget(resizeXLabel);
	resizeParamLayout->addWidget(resizeX);
	resizeParamLayout->addWidget(resizeYLabel);
	resizeParamLayout->addWidget(resizeY);
	forcePalActLayout = new QHBoxLayout;
	forcePalActLayout->addWidget(forcePalActLabel);
	forcePalActLayout->addWidget(forcePalAct);
	resizeParamLayout->setSpacing(3);
	forcePalActLayout->setSpacing(3);

	forceDt1 = new QCheckBox(tr("Forc&e .dt1"));
	forcePalette = new QCheckBox(tr("Force Pa&lette"));
	noCheckAct = new QCheckBox(tr("&Don't Check Act"));
	resize = new QCheckBox(tr("&Resize"));
	autoPrestDef = new QCheckBox(tr("A&uto-detect LvlPrest Def"));
	resizeParamWidget = new QWidget;
	configDt1List = new QPushButton(tr("&Configure .dt1 List..."));
	forcePalActWidget = new QWidget;
	resizeParamWidget->setLayout(resizeParamLayout);
	forcePalActWidget->setLayout(forcePalActLayout);

	connect(resize, SIGNAL(toggled(bool)), SLOT(setResizeEnabled(bool)));
	connect(forceDt1, SIGNAL(toggled(bool)), SLOT(setConfigDt1ListEnabled(bool)));
	connect(forcePalette, SIGNAL(toggled(bool)), SLOT(setForcePalActEnabled(bool)));
	connect(configDt1List, SIGNAL(clicked()), SLOT(configureDt1List()));

	paramsLayout = new QGridLayout;
	paramsLayout->addWidget(resize, 0, 0);
	paramsLayout->addWidget(forceDt1, 0, 1);
	paramsLayout->addWidget(forcePalette, 0, 2);
	paramsLayout->addWidget(noCheckAct, 0, 3);
	paramsLayout->addWidget(resizeParamWidget, 1, 0);
	paramsLayout->addWidget(configDt1List, 1, 1);
	paramsLayout->addWidget(forcePalActWidget, 1, 2);
	paramsLayout->addWidget(autoPrestDef, 1, 3);

	mapList = new QTreeWidget;
	params = new QGroupBox(tr("Parameters"));
	editorOutput = new QTextEdit;
	params->setLayout(paramsLayout);
	mapList->setColumnCount(6);
	mapList->headerItem()->setText(0, tr("File"));
	mapList->headerItem()->setText(1, tr("Name"));
	mapList->headerItem()->setText(2, tr("Game Path"));
	mapList->headerItem()->setText(3, tr("LvlType ID"));
	mapList->headerItem()->setText(4, tr("LvlPrest Def"));
	mapList->headerItem()->setText(5, tr("Comment"));
	mapList->setSelectionBehavior(QAbstractItemView::SelectRows);
	mapList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	connect(mapList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		SLOT(mapDoubleClicked(QTreeWidgetItem*, int)));

	QFont monospaceFont("Courier", 10);
	monospaceFont.setStyleHint(QFont::TypeWriter);
	QTextCharFormat outputFormat;
	outputFormat.setFont(monospaceFont);
	editorOutput->setCurrentCharFormat(outputFormat);
	editorOutput->setAcceptRichText(false);
	editorOutput->setReadOnly(true);
	editorOutput->setStyleSheet(editorOutput->styleSheet() +
		"\nQWidget { background: #CCC; }\n");

	menu = menuBar(); //new QMenuBar(this);
	//menu->setMinimumWidth(200);

	QSplitter* splitter = new QSplitter(Qt::Vertical, this);
	setCentralWidget(splitter);
	QWidget* wrapper = new QWidget;
	QVBoxLayout* wrapperLayout = new QVBoxLayout(wrapper);
	wrapperLayout->addWidget(mapList);
	wrapperLayout->addWidget(params);
	splitter->addWidget(wrapper);
	editorOutputWrapper = new QWidget;
	wrapperLayout = new QVBoxLayout(editorOutputWrapper);
	wrapperLayout->addWidget(editorOutput);
	splitter->addWidget(editorOutputWrapper);
}

void LoaderDialog::setupActions()
{
	FNTRACE("", "LoaderDialog", "setupActions", "");

	addFileAct = new QAction(tr("&Add..."), this);
	removeFileAct = new QAction(tr("&Remove"), this);
	loadBatchAct = new QAction(tr("&Load Batch..."), this);
	makeBatchAct = new QAction(tr("&Make Batch..."), this);
	runAct = new QAction(tr("R&un"), this);
	exitAct = new QAction(tr("E&xit"), this);
	configEditorAct = new QAction(tr("Configure &Editor..."), this);
	configLoaderAct = new QAction(tr("Configure &Loader..."), this);
#ifdef Q_WS_WIN
	toggleSingleAct = new QAction(tr("&Auto-Add Files Opened in Explorer"), this);
	toggleSingleAct->setCheckable(true);
	toggleSingleAct->setChecked(true);
#endif
	showParamsAct = new QAction(tr("Show &Parameters"), this);
	showParamsAct->setCheckable(true);
	showOutputAct = new QAction(tr("Show &Output"), this);
	showOutputAct->setCheckable(true);
#ifdef Q_WS_WIN
	registerExtensionAct = new QAction(tr("&Register Extension"), this);
#endif
	aboutAct = new QAction(tr("&About..."), this);
	aboutQtAct = new QAction(tr("About &Qt..."), this);

	connect(addFileAct, SIGNAL(triggered()), SLOT(addFiles()));
	addFileAct->setShortcut(Qt::ControlModifier | Qt::Key_A);
	connect(removeFileAct, SIGNAL(triggered()), SLOT(removeSelection()));
	removeFileAct->setShortcut(Qt::Key_Delete);
	connect(loadBatchAct, SIGNAL(triggered()), SLOT(loadBatch()));
	loadBatchAct->setShortcut(Qt::ControlModifier | Qt::Key_O);
	connect(makeBatchAct, SIGNAL(triggered()), SLOT(makeBatch()));
	makeBatchAct->setShortcut(Qt::ControlModifier | Qt::Key_M);
	connect(runAct, SIGNAL(triggered()), SLOT(run()));
	runAct->setShortcut(Qt::ControlModifier | Qt::Key_R);
	connect(exitAct, SIGNAL(triggered()), SLOT(close()));
	exitAct->setShortcut(Qt::ControlModifier | Qt::Key_Q);
	connect(showParamsAct, SIGNAL(toggled(bool)), params, SLOT(setVisible(bool)));
	connect(showOutputAct, SIGNAL(toggled(bool)), editorOutputWrapper, SLOT(setVisible(bool)));
	connect(configEditorAct, SIGNAL(triggered()), SLOT(configureEditor()));
	connect(configLoaderAct, SIGNAL(triggered()), SLOT(configureLoader()));
#ifdef Q_WS_WIN
	connect(toggleSingleAct, SIGNAL(toggled(bool)), SLOT(enableSingleInstance(bool)));
	connect(registerExtensionAct, SIGNAL(triggered()), SLOT(registerExtension()));
#endif
	connect(aboutAct, SIGNAL(triggered()), SLOT(about()));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

#ifdef Q_WS_WIN
	registerExtensionAct->setEnabled(false);
#endif
}

void LoaderDialog::setupMenu()
{
	FNTRACE("", "LoaderDialog", "setupMenu", "");

	QMenu* currMenu;

	currMenu = menu->addMenu(tr("&File"));
	currMenu->addAction(addFileAct);
	currMenu->addAction(removeFileAct);
	currMenu->addSeparator();
	currMenu->addAction(loadBatchAct);
	currMenu->addSeparator();
	currMenu->addAction(makeBatchAct);
	currMenu->addAction(runAct);
	currMenu->addSeparator();
	currMenu->addAction(exitAct);

	currMenu = menu->addMenu(tr("&Options"));
	currMenu->addAction(configEditorAct);
	currMenu->addAction(configLoaderAct);
	currMenu->addSeparator();
#ifdef Q_WS_WIN
	currMenu->addAction(toggleSingleAct);
	currMenu->addAction(registerExtensionAct);
#endif

	currMenu = menu->addMenu(tr("&View"));
	currMenu->addAction(showParamsAct);
	currMenu->addAction(showOutputAct);

	currMenu = menu->addMenu(tr("&Help"));
	currMenu->addAction(aboutAct);
	currMenu->addAction(aboutQtAct);
}

void LoaderDialog::updateEnabledStates()
{
	FNTRACE("", "LoaderDialog", "updateEnabledStates", "");

	params->setVisible(showParamsAct->isChecked());
	editorOutputWrapper->setVisible(showOutputAct->isChecked());

	const bool enableParams = mapList->topLevelItemCount() <= 1;
	forceDt1->setEnabled(enableParams);
	forcePalette->setEnabled(enableParams);
	noCheckAct->setEnabled(enableParams);
	resize->setEnabled(enableParams);
	resizeParamWidget->setEnabled(enableParams && resize->isChecked());
	configDt1List->setEnabled(enableParams && forceDt1->isChecked());
	forcePalActWidget->setEnabled(enableParams && forcePalette->isChecked());

	if (mapList->topLevelItemCount() > 0)
	{
		makeBatchAct->setEnabled(true);
		runAct->setEnabled(true);
	}
	else
	{
		makeBatchAct->setEnabled(false);
		runAct->setEnabled(false);
	}
}

void LoaderDialog::showEvent(QShowEvent* event)
{
	FNTRACE("", "LoaderDialog", "showEvent", "event");

	updateEnabledStates();
	QMainWindow::showEvent(event);
}

void LoaderDialog::AddDS1(const QString& fileName, const QString& mapName, const QString& gamePath,
	int typeId, int prestDef, const QString& comment)
{
	FNTRACE("", "LoaderDialog", "AddDS1", QString("%1, %2, %3, %4, %5, %6")
		.arg(fileName).arg(mapName).arg(gamePath).arg(typeId).arg(prestDef).arg(comment));

	if (mapList->topLevelItemCount() >= MAX_MAPS)
		ETHROW(Exception("Maximum map number reached"));
	if (mapList->findItems(fileName, Qt::MatchFixedString, 0).size() > 0)
		return;

	QTreeWidgetItem* item = new QTreeWidgetItem;
	item->setText(0, fileName);
	item->setText(1, mapName);
	item->setText(2, gamePath);
	item->setText(3, QString::number(typeId));
	item->setText(4, QString::number(prestDef));
	item->setText(5, comment);

	mapList->addTopLevelItem(item);

	updateEnabledStates();
}

void LoaderDialog::addFiles()
{
	FNTRACE("", "LoaderDialog", "addFiles", "");

	if (mapList->topLevelItemCount() >= MAX_MAPS)
		return;

	QStringList mapFiles = QFileDialog::getOpenFileNames(
		this, tr("Choose Maps to Add to the List"), QString(),
		tr("Diablo Maps (*.ds1);;All Files (*.*)"));

	try {
		addFileList(mapFiles);
	} catch (Exception& e) {
		QMessageBox::critical(this, "Error", e.what());
	}
}

void LoaderDialog::removeSelection()
{
	FNTRACE("", "LoaderDialog", "removeSelection", "");

	QList<QTreeWidgetItem*> selection = mapList->selectedItems();
	for (QList<QTreeWidgetItem*>::iterator selectedIter = selection.begin();
		selectedIter != selection.end(); ++selectedIter)
	{
		delete mapList->takeTopLevelItem(mapList->indexOfTopLevelItem(*selectedIter));
	}
	updateEnabledStates();
}

void LoaderDialog::run()
{
	FNTRACE("", "LoaderDialog", "run", "");

	if (mapList->topLevelItemCount() == 0)
		return;

	try {
		setEnabled(false);
		//QDir::setCurrent(ds1editPath);

		EditorInterface* editor = new EditorInterface(this, ds1editPath, ds1editName);
		connect(editor, SIGNAL(finished()), SLOT(enable()));
		connect(editor, SIGNAL(finished()), editor, SLOT(deleteLater()));
		editor->run();
		showOutput();
	} catch (Exception& e) {
		QMessageBox::critical(this, "Error", e.what());
		enable();
	}
}

void LoaderDialog::makeBatch()
{
	FNTRACE("", "LoaderDialog", "makeBatch", "");

	if (mapList->topLevelItemCount() == 0)
		return;

	QString batchName = QFileDialog::getSaveFileName(QApplication::activeWindow(),
		tr("Save Batch Script"), "", tr("Batch Scripts (*.bat);; All Files (*.*)"));
	if (batchName.isEmpty())
		return;

	try {
		EditorInterface editor(this, ds1editPath, ds1editName);
		editor.makeBatch(batchName);
	} catch (Exception& e) {
		QMessageBox::critical(this, "Error", e.what());
	}
}

/** Load previously saved script.
 * Analyse first line of selected file to determine type.
 * Thats right, I'm not the extension :P
 */
void LoaderDialog::loadBatch()
{
	FNTRACE("", "LoaderDialog", "loadBatch", "");

	QString fileName = QFileDialog::getOpenFileName(QApplication::activeWindow(),
		tr("Loadable Files"), "", tr("Loadable files (*.bat *.ini);; All Files (*.*)"));
	if (fileName.length() == 0)
		return;

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		QMessageBox::critical(this, "Error", QString("Failed to open %1 for reading").arg(fileName));
	QTextStream in(&file);

	QString line = in.readLine();
	in.setDevice(0);
	file.close();
	DEBUG_ONLY(cout << line.toStdString() << '\n');

	try {
		if (line.compare("@echo off", Qt::CaseInsensitive) == 0)
			loadFromBatch(fileName);
		else
			loadFromIni(fileName);
	} catch (Exception& e) {
		QMessageBox::critical(this, "Error", e.what());
	}
}

/** Load contents of a batch file.
 * General expected format of batch file (line numbers added):
\verbatim
1: @echo off
2: <drive>:
3: cd <path>
4: <editorname> <params>
\endverbatim
 * - Line 2 is optional and meant for changing the "current drive" under windows.
 * - The @c path, @c editorname and @c params may be quoted as needed.
 * - Escaping spaces is not supported.
 *
 * Warning: Messy Code.
 */
void LoaderDialog::loadFromBatch(const QString& fileName)
{
	FNTRACE("", "LoaderDialog", "loadFromBatch", fileName);

	DEBUG_ONLY(cout << "batch\n");

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		QMessageBox::critical(this, "Error", QString("Failed to open %1 for reading").arg(fileName));
	QTextStream in(&file);

	QString line = in.readLine();
	int lineNumber = 1;
	assert(line.compare("@echo off", Qt::CaseInsensitive) == 0);

	if (in.atEnd())
		ETHROW(ParseError(fileName, 1, "Unexpected end of file."));
	line = in.readLine();
	lineNumber = 2;
	if (in.atEnd())
		ETHROW(ParseError(fileName, 2, "Unexpected end of file."));
	DEBUG_ONLY(cout << "2: " << line.toStdString() << '\n');
	if (QDir::drives().contains(QFileInfo(line+'/'))) // drive letter
	{
		DEBUG_ONLY(cout << "drive change\n");
		line = in.readLine();
		if (in.atEnd())
			ETHROW(ParseError(fileName, 3, "Unexpected end of file."));
		lineNumber = 3;
		DEBUG_ONLY(cout << "3: " << line.toStdString() << '\n');
	}
	if (!line.startsWith("cd "))
		ETHROW(ParseError(fileName, 3,
			QString("Expected line starting with \"cd \", got\n%1").arg(line)));
	line = line.mid(3); // drop "cd " part
	QString directory;
	if (line[0] == '"')
	{
		DEBUG_ONLY(cout << "quoted\n");
		if (line[line.length()-1] != '"')
			ETHROW(ParseError(fileName, lineNumber, "Weird line, missing \"?"));
		directory = line.mid(1); // chop leading "
		directory.chop(1); // chop trailing "
	}
	else
		directory = line;
	DEBUG_ONLY(cout << "directory: " << directory.toStdString() << '\n');

	QString executable;
	line = in.readLine();
	++lineNumber;
	DEBUG_ONLY(cout << lineNumber << ": " << line.toStdString() << '\n');
	line = line.trimmed();
	if (line[0] == '"')
	{
		DEBUG_ONLY(cout << "quoted\n");
		line = line.mid(1);
		if (!line.contains('"'))
			ETHROW(ParseError(fileName, lineNumber, "Weird line, missing \"?"));
		executable = line.left(line.indexOf('"'));
		line = line.mid(executable.length()).trimmed();
	}
	else
	{
		executable = line.left(line.indexOf(QRegExp("\\s"))); // space delimiter
		line = line.mid(executable.length()).trimmed();
	}
	DEBUG_ONLY(cout << "executable: " << executable.toStdString() << '\n');
	DEBUG_ONLY(cout << "remaining line: " << line.toStdString() << '\n');

	QString paramFileName;
	if (line[0] == '"')
	{
		line = line.mid(1);
		if (!line.contains('"'))
			ETHROW(ParseError(fileName, lineNumber, "Weird line, missing \"?"));
		paramFileName = line.left(line.indexOf('"'));
		line = line.mid(paramFileName.length()).trimmed();
	}
	else
	{
		paramFileName = line.left(line.indexOf(QRegExp("\\s"))); // space delimiter
		line = line.mid(paramFileName.length()).trimmed();
	}
	DEBUG_ONLY(cout << "paramFileName: " << paramFileName.toStdString() << '\n');
	DEBUG_ONLY(cout << "remaining line: " << line.toStdString() << '\n');

	// make paramFileName relative
	if (QFileInfo(paramFileName).isRelative())
		paramFileName = QDir(directory).absoluteFilePath(paramFileName);

	if (paramFileName.endsWith(".ini"))
	{
		// it's a .ini, so proceed to .ini-parsing
		in.setDevice(0);
		file.close();
		loadFromIni(paramFileName, directory);
	}
	else
	{
		Ds1Info map;
		int result = SelectWantedFileDialog::selectWantedFile(paramFileName, map, this);
		if (result == QDialog::Accepted)
			AddDS1(map.file, map.name, map.path, map.typeId, map.prestDef, map.comment);
		else if (result == SelectWantedFileDialog::NotFound)
			QMessageBox::information(this, "Map Not Found",
				tr("No matching map entries found for file %1.").arg(paramFileName));
	}
}

void LoaderDialog::loadFromIni(const QString& fileName, const QString& basePath)
{
	FNTRACE("", "LoaderDialog", "loadFromIni", fileName);

	DEBUG_ONLY(cout << "ini\n");

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		QMessageBox::critical(this, "Error", QString("Failed to open %1 for reading").arg(fileName));
	QTextStream in(&file);

	QString /*typeId, prestDef,*/ mapFileName;
	QList<Ds1Info> selectedMaps;
	QStringList unknownMaps;
	QString line;
	int lineNumber = 0;
	while (!in.atEnd())
	{
		line = in.readLine(); ++lineNumber;
		DEBUG_ONLY(cout << lineNumber << ": " << line.toStdString() << '\n');

		if (line.isEmpty()) continue;
		//typeId = line.left(line.indexOf(' '));
		line = line.mid(line.indexOf(' ')).trimmed();
		//prestDef = line.left(line.indexOf(' '));
		mapFileName = line.mid(line.indexOf(' ')).trimmed();
		if (mapFileName.length() == 0)
			ETHROW(ParseError(fileName, lineNumber, "Invalid line"));
		if (QFileInfo(mapFileName).isRelative() && basePath.length())
			mapFileName = QDir(basePath).absoluteFilePath(mapFileName);
		Ds1Info ds1;
		int result = SelectWantedFileDialog::selectWantedFile(mapFileName, ds1, this);
		if (result == SelectWantedFileDialog::Skipped)
			continue;
		else if (result == SelectWantedFileDialog::Accepted)
			selectedMaps.append(ds1);
		else if (result == SelectWantedFileDialog::NotFound)
			unknownMaps << mapFileName;
		else
			return;
	}

	for (QList<Ds1Info>::iterator mapIter = selectedMaps.begin();
		mapIter != selectedMaps.end(); ++mapIter)
	{
		AddDS1(mapIter->file, mapIter->name, mapIter->path, mapIter->typeId,
			mapIter->prestDef, mapIter->comment);
	}
	if (unknownMaps.size())
		QMessageBox::information(this, tr("Unknown Maps"),
			tr("No matching map entries found for following files:\n")
			.append(unknownMaps.join("\n")));
}

void LoaderDialog::configureDt1List()
{
	FNTRACE("", "LoaderDialog", "configureDt1List", "");

	Dt1ListDialog dlg(dt1List, this);
	if (dlg.exec() == QDialog::Accepted)
		dt1List = dlg.dt1List();
}

void LoaderDialog::configureEditor()
{
	FNTRACE("", "LoaderDialog", "configureEditor", "");

	QDir currentDir = QDir::current();
	try {
		QDir::setCurrent(ds1editPath);
		EditorConfigDialog dlg(this, true);
		dlg.exec();
	} catch (Exception& e) {
		QMessageBox::critical(this, "Error", e.what());
	}
	QDir::setCurrent(currentDir.absolutePath());
}

void LoaderDialog::configureLoader()
{
	FNTRACE("", "LoaderDialog", "configureLoader", "");

	try {
		LoaderConfigDialog dlg(this);
		if (dlg.exec() == QDialog::Accepted)
			reloadSettings();
	} catch (Exception& e) {
		QMessageBox::critical(this, "Error", e.what());
	}
}

void LoaderDialog::setResizeEnabled(bool enabled)
{
	resizeParamWidget->setEnabled(enabled);
}
void LoaderDialog::setResizeDisabled(bool disabled)
{
	setResizeEnabled(!disabled);
}

void LoaderDialog::setConfigDt1ListEnabled(bool enabled)
{
	configDt1List->setEnabled(enabled);
}
void LoaderDialog::setConfigDt1ListDisabled(bool disabled)
{
	setConfigDt1ListEnabled(!disabled);
}

void LoaderDialog::setForcePalActEnabled(bool enabled)
{
	forcePalActWidget->setEnabled(enabled);
}
void LoaderDialog::setForcePalActDisabled(bool disabled)
{
	setForcePalActEnabled(!disabled);
}

void LoaderDialog::setParametersEnabled(bool enabled)
{
	params->setEnabled(enabled);
}
void LoaderDialog::setParametersDisabled(bool disabled)
{
	setParametersEnabled(!disabled);
}

void LoaderDialog::showParameters()
{
	assert(showParamsAct != 0);
	showParamsAct->setChecked(true);
}

void LoaderDialog::showOutput()
{
	assert(showOutputAct != 0);
	showOutputAct->setChecked(true);
}

void LoaderDialog::reloadSettings()
{
	FNTRACE("", "LoaderDialog", "reloadSettings", "");

	ini::AttributeMap iniData;
	ini::loader::readData(iniData);
	if (!iniData.contains("ds1editpath"))
		ETHROW(Exception("ds1editpath not found in loader's .ini"));
	ds1editPath = iniData.value("ds1editpath");
	if (!iniData.contains("ds1editname"))
		ETHROW(Exception("ds1editname not found in loader's .ini"));
	ds1editName = iniData.value("ds1editname");
}

void LoaderDialog::about()
{
	FNTRACE("", "LoaderDialog", "about", "");

	AboutDialog dlg;
	dlg.exec();
}

void LoaderDialog::dragEnterEvent(QDragEnterEvent* event)
{
	FNTRACE("", "LoaderDialog", "dragEnterEvent", event->mimeData()->formats().join(" "));

	if (event->mimeData()->hasUrls())
	{
		event->setDropAction(Qt::CopyAction);
		event->accept(rect());
	}
}

void LoaderDialog::dropEvent(QDropEvent* event)
{
	FNTRACE("", "LoaderDialog", "dropEvent", event->mimeData()->formats().join(" "));

	if (event->mimeData()->hasUrls() && event->proposedAction() == Qt::CopyAction)
	{
		QList<QUrl> urls = event->mimeData()->urls();
		event->acceptProposedAction();
		QStringList files; QString file;
		for (QList<QUrl>::iterator url = urls.begin();
			url != urls.end(); ++url)
		{
			file = url->toLocalFile();
			if (file.length())
				files << file;
		}
		try {
			addFileList(files);
		} catch (Exception& e) {
			QMessageBox::critical(this, "Error", e.what());
		}
	}
}

void LoaderDialog::addFileList(const QStringList& files)
{
	FNTRACE("", "LoaderDialog", "addFileList", files.join(" "));

	QString fileName;
	QList<Ds1Info> selectedMaps;
	QStringList unknownMaps;
	for (QStringList::const_iterator file = files.begin();
		file != files.end(); ++file)
	{
		Ds1Info ds1;
		int result = SelectWantedFileDialog::selectWantedFile(*file, ds1, this);
		if (result == SelectWantedFileDialog::Skipped)
			continue;
		else if (result == SelectWantedFileDialog::NotFound)
			unknownMaps << *file;
		else if (result == SelectWantedFileDialog::Accepted)
			selectedMaps.append(ds1);
		else
			return;
	}
	if (unknownMaps.size())
	{
		MapsNotFoundDialog dlg(unknownMaps, this);
		QList<Ds1Info> addedMaps;
		int result;
		if ((result = dlg.exec()) == QDialog::Accepted &&
			(addedMaps = dlg.newMaps()).size() > 0)
		{
			if (QMessageBox::question(this, tr("Save new maps?"),
				tr("Would you like to save the new maps to maplist.txt?"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)
				== QMessageBox::Yes)
			{
				params::appendParams(addedMaps);
			}
			selectedMaps += addedMaps;
		}
		else if (result == QDialog::Rejected)
			return;
	}
	for (QList<Ds1Info>::iterator mapIter = selectedMaps.begin();
		mapIter != selectedMaps.end(); ++mapIter)
	{
		AddDS1(mapIter->file, mapIter->name, mapIter->path, mapIter->typeId,
			mapIter->prestDef, mapIter->comment);
	}
}

void LoaderDialog::mapDoubleClicked(QTreeWidgetItem* item, int column)
{
	FNTRACE("", "LoaderDialog", "mapDoubleClicked", "...");

	Ds1Info map;
	if (SelectWantedFileDialog::selectWantedFile(item->text(0), map, this, false) == QDialog::Accepted)
	{
		item->setText(0, map.file);
		item->setText(1, map.name);
		item->setText(2, map.path);
		item->setText(3, QString::number(map.typeId));
		item->setText(4, QString::number(map.prestDef));
		item->setText(5, map.comment);
	}
}

#ifdef Q_WS_WIN
void  LoaderDialog::registerExtension()
{
	FNTRACE("", "LoaderDialog", "registerExtension", "");

	QSettings ext("HKEY_CLASSES_ROOT\\.ds1", QSettings::NativeFormat);
	ext.setValue(QString(), "Ds1editLoader.ds1");

	QSettings filetype("HKEY_CLASSES_ROOT\\Ds1editLoader.ds1",
		QSettings::NativeFormat);
	filetype.setValue(QString(), "Diablo Map");

	QSettings icon("HKEY_CLASSES_ROOT\\Ds1editLoader.ds1\\DefaultIcon",
		QSettings::NativeFormat);
	icon.setValue(QString(), QString("%1,1")
		.arg(QCoreApplication::applicationFilePath().replace('/', '\\')));

	QSettings command("HKEY_CLASSES_ROOT\\Ds1editLoader.ds1\\shell\\Open\\Command",
		QSettings::NativeFormat);
	command.setValue(QString(), QString("\"%1\" \"%2\"")
		.arg(QCoreApplication::applicationFilePath()).replace('/', '\\').arg("%1"));

	QSettings ddeexec("HKEY_CLASSES_ROOT\\Ds1editLoader.ds1\\shell\\Open\\ddeexec",
		QSettings::NativeFormat);
	ddeexec.setValue(QString(), "Open(%1)");

	QSettings app("HKEY_CLASSES_ROOT\\Ds1editLoader.ds1\\shell\\Open\\ddeexec\\Application",
		QSettings::NativeFormat);
	app.setValue(QString(), ServiceName);

	QSettings topic("HKEY_CLASSES_ROOT\\Ds1editLoader.ds1\\shell\\Open\\ddeexec\\Topic",
		QSettings::NativeFormat);
	topic.setValue(QString(), TopicName);

	QMessageBox::information(this, "Success", ".ds1 extension registered");
}
#endif

#ifdef Q_WS_WIN
void LoaderDialog::enableSingleInstance(bool enabled)
{
	FNTRACE("", "LoaderDialog", "enableSingleInstance", (enabled?"True":"False"));

	if (enabled && !ddeServer)
	{
		ddeServer = new DdeInterface(this);

		connect(ddeServer, SIGNAL(open(const QString&)), SLOT(addFile(const QString&)));

		// running from dde command should probably be done asynchronously, so
		// use Qt::QueuedConnection
		connect(ddeServer, SIGNAL(run()), SLOT(run()), Qt::QueuedConnection);

		// according to the spec, commands that make the server shutdown should be
		// performed *after* the callback has returned -> Qt::QueuedConnection
		connect(ddeServer, SIGNAL(exit()), SLOT(close()), Qt::QueuedConnection);
	}
	else if (!enabled && ddeServer)
	{
		delete ddeServer;
		ddeServer = 0;
	}
}
#endif
