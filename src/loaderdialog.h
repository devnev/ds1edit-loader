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

#ifndef LOADERDIALOG_H_INCLUDED
#define LOADERDIALOG_H_INCLUDED

#include <QMainWindow>
#include <QString>
#include <QStringList>

class QMenuBar;
class QVBoxLayout;
class QTabWidget;
class QTreeWidget;
class QGroupBox;
class QTextEdit;
class QGridLayout;
class QCheckBox;
class QLineEdit;
class QPushButton;
class QLabel;
class QHBoxLayout;
class QIntValidator;
class QAction;
class QShowEvent;
class QWidget;
class QTreeWidgetItem;
#ifdef Q_WS_WIN
class DdeInterface;
#endif

class LoaderDialog : public QMainWindow
{
	Q_OBJECT

private:
	/** @name Widgets
	 * Main dialog widgets.
	 */
	/*@{*/
	QMenuBar* menu;
	QVBoxLayout* dialogLayout;
	QTabWidget* dialogTabs;

	// top-level dialog widgets
	QTreeWidget* mapList;
	//QGroupBox* params;
	QWidget* params;
	QTextEdit* editorOutput;
	QWidget* editorOutputWrapper;

	// parameter-widgets
	QGridLayout* paramsLayout;
	QCheckBox* forceDt1; //BOOL	m_bForceDt1;
	QCheckBox* forcePalette; //BOOL	m_bForcePalette;
	QCheckBox* noCheckAct; //BOOL	m_bNoCheckAct;
	QCheckBox* resize; //BOOL	m_bResize;
	QLineEdit* resizeX; //DWORD	m_dwResizeX;
	QLineEdit* resizeY; //DWORD	m_dwResizeY;
	QPushButton* configDt1List;
	QLineEdit* forcePalAct; //UINT	m_iForcePalAct;
	QCheckBox* autoPrestDef;
	// labels
	QLabel *resizeXLabel, *resizeYLabel, *forcePalActLabel;
	// label-linedit layouts
	QHBoxLayout* resizeParamLayout; QWidget* resizeParamWidget;
	QHBoxLayout* forcePalActLayout; QWidget* forcePalActWidget;
	/*@}*/

	// validators
	QIntValidator* resizeValidator;
	QIntValidator* actValidator;

	/** @name Actions
	 * Main dialog actions
	 */
	/*@{*/
	QAction* addFileAct;
	QAction* removeFileAct;
	QAction* loadBatchAct;
	QAction* makeBatchAct;
	QAction* runAct;
	QAction* exitAct;
	QAction* configEditorAct;
	QAction* configLoaderAct;
#ifdef Q_WS_WIN
	QAction* toggleSingleAct;
#endif
	QAction* showParamsAct;
	QAction* showOutputAct;
#ifdef Q_WS_WIN
	QAction* registerExtensionAct;
#endif
	QAction* aboutAct;
	QAction* aboutQtAct;
	/*@}*/

#ifdef Q_WS_WIN
	DdeInterface* ddeServer;
#endif

	QStringList dt1List;

	void setupWidgets();
	void setupActions();
	void setupMenu();

	void updateEnabledStates();

	void loadFromBatch(const QString& fileName);
	void loadFromIni(const QString& fileName, const QString& basePath = QString());

	QString ds1editPath;
	QString ds1editName;
	bool useTabs;
	bool configUseTabs;
	bool showOutputOnRun;

protected:
	virtual void showEvent(QShowEvent* event);
	void reloadSettings();

private slots:
	void setResizeEnabled(bool enabled);
	void setResizeDisabled(bool disabled);

	void setConfigDt1ListEnabled(bool enabled);
	void setConfigDt1ListDisabled(bool disabled);

	void setForcePalActEnabled(bool enabled);
	void setForcePalActDisabled(bool disabled);

	void setParametersEnabled(bool enabled);
	void setParametersDisabled(bool disabled);

	void enable() { setEnabled(true); }

	void showParameters();
	void showOutput();

	void addFiles();
	void removeSelection();
	void run();
	void makeBatch();
	void loadBatch();
	void configureDt1List();
	void configureEditor();
	void configureLoader();
#ifdef Q_WS_WIN
	void registerExtension();
#endif
	void about();

#ifdef Q_WS_WIN
	void enableSingleInstance(bool enabled);
#endif

	void mapDoubleClicked(QTreeWidgetItem* item, int column);

public:
	LoaderDialog(QWidget* parent = 0);

	void AddDS1(const QString& fileName, const QString& mapName, const QString& gamePath,
		int typeId, int prestDef, const QString& comment);

	friend class EditorInterface;

	static const int MAX_MAPS = 100;

public slots:

	void addFileList(const QStringList& files);
	void addFile(const QString& file) { addFileList(QStringList(file)); }

protected:
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);
};

#endif //LOADERDIALOG_H_INCLUDED
