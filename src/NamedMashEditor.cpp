/*
 * NamedMashEditor.cpp is part of Brewtarget, and is Copyright Mik Firestone
 * (mikfire@gmail.com), 2013-2015
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

#include <QWidget>
#include <QDebug>

#include "mash.h"
#include "brewtarget.h"
#include "unit.h"
#include "equipment.h"
#include "recipe.h"
#include "database.h"

#include "NamedMashEditor.h"

NamedMashEditor::NamedMashEditor(QWidget* parent, MashStepEditor* editor, bool singleMashEditor) 
   : QDialog(parent), mashObs(0)
{
   setupUi(this);

   if ( singleMashEditor ) 
   {
      for (int i = 0; i < horizontalLayout_mashs->count(); ++i)
      {
         QWidget* w = horizontalLayout_mashs->itemAt(i)->widget();
         if (w)
            w->setVisible(false);
      }
      // pushButton_new->setVisible(false);
   }
   
   //! Create the list model and assign it to the combo box
   mashListModel = new MashListModel(mashComboBox);
   mashComboBox->setModel( mashListModel );

   //! Create the table model (and may St. Stevens take pity)
   mashStepTableModel = new MashStepTableModel(mashStepTableWidget);
   mashStepTableWidget->setItemDelegate(new MashStepItemDelegate());
   mashStepTableWidget->setModel(mashStepTableModel);

   //! Preserve the step editor
   mashStepEditor = editor;

   //! And do some fun stuff with the equipment
   equipListModel = new EquipmentListModel(equipmentComboBox);
   equipmentComboBox->setModel(equipListModel);
   connect(equipmentComboBox, SIGNAL(activated(const QString&)), this, SLOT(fromEquipment(const QString&)));

   // ok and cancel buttons
   connect(pushButton_save,           SIGNAL(clicked()), this, SLOT(saveAndClose()) );
   connect(pushButton_cancel,         SIGNAL(clicked()), this, SLOT(closeEditor()) );
   // new mash step, delete mash step, move mash step up and down
   connect(pushButton_addMashStep,    SIGNAL(clicked()), this, SLOT(addMashStep()));
   connect(pushButton_removeMashStep, SIGNAL(clicked()), this, SLOT(removeMashStep()));
   connect(pushButton_mashUp,         SIGNAL(clicked()), this, SLOT(moveMashStepUp()));
   connect(pushButton_mashDown,       SIGNAL(clicked()), this, SLOT(moveMashStepDown()));
   // finally, the combo box and the remove mash button
   connect(mashComboBox, SIGNAL(activated(const QString&)), this, SLOT(mashSelected(const QString&)));
   connect(pushButton_remove, SIGNAL(clicked()), this, SLOT(removeMash()));
   
   setMash(mashListModel->at(mashComboBox->currentIndex()));

   connect( lineEdit_grainTemp,  SIGNAL(editingFinished()), this, SLOT(updateField()));
   connect( lineEdit_spargeTemp, SIGNAL(editingFinished()), this, SLOT(updateField()));
   connect( lineEdit_tunTemp,    SIGNAL(editingFinished()), this, SLOT(updateField()));
   connect( lineEdit_tunMass,    SIGNAL(editingFinished()), this, SLOT(updateField()));
   connect( lineEdit_spargePh,   SIGNAL(editingFinished()), this, SLOT(updateField()));
   connect( lineEdit_tunSpHeat,  SIGNAL(editingFinished()), this, SLOT(updateField()));

}

void NamedMashEditor::updateField()
{

   QObject* selection = sender();
   QLineEdit* field = qobject_cast<QLineEdit*>(selection);
   double val;
 
   // temps first 
   if ( field == lineEdit_grainTemp || field == lineEdit_spargeTemp || field == lineEdit_tunTemp )
   {
      val = Brewtarget::tempQStringToSI(field->text());
      field->setText(Brewtarget::displayAmount( val, Units::celsius));
   }
   else if ( field == lineEdit_tunMass ) 
   {
      // odd ball
      val = Brewtarget::weightQStringToSI(field->text());
      field->setText(Brewtarget::displayAmount( val, Units::kilograms));
   }
   else 
   {
      //just format everything else nicely
      val = field->text().toDouble();
      field->setText(Brewtarget::displayAmount(val));
   }

}

void NamedMashEditor::showEditor()
{
   showChanges();
   setVisible(true);
}

void NamedMashEditor::closeEditor()
{
   setVisible(false);
}

void NamedMashEditor::saveAndClose()
{
   if( mashObs == 0 )
      return;
   
   //mash->disableNotification(); // If we don't do this, the notification will propagate to a showChanges() and we'll lose any info we want saved.
   mashObs->setEquipAdjust( true ); // BeerXML won't like me, but it's just stupid not to adjust for the equipment when you're able.

   mashObs->setName( lineEdit_name->text() );
   mashObs->setGrainTemp_c(Brewtarget::tempQStringToSI(lineEdit_grainTemp->text()));
   mashObs->setSpargeTemp_c(Brewtarget::tempQStringToSI(lineEdit_spargeTemp->text()));
   mashObs->setPh(lineEdit_spargePh->text().toDouble());
   mashObs->setTunTemp_c(Brewtarget::tempQStringToSI(lineEdit_tunTemp->text()));
   mashObs->setTunWeight_kg(Brewtarget::weightQStringToSI(lineEdit_tunMass->text()));
   mashObs->setTunSpecificHeat_calGC(lineEdit_tunSpHeat->text().toDouble() );

   mashObs->setNotes( textEdit_notes->toPlainText() );
   
   //mash->reenableNotification();
   //mash->forceNotify();
}

void NamedMashEditor::setMash(Mash* mash)
{
   if( mashObs )
      disconnect( mashObs, 0, this, 0 );
   
   mashObs = mash;
   mashStepTableModel->setMash(mashObs);

   if( mashObs )
   {
      connect( mashObs, SIGNAL(changed(QMetaProperty,QVariant)), this, SLOT(changed(QMetaProperty,QVariant)) );
      showChanges();
   }
}

void NamedMashEditor::changed(QMetaProperty prop, QVariant /*val*/)
{
   if( sender() == mashObs )
      showChanges(&prop);
}

void NamedMashEditor::showChanges(QMetaProperty* prop)
{
   bool updateAll = false;
   QString propName;

   if( mashObs == 0 )
   {
      clear();
      return;
   }

   if( prop == 0 )
      updateAll = true;
   else
      propName = prop->name();

   if( propName == "name" || updateAll ) {
      lineEdit_name->setText(mashObs->name());
      if( ! updateAll )
         return;
   }
   if( propName == "grainTemp_c" || updateAll ) {
      lineEdit_grainTemp->setText(Brewtarget::displayAmount(mashObs->grainTemp_c(), Units::celsius));
      if( ! updateAll )
         return;
   }
   if( propName == "spargeTemp_c" || updateAll ) {
      lineEdit_spargeTemp->setText(Brewtarget::displayAmount(mashObs->spargeTemp_c(), Units::celsius));
      if( ! updateAll )
         return;
   }
   if( propName == "ph" || updateAll ) {
      lineEdit_spargePh->setText(Brewtarget::displayAmount(mashObs->ph()));
      if( ! updateAll )
         return;
   }
   if( propName == "tunTemp_c" || updateAll ) {
      lineEdit_tunTemp->setText(Brewtarget::displayAmount(mashObs->tunTemp_c(), Units::celsius));
      if( ! updateAll )
         return;
   }
   if( propName == "tunMass_kg" || updateAll ) {
      lineEdit_tunMass->setText(Brewtarget::displayAmount(mashObs->tunWeight_kg(), Units::kilograms));
      if( ! updateAll )
         return;
   }
   if( propName == "tunSpecificHeat_calGC" || updateAll ) {
      lineEdit_tunSpHeat->setText(Brewtarget::displayAmount(mashObs->tunSpecificHeat_calGC()));
      if( ! updateAll )
         return;
   }
   if( propName == "notes" || updateAll ) {
      textEdit_notes->setPlainText(mashObs->notes());
      if( ! updateAll )
         return;
   }
}

void NamedMashEditor::clear()
{
   lineEdit_name->setText("");
   lineEdit_grainTemp->setText("");
   lineEdit_spargeTemp->setText("");
   lineEdit_spargePh->setText("");
   lineEdit_tunTemp->setText("");
   lineEdit_tunMass->setText("");
   lineEdit_tunSpHeat->setText("");

   textEdit_notes->setPlainText("");

}

void NamedMashEditor::addMashStep() 
{
   if ( ! mashObs )
      return;

   MashStep* step = Database::instance().newMashStep(mashObs);
   mashStepEditor->setMashStep(step);
   mashStepEditor->setVisible(true);
}

bool NamedMashEditor::justOne(QModelIndexList selected)
{
   int row, size, i;

   size = selected.size();
   if ( ! size )
      return false;

   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if ( selected[i].row() != row )
         return false;
   }
   return true;
}

void NamedMashEditor::removeMashStep()
{
   if ( ! mashObs )
      return;

   QModelIndexList selected = mashStepTableWidget->selectionModel()->selectedIndexes();
   if ( !justOne(selected) )
      return;

   MashStep* step = mashStepTableModel->getMashStep(selected[0].row());
   Database::instance().removeFrom(mashObs, step);
}

void NamedMashEditor::moveMashStepUp()
{
   QModelIndexList selected = mashStepTableWidget->selectionModel()->selectedIndexes();
   int row = selected[0].row();

   if ( ! justOne(selected) || row < 1)
      return;

   MashStep* m1 = mashStepTableModel->getMashStep(row);
   MashStep* m2 = mashStepTableModel->getMashStep(row-1);
   Database::instance().swapMashStepOrder(m1,m2);
}

void NamedMashEditor::moveMashStepDown()
{
   QModelIndexList selected = mashStepTableWidget->selectionModel()->selectedIndexes();
   int row = selected[0].row();

   if ( ! justOne(selected) || row >= mashStepTableModel->rowCount()-1 )
      return;

   MashStep* m1 = mashStepTableModel->getMashStep(row);
   MashStep* m2 = mashStepTableModel->getMashStep(row+1);
   Database::instance().swapMashStepOrder(m1,m2);
}

void NamedMashEditor::mashSelected(const QString& name)
{
   Mash* selected = mashListModel->at(mashComboBox->currentIndex());
   if (selected && selected != mashObs)
      setMash(selected);
}

void NamedMashEditor::fromEquipment(const QString& name)
{
   if( mashObs == 0 )
      return;
   Equipment* selected = equipListModel->at(equipmentComboBox->currentIndex());

   if ( selected )
   {
      lineEdit_tunMass->setText(Brewtarget::displayAmount(selected->tunWeight_kg(), Units::kilograms));
      lineEdit_tunSpHeat->setText(Brewtarget::displayAmount(selected->tunSpecificHeat_calGC()));
   }
}

void NamedMashEditor::removeMash()
{
   if ( ! mashObs )
      return;
   
   int newMash = mashComboBox->currentIndex() - 1;

   // I *think* we want to disconnect the mash first?
   disconnect(mashObs, 0, this, 0);
   // Delete the mashsteps
   QList<MashStep*> steps = mashObs->mashSteps();
   Database::instance().remove(steps);
   // and delete the mash itself
   Database::instance().remove(mashObs);
   setMash(mashListModel->at(newMash));
}
