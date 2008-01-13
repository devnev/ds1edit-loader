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

#ifndef MAPDIALOG_H_INCLUDED
#define MAPDIALOG_H_INCLUDED

#include <QDialog>
#include <QString>
#include <QLineEdit>
#include "ds1info.h"

class QLabel;
class QTreeWidgetItem;

/// Dialog for Editing map list entries
class MapDialog : public QDialog
{
	Q_OBJECT

private:
	QLabel* fileLabel;
	QLineEdit* fileWidget;
	QLineEdit* nameWidget;
	QLineEdit* pathWidget;
	QLineEdit* typeIdWidget;
	QLineEdit* prestDefWidget;
	QLineEdit* commentWidget;
	bool empties;
	bool showFileWidget;

public:
	MapDialog(bool allowEmpties, QWidget* parent = 0);

	QString file() const { return fileWidget->text(); }
	void setFile(const QString& file) { fileWidget->setText(file); }
	QString name() const { return nameWidget->text(); }
	void setName(const QString& name) { nameWidget->setText(name); }
	QString path() const { return pathWidget->text(); }
	void setPath(const QString& path) { pathWidget->setText(path); }
	QString typeId() const { return typeIdWidget->text(); }
	void setTypeId(const QString& typeId) { typeIdWidget->setText(typeId); }
	void setTypeId(int typeId) { setTypeId(QString::number(typeId)); }
	QString prestDef() const { return prestDefWidget->text(); }
	void setPrestDef(const QString& prestDef) { prestDefWidget->setText(prestDef); }
	void setPrestDef(int prestDef) { setPrestDef(QString::number(prestDef)); }
	QString comment() const { return commentWidget->text(); }
	void setComment(const QString& comment) { commentWidget->setText(comment); }

	void fromMap(Ds1Info& map);
	void fromMapListItem(QTreeWidgetItem* listItem);
	Ds1Info toMap();

	bool showFileName() { return showFileWidget; }
	void setShowFileName(bool show);

public slots:
	virtual void accept();
};

#endif //MAPDIALOG_H_INCLUDED
