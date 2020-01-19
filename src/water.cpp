/*
 * water.cpp is part of Brewtarget, and is Copyright the following
 * authors 2009-2014
 * - Philip Greggory Lee <rocketman768@gmail.com>
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

#include <QVector>
#include "water.h"
#include "brewtarget.h"
#include <QDomElement>
#include <QDomText>
#include <QObject>
#include "water.h"
#include "brewtarget.h"

#include "TableSchemaConst.h"
#include "WaterSchema.h"

bool operator<(Water &w1, Water &w2)
{
   return w1.name() < w2.name();
}

bool operator==(Water &w1, Water &w2)
{
   return w1.name() == w2.name();
}

QString Water::classNameStr()
{
   static const QString name("Water");
   return name;
}

Water::Water(Brewtarget::DBTable table, int key)
   : BeerXMLElement(table, key),
   m_amount_l(0.0),
   m_calcium_ppm(0.0),
   m_bicarbonate_ppm(0.0),
   m_sulfate_ppm(0.0),
   m_chloride_ppm(0.0),
   m_sodium_ppm(0.0),
   m_magnesium_ppm(0.0),
   m_ph(0.0),
   m_alkalinity(0.0),
   m_notes(QString()),
   m_cacheOnly(false),
   m_type(NONE)
{
}

Water::Water(QString name, bool cache)
   : BeerXMLElement(Brewtarget::WATERTABLE, -1, name, true),
   m_amount_l(0.0),
   m_calcium_ppm(0.0),
   m_bicarbonate_ppm(0.0),
   m_sulfate_ppm(0.0),
   m_chloride_ppm(0.0),
   m_sodium_ppm(0.0),
   m_magnesium_ppm(0.0),
   m_ph(0.0),
   m_alkalinity(0.0),
   m_notes(QString()),
   m_cacheOnly(cache),
   m_type(NONE)
{
}

Water::Water(Water const& other, bool cache)
   : BeerXMLElement(Brewtarget::WATERTABLE, -1, other.name(), true),
   m_amount_l(other.m_amount_l),
   m_calcium_ppm(other.m_calcium_ppm),
   m_bicarbonate_ppm(other.m_bicarbonate_ppm),
   m_sulfate_ppm(other.m_sulfate_ppm),
   m_chloride_ppm(other.m_chloride_ppm),
   m_sodium_ppm(other.m_sodium_ppm),
   m_magnesium_ppm(other.m_magnesium_ppm),
   m_ph(other.m_ph),
   m_alkalinity(other.m_alkalinity),
   m_notes(other.m_notes),
   m_cacheOnly(cache),
   m_type(other.m_type)
{
}

Water::Water(Brewtarget::DBTable table, int key, QSqlRecord rec)
   : BeerXMLElement(table, key, rec.value(kcolName).toString(), rec.value(kcolDisplay).toBool(), rec.value(kcolFolder).toString()),
   m_amount_l(rec.value(kcolAmount).toDouble()),
   m_calcium_ppm(rec.value(kcolWaterCalcium).toDouble()),
   m_bicarbonate_ppm(rec.value(kcolWaterBiCarbonate).toDouble()),
   m_sulfate_ppm(rec.value(kcolWaterSulfate).toDouble()),
   m_chloride_ppm(rec.value(kcolWaterChloride).toDouble()),
   m_sodium_ppm(rec.value(kcolWaterSodium).toDouble()),
   m_magnesium_ppm(rec.value(kcolWaterMagnesium).toDouble()),
   m_ph(rec.value(kcolPH).toDouble()),
   m_alkalinity(rec.value(kcolWaterAlkalinity).toDouble()),
   m_notes(rec.value(kcolAmount).toString()),
   m_cacheOnly(false),
   m_type(static_cast<Water::Types>(rec.value(kcolWaterType).toInt()))
{
}

//================================"SET" METHODS=================================
void Water::setAmount_l( double var )
{
   m_amount_l = var;
   if ( ! m_cacheOnly ) {
      setEasy(kpropAmount, var);
   }
}

void Water::setCalcium_ppm( double var )
{
   m_calcium_ppm = var;
   if ( ! m_cacheOnly ) {
      setEasy(kpropCalcium, var);
   }
}

void Water::setBicarbonate_ppm( double var )
{
   m_bicarbonate_ppm = var;
   if ( ! m_cacheOnly ) {
      setEasy(kpropBiCarbonate, var);
   }
}

void Water::setChloride_ppm( double var )
{
   m_chloride_ppm = var;
   if ( ! m_cacheOnly ) {
      setEasy(kpropChloride, var);
   }
}

void Water::setSodium_ppm( double var )
{
   m_sodium_ppm = var;
   if ( ! m_cacheOnly ) {
      setEasy(kpropSodium, var);
   }
}

void Water::setMagnesium_ppm( double var )
{
   m_magnesium_ppm = var;
   if ( ! m_cacheOnly ) {
      setEasy(kpropMagnesium, var);
   }
}

void Water::setPh( double var )
{
   m_ph = var;
   if ( ! m_cacheOnly ) {
      setEasy(kpropPH, var);
   }
}

void Water::setAlkalinity(double var)
{
   m_alkalinity = var;
   if ( ! m_cacheOnly ) {
      setEasy(kpropAlkalinity, var);
   }
}

void Water::setSulfate_ppm( double var )
{
   m_sulfate_ppm = var;
   if ( ! m_cacheOnly ) {
      setEasy(kpropSulfate, var);
   }
}

void Water::setNotes( const QString &var )
{
   m_notes = var;
   if ( ! m_cacheOnly ) {
      setEasy(kpropNotes, var);
   }
}

void Water::setCacheOnly(bool cache) { m_cacheOnly = cache; }
void Water::setType(Types type)
{
   if ( type < NONE || type > SPARGE ) {
      return;
   }

   m_type = type;
   if ( ! m_cacheOnly ) {
      setEasy(kpropType, type);
   }
}

//=========================="GET" METHODS=======================================
QString Water::notes() const { return m_notes; }
double Water::sulfate_ppm() const { return m_sulfate_ppm; }
double Water::amount_l() const { return m_amount_l; }
double Water::calcium_ppm() const { return m_calcium_ppm; }
double Water::bicarbonate_ppm() const { return m_bicarbonate_ppm; }
double Water::chloride_ppm() const { return m_chloride_ppm; }
double Water::sodium_ppm() const { return m_sodium_ppm; }
double Water::magnesium_ppm() const { return m_magnesium_ppm; }
double Water::ph() const { return m_ph; }
double Water::alkalinity() const { return m_alkalinity; }
bool Water::cacheOnly() const { return m_cacheOnly; }
Water::Types Water::type() const { return m_type; }
