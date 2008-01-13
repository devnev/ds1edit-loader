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

#include "mapdialog.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QIntValidator>
#include "ds1info.h"
#include <QTreeWidgetItem>
#include <cassert>

MapDialog::MapDialog(bool allowEmpties, QWidget* parent)
: QDialog(parent), empties(allowEmpties)
{
	QGridLayout* topLevelLayout = new QGridLayout(this);
	QLabel* label;

	fileLabel = new QLabel(tr("File:"), this);
	fileLabel->setAlignment(Qt::AlignTrailing | Qt::AlignVCenter);
	topLevelLayout->addWidget(fileLabel, 0 , 0);
	fileWidget = new QLineEdit(this);
	topLevelLayout->addWidget( fileWidget , 0 , 1);

#define ADDMAPPROP(text, row, widget) \
	label = new QLabel(tr(text), this); \
	label->setAlignment(Qt::AlignTrailing | Qt::AlignVCenter); \
	topLevelLayout->addWidget(label, row , 0); \
	widget = new QLineEdit(this); \
	topLevelLayout->addWidget( widget , row , 1); \

	ADDMAPPROP("Name:", 1, nameWidget);
	ADDMAPPROP("Path:", 2, pathWidget);
	ADDMAPPROP("LvlType ID:", 3, typeIdWidget);
	ADDMAPPROP("LvlPrest Def:", 4, prestDefWidget);
	ADDMAPPROP("Comment:", 5, commentWidget);
#undef ADDMAPPROP

	QIntValidator* typeIdValidator = new QIntValidator(1, 999, this);
	typeIdWidget->setValidator(typeIdValidator);
	QIntValidator* prestDefValidator = new QIntValidator(-1, 99999, this);
	prestDefWidget->setValidator(prestDefValidator);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	topLevelLayout->addWidget(buttonBox, 6, 0, 1, 2);
	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

	setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
}

void MapDialog::accept()
{
	if ((empties || fileWidget->text().length()!=0 || !showFileWidget) &&
		(empties || nameWidget->text().length()!=0) &&
		(empties || pathWidget->text().length()!=0) &&
		(typeIdWidget->hasAcceptableInput() ||
			(empties && typeIdWidget->text().length()==0)) &&
		(prestDefWidget->hasAcceptableInput() ||
			(empties && prestDefWidget->text().length()==0)) &&
		prestDefWidget->text() != "0") // -1 is acceptable, 0 is not, >0 is.
	{
		QDialog::accept();
	}
}

void MapDialog::setShowFileName(bool show)
{
	fileLabel->setVisible(show);
	fileWidget->setVisible(show);
	showFileWidget = show;
}

void MapDialog::fromMap(Ds1Info& map)
{
	setFile(map.file);
	setName(map.name);
	setPath(map.path);
	setTypeId(map.typeId);
	setPrestDef(map.prestDef);
	setComment(map.comment);
}

void MapDialog::fromMapListItem(QTreeWidgetItem* listItem)
{
	setFile(listItem->text(0));
	setName(listItem->text(1));
	setPath(listItem->text(2));
	setTypeId(listItem->text(3));
	setPrestDef(listItem->text(4));
	setComment(listItem->text(5));
}

Ds1Info MapDialog::toMap()
{
	Ds1Info map = {
		file(),
		name(),
		path(),
		typeId().toInt(),
		prestDef().toInt(),
		comment()
	};
	return map;
}
