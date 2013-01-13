/*
 * misc.h is part of Brewtarget, and is Copyright Philip G. Lee
 * (rocketman768@gmail.com), 2009-2013.
 *
 * Brewtarget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Brewtarget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MISC_H
#define _MISC_H

#include <QString>
#include "BeerXMLElement.h"

// Forward declarations.
class Misc;

/*!
 * \class Misc
 * \author Philip G. Lee
 *
 * \brief Model for a misc record in the database.
 */
class Misc : public BeerXMLElement
{
   Q_OBJECT
   
   friend class Database;
public:
  
   //! \brief The type of ingredient.
   enum Type {Spice, Fining, Water_Agent, Herb, Flavor, Other}; // NOTE: BeerXML expects "Water Agent", but we can't have white space in enums :-/.
   //! \brief Where the ingredient is used.
   enum Use { Boil, Mash, Primary, Secondary, Bottling };
   Q_ENUMS( Type Use )
   
   virtual ~Misc() {}
   
   //! \brief The name.
   Q_PROPERTY( QString name READ name WRITE setName NOTIFY changedName )
   //! \brief The \c Type.
   Q_PROPERTY( Type type READ type WRITE setType /*NOTIFY changed*/ /*changedType*/ )
   //! \brief The  \c Type string.
   Q_PROPERTY( QString typeString READ typeString /*NOTIFY changed*/ STORED false )
   //! \brief The translated \c Type string.
   Q_PROPERTY( QString typeStringTr READ typeStringTr /*NOTIFY changed*/ STORED false )
   //! \brief The \c Use.
   Q_PROPERTY( Use use READ use WRITE setUse /*NOTIFY changed*/ /*changedUse*/ )
   //! \brief The \c Use string.
   Q_PROPERTY( QString useString READ useString /*NOTIFY changed*/ /*changedUse*/ STORED false )
   //! \brief The translated \c Use string.
   Q_PROPERTY( QString useStringTr READ useStringTr /*NOTIFY changed*/ /*changedUse*/ STORED false )
   //! \brief The time used in minutes.
   Q_PROPERTY( double time READ time WRITE setTime /*NOTIFY changed*/ /*changedTime*/ )
   //! \brief The amount in either kg or L, depending on \c amountIsWeight().
   Q_PROPERTY( double amount READ amount WRITE setAmount /*NOTIFY changed*/ /*changedAmount*/ )
   //! \brief Whether the amount is weight (kg), or volume (L).
   Q_PROPERTY( bool amountIsWeight READ amountIsWeight WRITE setAmountIsWeight /*NOTIFY changed*/ /*changedAmountIsWeight*/ )
   //! \brief What to use it for.
   Q_PROPERTY( QString useFor READ useFor WRITE setUseFor /*NOTIFY changed*/ /*changedUseFor*/ )
   //! \brief The notes.
   Q_PROPERTY( QString notes READ notes WRITE setNotes /*NOTIFY changed*/ /*changedNotes*/ )
   
   // Set
   void setName( const QString &var );
   void setType( Type t );
   void setUse( Use u );
   void setAmount( double var );
   void setTime( double var );
   void setAmountIsWeight( bool var );
   void setUseFor( const QString &var );
   void setNotes( const QString &var );
   
   // Get
   QString name() const;
   Type type() const;
   const QString typeString() const;
   const QString typeStringTr() const;
   Use use() const;
   const QString useString() const;
   const QString useStringTr() const;
   double amount() const;
   double time() const;
   bool amountIsWeight() const;
   QString useFor() const;
   QString notes() const;
   
signals:
   
   //! \brief Emitted when \c name() changes.
   void changedName(QString);
   /*
   void changedType(Type);
   void changedUse(Use);
   void changedTime(double);
   void changedAmount(double);
   void changedAmountIsWeight(bool);
   void changedUseFor(QString);
   void changedNotes(QString);
   */
   
private:
   Misc();
   Misc(Misc const& other);
   
   bool isValidType( const QString &var );
   bool isValidUse( const QString &var );
   
   static QStringList types;
   static QStringList uses;
   
   static QHash<QString,QString> tagToProp;
   static QHash<QString,QString> tagToPropHash();
};

inline bool MiscPtrLt( Misc* lhs, Misc* rhs)
{
   return lhs->name() < rhs->name();
}

inline bool MiscPtrEq( Misc* lhs, Misc* rhs)
{
   return lhs->name() == rhs->name();
}

struct Misc_ptr_cmp
{
   bool operator()( Misc* lhs, Misc* rhs)
   {
      return lhs->name() < rhs->name();
   }
};

struct Misc_ptr_equals
{
   bool operator()( Misc* lhs, Misc* rhs )
   {
      return lhs->name() == rhs->name();
   }
};

#endif   /* _MISC_H */
