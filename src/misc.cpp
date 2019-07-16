/*
 * misc.cpp is part of Brewtarget, and is Copyright the following
 * authors 2009-2014
 * - mik firestone <mikfire@gmail.com>
 * - Philip Greggory Lee <rocketman768@gmail.com>
 * - Samuel Östling <MrOstling@gmail.com>
 *
 * Brewtarget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Brewtarget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "brewtarget.h"
#include <iostream>
#include <string>
#include <QVector>
#include "misc.h"
#include "brewtarget.h"
#include <QDomElement>
#include <QDomText>
#include <QObject>
#include <QDebug>


/************* Columns *************/
const QString kName("name");
const QString kType("mtype");
const QString kUse("use");
const QString kTime("time");
const QString kAmount("amount");
const QString kAmountIsWeight("amount_is_weight");
const QString kInventory("inventory");
const QString kUseFor("use_for");
const QString kNotes("notes");

// these are defined in the parent, but I need them here too
const QString kDeleted("deleted");
const QString kDisplay("display");
const QString kFolder("folder");

/************** Props **************/
const QString kNameProp("name");
const QString kTypeProp("type");
const QString kUseProp("use");
const QString kTimeProp("time");
const QString kAmountProp("amount");
const QString kAmountIsWeightProp("amountIsWeight");
const QString kInventoryProp("inventory");
const QString kUseForProp("useFor");
const QString kNotesProp("notes");

QStringList Misc::uses = QStringList() << "Boil" << "Mash" << "Primary" << "Secondary" << "Bottling";
QStringList Misc::types = QStringList() << "Spice" << "Fining" << "Water Agent" << "Herb" << "Flavor" << "Other";
QStringList Misc::amountTypes = QStringList() << "Weight" << "Volume";
QHash<QString,QString> Misc::tagToProp = Misc::tagToPropHash();

QHash<QString,QString> Misc::tagToPropHash()
{
   QHash<QString,QString> propHash;
   propHash["NAME"] = kNameProp;
   //propHash["TYPE"] = kTypeProp;
   //propHash["USE"] = kUseProp";
   propHash["TIME"] = kTimeProp;
   propHash["AMOUNT"] = kAmountProp;
   propHash["AMOUNT_IS_WEIGHT"] = kAmountIsWeightProp;
   propHash["INVENTORY"] = kInventoryProp;
   propHash["USE_FOR"] = kUseForProp;
   propHash["NOTES"] = kNotesProp;
   return propHash;
}

QString Misc::classNameStr()
{
   static const QString name("Misc");
   return name;
}

//============================CONSTRUCTORS======================================
Misc::Misc(Brewtarget::DBTable table, int key) 
   : BeerXMLElement(table, key)
{
}

Misc::Misc(Brewtarget::DBTable table, int key, QSqlRecord rec)
   : BeerXMLElement(table, key, rec.value(kName).toString(), rec.value(kDisplay).toBool()),
   m_typeString(rec.value(kType).toString()),
   m_type(static_cast<Misc::Type>(types.indexOf(m_typeString))),
   m_useString(rec.value(kUse).toString()),
   m_use(static_cast<Misc::Use>(uses.indexOf(m_useString))),
   m_time(rec.value(kTime).toDouble()),
   m_amount(rec.value(kAmount).toDouble()),
   m_amountIsWeight(rec.value(kAmountIsWeight).toBool()),
   m_useFor(rec.value(kUseFor).toString()),
   m_notes(rec.value(kNotes).toString())
{
}

Misc::Misc(Misc const& other) : BeerXMLElement(other)
{
}

//============================"GET" METHODS=====================================
Misc::Type Misc::type() const { return m_type; }

const QString Misc::typeString() const { return m_typeString; }

Misc::Use Misc::use() const { return m_use; }

const QString Misc::useString() const { return m_useString; }

double Misc::amount()    const { return m_amount; }

double Misc::time()      const { return m_time; }

bool Misc::amountIsWeight() const { return m_amountIsWeight; }

QString Misc::useFor() const { return m_useFor; }

QString Misc::notes() const { return m_notes; }

double Misc::inventory() const
{
   return getInventory(kAmount).toDouble();
}

Misc::AmountType Misc::amountType() const { return m_amountIsWeight ? AmountType_Weight : AmountType_Volume; }

const QString Misc::amountTypeString() const { return amountTypes.at(amountType()); }

const QString Misc::typeStringTr() const
{
   QStringList typesTr = QStringList() << tr("Spice") << tr("Fining") << tr("Water Agent") << tr("Herb") << tr("Flavor") << tr("Other");
   return typesTr.at(type());
}

const QString Misc::useStringTr() const
{
   QStringList usesTr = QStringList() << tr("Boil") << tr("Mash") << tr("Primary") << tr("Secondary") << tr("Bottling");
   return usesTr.at(use());
}

const QString Misc::amountTypeStringTr() const
{
   QStringList amountTypesTr = QStringList() << tr("Weight") << tr("Volume");
   return amountTypesTr.at(amountType());
}

//============================"SET" METHODS=====================================
void Misc::setType( Type t )
{
   m_type = t;
   m_typeString = types.at(t);
   set( kTypeProp, kType, m_typeString );
}

void Misc::setUse( Use u )
{
   m_use = u;
   m_useString = uses.at(u);
   set( kUseProp, kUse, m_useString );
}

void Misc::setUseFor( const QString& var )
{
   m_useFor = var;
   set( kUseForProp, kUseFor, var );
}

void Misc::setNotes( const QString& var )
{
   m_notes = var;
   set( kNotesProp, kNotes, var );
}

void Misc::setAmountType( AmountType t )
{
   m_amountIsWeight = t == AmountType_Weight;
   setAmountIsWeight(m_amountIsWeight);
}

void Misc::setAmountIsWeight( bool var )
{
   m_amountIsWeight = var;
   set( kAmountIsWeightProp, kAmountIsWeight, var );
}

void Misc::setAmount( double var )
{
   if( var < 0.0 )
      Brewtarget::logW( QString("Misc: amount < 0: %1").arg(var) );
   else {
      m_amount = var;
      set( kAmountProp, kAmount, var );
   }
}

void Misc::setInventoryAmount( double var )
{
   if( var < 0.0 )
      Brewtarget::logW( QString("Misc: inventory < 0: %1").arg(var) );
   else
      setInventory(kInventoryProp, kAmount, var );
}

void Misc::setTime( double var )
{
   if( var < 0.0 )
      Brewtarget::logW( QString("Misc: time < 0: %1").arg(var) );
   else {
      m_time = var;
      set( kTimeProp, kTime, var );
   }
}

//========================OTHER METHODS=========================================

bool Misc::isValidUse( const QString& var )
{
   static const QString uses[] = {"Boil", "Mash", "Primary", "Secondary", "Bottling"};
   static const unsigned int size = 5;
   unsigned int i;
   
   for( i = 0; i < size; ++i )
      if( var == uses[i] )
         return true;
   
   return false;
}

bool Misc::isValidType( const QString& var )
{
   static const QString types[] = {"Spice", "Fining", "Water Agent", "Herb", "Flavor", "Other"};
   static const unsigned int size = 6;
   unsigned int i;
   
   for( i = 0; i < size; ++i )
      if( var == types[i] )
         return true;
   
   return false;
}
