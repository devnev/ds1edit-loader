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

#include "aboutdialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QDialogButtonBox>

AboutDialog::AboutDialog(QWidget* parent)
: QDialog(parent, Qt::MSWindowsFixedSizeDialogHint)
{
	QGridLayout* topLevelLayout = new QGridLayout(this);

	QLabel* icon = new QLabel(this);
	icon->setPixmap(QPixmap(":/res/ds1edit loader.png"));
	icon->setAlignment(Qt::AlignTop);
	topLevelLayout->addWidget(icon, 0, 0, 3, 1);

	QLabel* copyright = new QLabel(tr("ds1edit Loader version (2.0.0)\n"
		"Copyright \251 Mark Nevill 2002-2007"), this);
	topLevelLayout->addWidget(copyright, 0, 1, 1, 1);

	QLabel* feedback = new QLabel(tr("<p>For comments, feedback and updates visit:\n"
		"<a href=\"http://www.d2mods.com/forum/viewtopic.php?t=21281\">"
		"The Phrozen Keep</a></p>"), this);
	feedback->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
	feedback->setOpenExternalLinks(true);
	topLevelLayout->addWidget(feedback, 1, 1, 1, 1);

	QLabel* legal = new QLabel(tr("This program is distributed in the hope that it will be useful, "
		"but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or "
		"FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n"
		"You should have received a copy of the GNU General Public License along with this program; "
		"if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, "
		"MA 02110-1301 USA"), this);
	legal->setWordWrap(true);
	topLevelLayout->addWidget(legal, 2, 1, 1, 2);

	QDialogButtonBox* buttons = new QDialogButtonBox(
		QDialogButtonBox::Ok, Qt::Vertical, this);
	topLevelLayout->addWidget(buttons, 0, 2, 2, 1);

	connect(buttons, SIGNAL(accepted()), SLOT(accept()));

	setWindowTitle(tr("About ds1edit Loader"));
	setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
}
