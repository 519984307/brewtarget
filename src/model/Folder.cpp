/*╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌
 * model/Folder.cpp is part of Brewken, and is copyright the following authors 2009-2024:
 *   • Matt Young <mfsy@yahoo.com>
 *   • Mik Firestone <mikfire@gmail.com>
 *   • Philip Greggory Lee <rocketman768@gmail.com>
 *
 * Brewtarget is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Brewtarget is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 ╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌*/
#include "model/Folder.h"

#include <QDebug>
#include <QString>

Folder::Folder() : QObject() {
   setObjectName("Folder");
   return;
}

Folder::Folder(Folder const & other) :
   QObject(),
   m_name    {other.m_name    },
   m_path    {other.m_path    },
   m_fullPath{other.m_fullPath} {
   setObjectName("Folder");
   return;
}

Folder::~Folder() = default;

QString Folder::name() const { return m_name; }
QString Folder::path() const { return m_path; }
QString Folder::fullPath() const { return m_fullPath; }

// changing the name changes the fullPath
void Folder::setName(QString var) {
   m_name = var;
   m_fullPath = m_path.append("/").append(m_name);
   return;
}

// changing the path changes the fullPath
void Folder::setPath(QString var) {
   m_path = var;
   m_fullPath = m_path.append("/").append(m_name);
   return;
}

// changing the full path necessarily changes the name and the path
void Folder::setfullPath(QString var) {
#if QT_VERSION < QT_VERSION_CHECK(5,15,0)
   QStringList pieces = var.split("/", QString::SkipEmptyParts);
#else
   QStringList pieces = var.split("/", Qt::SkipEmptyParts);
#endif

   if (!pieces.isEmpty()) {
      m_name = pieces.last();
      pieces.removeLast();
      m_path = pieces.join("/");

      m_fullPath = var;

   } else {
      m_name = var;
      m_path = var;
      m_fullPath = var;
   }
   return;
}

bool Folder::isFolder(QString var) {
   return m_fullPath == var;
}
