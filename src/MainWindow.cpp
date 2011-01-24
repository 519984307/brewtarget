/*
 * MainWindow.cpp is part of Brewtarget, and is Copyright Philip G. Lee
 * (rocketman768@gmail.com), 2009-2011.
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

#include "MashStepEditor.h"
#include "MashStepTableModel.h"
#include "mash.h"
#include "MashEditor.h"
#include <QWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QToolButton>
#include <QSize>
#include "brewtarget.h"
#include "FermentableEditor.h"
#include "MiscEditor.h"
#include "HopEditor.h"
#include "YeastEditor.h"
#include "YeastTableModel.h"
#include "MiscTableModel.h"
#include <QtGui>
#include "style.h"
#include <QString>
#include <QFileDialog>
#include <QIcon>
#include <QPixmap>
#include <iostream>
#include <fstream>
#include <QList>
#include <QVector>
#include "recipe.h"
#include "MainWindow.h"
#include "AboutDialog.h"
#include "database.h"
#include "MiscTableWidget.h"
#include "YeastTableWidget.h"
#include "YeastDialog.h"
#include "BeerColorWidget.h"
#include "config.h"
#include "unit.h"
#include <QVBoxLayout>
#include <QDomDocument>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDomNodeList>
#include <QDomNode>
#include <QDomElement>
#include "ScaleRecipeTool.h"
#include "HopTableModel.h"
#include <QInputDialog>
#include <QLineEdit>
#include "BtDigitWidget.h"
#include <QUrl>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent)
{
   // Need to call this to get all the widgets added (I think).
   setupUi(this);

   if( Database::isInitialized() )
      db = Database::getDatabase();
   else
   {
      Database::initialize();
      db = Database::getDatabase();
   }

   //setWindowIcon(QIcon(ICON48));
   
   // Different palettes for some text.
   lcdPalette_old = lcdNumber_og->palette();
   lcdPalette_tooLow = QPalette(lcdPalette_old);
   lcdPalette_tooLow.setColor(QPalette::Active, QPalette::WindowText, QColor::fromRgb(0, 0, 208));
   lcdPalette_good = QPalette(lcdPalette_old);
   lcdPalette_good.setColor(QPalette::Active, QPalette::WindowText, QColor::fromRgb(0, 128, 0));
   lcdPalette_tooHigh = QPalette(lcdPalette_old);
   lcdPalette_tooHigh.setColor(QPalette::Active, QPalette::WindowText, QColor::fromRgb(208, 0, 0));

   // Set constant colors for some lcds.
   lcdNumber_ogLow->setConstantColor(BtDigitWidget::LOW);
   lcdNumber_ogHigh->setConstantColor(BtDigitWidget::HIGH);
   lcdNumber_fgLow->setConstantColor(BtDigitWidget::LOW);
   lcdNumber_fgHigh->setConstantColor(BtDigitWidget::HIGH);
   lcdNumber_abvLow->setConstantColor(BtDigitWidget::LOW);
   lcdNumber_abvHigh->setConstantColor(BtDigitWidget::HIGH);
   lcdNumber_ibuLow->setConstantColor(BtDigitWidget::LOW);
   lcdNumber_ibuHigh->setConstantColor(BtDigitWidget::HIGH);
   lcdNumber_srmLow->setConstantColor(BtDigitWidget::LOW);
   lcdNumber_srmHigh->setConstantColor(BtDigitWidget::HIGH);
   lcdNumber_boilSG->setConstantColor(BtDigitWidget::BLACK);
   lcdNumber_ibugu->setConstantColor(BtDigitWidget::BLACK);

   // Null out the recipe
   recipeObs = 0;

   // Make the fermentable table show grain percentages in row headers.
   fermentableTable->getModel()->setDisplayPercentages(true);
   // Hop table show IBUs in row headers.
   hopTable->getModel()->setShowIBUs(true);
   
   dialog_about = new AboutDialog(this);
   equipEditor = new EquipmentEditor(this);
   fermDialog = new FermentableDialog(this);
   fermEditor = new FermentableEditor(this);
   hopDialog = new HopDialog(this);
   hopEditor = new HopEditor(this);
   mashEditor = new MashEditor(this);
   mashStepEditor = new MashStepEditor(this);
   mashWizard = new MashWizard(this);
   miscDialog = new MiscDialog(this);
   miscEditor = new MiscEditor(this);
   styleEditor = new StyleEditor(this);
   yeastDialog = new YeastDialog(this);
   yeastEditor = new YeastEditor(this);
   optionDialog = new OptionDialog(this);
   htmlViewer = new HtmlViewer(this);
   recipeScaler = new ScaleRecipeTool(this);
   recipeFormatter = new RecipeFormatter();
   ogAdjuster = new OgAdjuster();
   converterTool = new ConverterTool();
   timerListDialog = new TimerListDialog(this);
   mashComboBox = new MashComboBox(this);
   primingDialog = new PrimingDialog(this);
   refractoDialog = new RefractoDialog(this);
   mashDesigner = new MashDesigner(this);
   pitchDialog = new PitchDialog(this);

   setupToolbar();

   // Set up the printer
   printer = new QPrinter;
   printer->setPageSize(QPrinter::Letter);

   // Set up brewDayDialog
   /*
   brewDayDialog->setModal(false);
   QVBoxLayout* vblayout = new QVBoxLayout();
   vblayout->addWidget(brewDayWidget);
   brewDayDialog->setLayout(vblayout);
   brewDayDialog->setWindowTitle(tr("Brew day mode"));
   */

   // Set up the fileOpener dialog.
   fileOpener = new QFileDialog(this, tr("Open"), QDir::homePath(), tr("BeerXML files (*.xml)"));
   fileOpener->setAcceptMode(QFileDialog::AcceptOpen);
   fileOpener->setFileMode(QFileDialog::ExistingFiles);
   fileOpener->setViewMode(QFileDialog::List);

   // Set up the fileSaver dialog.
   fileSaver = new QFileDialog(this, tr("Save"), QDir::homePath(), tr("BeerXML files (*.xml)") );
   fileSaver->setAcceptMode(QFileDialog::AcceptSave);
   fileSaver->setFileMode(QFileDialog::AnyFile);
   fileSaver->setViewMode(QFileDialog::List);
   fileSaver->setDefaultSuffix(QString("xml"));

   // Set up and place the BeerColorWidget
   //verticalLayout_beerColor->insertWidget( 0, &beerColorWidget);

   // And test out the maltiness widget.
   maltWidget = new MaltinessWidget(groupBox);
   verticalLayout_beerColor->insertWidget( -1, maltWidget );
   horizontalLayout_mash->insertWidget( 1, mashComboBox );

   // Set up HtmlViewer to view documentation.
   htmlViewer->setHtml(Brewtarget::getDocDir() + "index.html");

   // Setup some of the widgets.
   recipeComboBox->startObservingDB();
   equipmentComboBox->startObservingDB();
   styleComboBox->startObservingDB();
   mashComboBox->startObservingDB();
   fermDialog->startObservingDB();
   hopDialog->startObservingDB();
   miscDialog->startObservingDB();
   yeastDialog->startObservingDB();
   
   if( db->getNumRecipes() > 0 )
      setRecipe( *(db->getRecipeBegin()) );

   // Resize to the smallest possible size.
   this->resize(0,0);

   // Connect signals.
   connect( recipeComboBox, SIGNAL( activated(const QString&) ), this, SLOT(setRecipeByName(const QString&)) );
   connect( equipmentComboBox, SIGNAL( activated(const QString&) ), this, SLOT(updateRecipeEquipment(const QString&)) );
   connect( mashComboBox, SIGNAL( currentIndexChanged(const QString&) ), this, SLOT(setMashByName(const QString&)) );
   connect( styleComboBox, SIGNAL( activated(const QString&) ), this, SLOT(updateRecipeStyle(const QString&)) );
   connect( actionExit, SIGNAL( triggered() ), this, SLOT( close() ) );
   connect( actionAbout_BrewTarget, SIGNAL( triggered() ), dialog_about, SLOT( show() ) );
   connect( actionNewRecipe, SIGNAL( triggered() ), this, SLOT( newRecipe() ) );
   connect( actionImport_Recipes, SIGNAL( triggered() ), this, SLOT( importRecipes() ) );
   connect( actionExportRecipe, SIGNAL( triggered() ), this, SLOT( exportRecipe() ) );
   connect( actionEquipments, SIGNAL( triggered() ), equipEditor, SLOT( show() ) );
   connect( actionStyles, SIGNAL( triggered() ), styleEditor, SLOT( show() ) );
   connect( actionFermentables, SIGNAL( triggered() ), fermDialog, SLOT( show() ) );
   connect( actionHops, SIGNAL( triggered() ), hopDialog, SLOT( show() ) );
   connect( actionMiscs, SIGNAL( triggered() ), miscDialog, SLOT( show() ) );
   connect( actionYeasts, SIGNAL( triggered() ), yeastDialog, SLOT( show() ) );
   connect( actionOptions, SIGNAL( triggered() ), optionDialog, SLOT( show() ) );
   connect( actionManual, SIGNAL( triggered() ), htmlViewer, SLOT( show() ) );
   connect( actionScale_Recipe, SIGNAL( triggered() ), recipeScaler, SLOT( show() ) );
   connect( action_recipeToTextClipboard, SIGNAL( triggered() ), recipeFormatter, SLOT( toTextClipboard() ) );
   connect( actionConvert_Units, SIGNAL( triggered() ), converterTool, SLOT( show() ) );
   connect( actionOG_Correction_Help, SIGNAL( triggered() ), ogAdjuster, SLOT( show() ) );
   connect( actionBackup_Database, SIGNAL( triggered() ), this, SLOT( backup() ) );
   connect( actionRestore_Database, SIGNAL( triggered() ), this, SLOT( restoreFromBackup() ) );
   connect( actionCopy_Recipe, SIGNAL( triggered() ), this, SLOT( copyRecipe() ) );
   connect( actionPriming_Calculator, SIGNAL( triggered() ), primingDialog, SLOT( show() ) );
   connect( actionRefractometer_Tools, SIGNAL( triggered() ), refractoDialog, SLOT( show() ) );
   connect( actionPitch_Rate_Calculator, SIGNAL(triggered()), pitchDialog, SLOT(show()));

   connect( actionPrint_Preview, SIGNAL(triggered()), this, SLOT( printPreview()));
   connect( actionPrint_2, SIGNAL(triggered()), this, SLOT( printRecipe()));

   connect( lineEdit_name, SIGNAL( editingFinished() ), this, SLOT( updateRecipeName() ) );
   connect( lineEdit_batchSize, SIGNAL( editingFinished() ), this, SLOT( updateRecipeBatchSize() ) );
   connect( lineEdit_boilSize, SIGNAL( editingFinished() ), this, SLOT( updateRecipeBoilSize() ) );
   connect( lineEdit_efficiency, SIGNAL( editingFinished() ), this, SLOT( updateRecipeEfficiency() ) );
   connect( pushButton_addFerm, SIGNAL( clicked() ), fermDialog, SLOT( show() ) );
   connect( pushButton_addHop, SIGNAL( clicked() ), hopDialog, SLOT( show() ) );
   connect( pushButton_addMisc, SIGNAL( clicked() ), miscDialog, SLOT( show() ) );
   connect( pushButton_addYeast, SIGNAL( clicked() ), yeastDialog, SLOT( show() ) );
   connect( pushButton_removeFerm, SIGNAL( clicked() ), this, SLOT( removeSelectedFermentable() ) );
   connect( pushButton_removeHop, SIGNAL( clicked() ), this, SLOT( removeSelectedHop() ) );
   connect( pushButton_removeMisc, SIGNAL( clicked() ), this, SLOT( removeSelectedMisc() ) );
   connect( pushButton_removeYeast, SIGNAL( clicked() ), this, SLOT( removeSelectedYeast() ) );
   connect( pushButton_editFerm, SIGNAL(clicked()), this, SLOT( editSelectedFermentable() ) );
   connect( pushButton_editMisc, SIGNAL( clicked() ), this, SLOT( editSelectedMisc() ) );
   connect( pushButton_editHop, SIGNAL( clicked() ), this, SLOT( editSelectedHop() ) );
   connect( pushButton_editYeast, SIGNAL( clicked() ), this, SLOT( editSelectedYeast() ) );
   connect( pushButton_editMash, SIGNAL( clicked() ), mashEditor, SLOT( showEditor() ) );
   connect( pushButton_addMashStep, SIGNAL( clicked() ), this, SLOT(addMashStep()) );
   connect( pushButton_removeMashStep, SIGNAL( clicked() ), this, SLOT(removeSelectedMashStep()) );
   connect( pushButton_editMashStep, SIGNAL( clicked() ), this, SLOT(editSelectedMashStep()) );
   connect( pushButton_mashWizard, SIGNAL( clicked() ), mashWizard, SLOT( show() ) );
   connect( pushButton_saveMash, SIGNAL( clicked() ), this, SLOT( saveMash() ) );
   connect( pushButton_mashDes, SIGNAL( clicked() ), mashDesigner, SLOT( show() ) );
   connect( pushButton_mashUp, SIGNAL( clicked() ), mashStepTableWidget, SLOT( moveSelectedStepUp() ) );
   connect( pushButton_mashDown, SIGNAL( clicked() ), mashStepTableWidget, SLOT( moveSelectedStepDown() ) );
   connect( pushButton_mashRemove, SIGNAL( clicked() ), this, SLOT( removeMash() ) );
   connect( pushButton_donate, SIGNAL( clicked() ), this, SLOT( openDonateLink() ) );
}

void MainWindow::setupToolbar()
{
   QToolButton *newRec, *clearRec, *save, *removeRec,
               *viewEquip, *viewFerm, *viewHops,
               *viewMiscs, *viewStyles, *viewYeast,
               *timers;

   setIconSize(QSize(16, 16));

   newRec = new QToolButton(toolBar);
   clearRec = new QToolButton(toolBar);
   removeRec = new QToolButton(toolBar);
   save = new QToolButton(toolBar);
   viewEquip = new QToolButton(toolBar);
   viewFerm = new QToolButton(toolBar);
   viewHops = new QToolButton(toolBar);
   viewMiscs = new QToolButton(toolBar);
   viewStyles = new QToolButton(toolBar);
   viewYeast = new QToolButton(toolBar);
   timers = new QToolButton(toolBar);
   
   newRec->setIcon(QIcon(SMALLPLUS));
   clearRec->setIcon(QIcon(SHRED));
   removeRec->setIcon(QIcon(SMALLMINUS));
   save->setIcon(QIcon(SAVEPNG));
   viewEquip->setIcon(QIcon(SMALLKETTLE));
   viewFerm->setIcon(QIcon(SMALLBARLEY));
   viewHops->setIcon(QIcon(SMALLHOP));
   viewMiscs->setIcon(QIcon(SMALLQUESTION));
   viewStyles->setIcon(QIcon(SMALLSTYLE));
   viewYeast->setIcon(QIcon(SMALLYEAST));
   timers->setIcon(QIcon(CLOCKPNG));

   newRec->setToolTip(tr("New recipe"));
   clearRec->setToolTip(tr("Clear recipe"));
   removeRec->setToolTip(tr("Remove recipe"));
   save->setToolTip(tr("Save database"));
   viewEquip->setToolTip(tr("View equipments"));
   viewFerm->setToolTip(tr("View fermentables"));
   viewHops->setToolTip(tr("View hops"));
   viewMiscs->setToolTip(tr("View miscs"));
   viewStyles->setToolTip(tr("View styles"));
   viewYeast->setToolTip(tr("View yeasts"));
   timers->setToolTip(tr("Timers"));

   toolBar->addWidget(newRec);
   toolBar->addWidget(save);
   toolBar->addWidget(clearRec);
   toolBar->addWidget(removeRec);
   toolBar->addSeparator();
   toolBar->addWidget(viewEquip);
   toolBar->addWidget(viewFerm);
   toolBar->addWidget(viewHops);
   toolBar->addWidget(viewMiscs);
   toolBar->addWidget(viewStyles);
   toolBar->addWidget(viewYeast);
   toolBar->addSeparator();
   toolBar->addWidget(timers);

   connect( newRec, SIGNAL(clicked()), this, SLOT(newRecipe()) );
   connect( removeRec, SIGNAL(clicked()), this, SLOT(removeRecipe()) );
   connect( save, SIGNAL(clicked()), this, SLOT(save()) );
   connect( clearRec, SIGNAL(clicked()), this, SLOT(clear()) );
   connect( viewEquip, SIGNAL(clicked()), equipEditor, SLOT(show()) );
   connect( viewFerm, SIGNAL(clicked()), fermDialog, SLOT(show()) );
   connect( viewHops, SIGNAL(clicked()), hopDialog, SLOT(show()) );
   connect( viewMiscs, SIGNAL(clicked()), miscDialog, SLOT(show()) );
   connect( viewStyles, SIGNAL(clicked()), styleEditor, SLOT(show()) );
   connect( viewYeast, SIGNAL(clicked()), yeastDialog, SLOT(show()) );
   connect( timers, SIGNAL(clicked()), timerListDialog, SLOT(show()) );
   //connect( extras, SIGNAL(clicked()), recipeExtrasDialog, SLOT(show()) );
}

void MainWindow::removeRecipe()
{
   Recipe* rec = recipeComboBox->getSelectedRecipe();
   db->removeRecipe(rec);

   recipeComboBox->setIndex(0);
   setRecipe(recipeComboBox->getSelectedRecipe());
}

void MainWindow::setRecipeByName(const QString& name)
{
   if(  ! Database::isInitialized() )
      return;

   setRecipe( db->findRecipeByName( name ) );
}

// Can handle null recipes.
void MainWindow::setRecipe(Recipe* recipe)
{
   // Don't like void pointers.
   if( recipe == 0 )
      return;

   unsigned int i;
   Fermentable *ferm;
   Hop *hop;
   Misc *misc;
   Yeast *yeast;
   Style* style;
   Style* dbStyle;
   Equipment* equip;
   Equipment* dbEquip;

   // Force the Style and Equipment pointers of the recipe to point
   // inside the database so that if we make changes to the database versions
   // (like with a StyleEditor or EquipmentEditor), those changes will be
   // reflected in the recipe.
   // If there is not a version in the database, we copy the one in the recipe
   // to the database.
   style = recipe->getStyle();
   equip = recipe->getEquipment();
   if( style )
   {
      dbStyle = db->findStyleByName(style->getName());
      if( dbStyle )
         recipe->setStyle( dbStyle );
      else
         db->addStyle(style); // Recipe and db point to same style.
   }
   if( equip )
   {
      dbEquip = db->findEquipmentByName(equip->getName());
      if( dbEquip )
         recipe->setEquipment( dbEquip );
      else
         db->addEquipment(equip);
   }

   // Remove any previous recipe shit.
   fermentableTable->getModel()->removeAll();
   hopTable->getModel()->removeAll();
   miscTable->getModel()->removeAll();
   yeastTable->getModel()->removeAll();
   //mashStepTableWidget->getModel()->removeAll();

   // Make sure this MainWindow is paying attention...
   recipeObs = recipe;
   setObserved(recipeObs); // Automatically removes the previous observer.

   // Tell some of our other widgets to observe the new recipe.
   mashWizard->setRecipe(recipe);
   brewDayScrollWidget->setRecipe(recipe);
   styleComboBox->observeRecipe(recipe);
   equipmentComboBox->observeRecipe(recipe);
   maltWidget->observeRecipe(recipe);
   beerColorWidget->setRecipe(recipe);
   hopTable->getModel()->setRecipe(recipe); // This is for calculating the IBUs to show in the row headers.
   recipeFormatter->setRecipe(recipe);
   ogAdjuster->setRecipe(recipe);
   recipeExtrasWidget->setRecipe(recipe);
   mashDesigner->setRecipe(recipe);
   
   // Make sure the fermentableTable is paying attention...
   for( i = 0; i < recipeObs->getNumFermentables(); ++i )
   {
      ferm = recipeObs->getFermentable(i);
      fermentableTable->getModel()->addFermentable(ferm);
   }

   for( i = 0; i < recipeObs->getNumHops(); ++i )
   {
      hop = recipeObs->getHop(i);
      hopTable->getModel()->addHop(hop);
   }

   for( i = 0; i < recipeObs->getNumMiscs(); ++i )
   {
      misc = recipeObs->getMisc(i);
      miscTable->getModel()->addMisc(misc);
   }

   for( i = 0; i < recipeObs->getNumYeasts(); ++i )
   {
      yeast = recipeObs->getYeast(i);
      yeastTable->getModel()->addYeast(yeast);
   }

   if( recipeObs->getMash() != 0 )
   {
      //Mash* mash = recipeObs->getMash();
      //for( i = 0; i < mash->getNumMashSteps(); ++i )
      mashStepTableWidget->getModel()->setMash(recipeObs->getMash());
   }

   mashEditor->setRecipe(recipeObs);
   recipeScaler->setRecipe(recipeObs);

   showChanges();
}

void MainWindow::notify(Observable* notifier, QVariant info)
{
   // Make sure the notifier is our observed recipe
   if( notifier != recipeObs )
      return;

   showChanges(info);
}

// This method should update all the widgets in the window (except the tables)
// to reflect the currently observed recipe.
void MainWindow::showChanges(const QVariant& info)
{
   if( recipeObs == 0 )
      return;

   recipeObs->recalculate();

   lineEdit_name->setText(recipeObs->getName());
   lineEdit_name->setCursorPosition(0);
   lineEdit_batchSize->setText( Brewtarget::displayAmount(recipeObs->getBatchSize_l(), Units::liters) );
   lineEdit_boilSize->setText( Brewtarget::displayAmount(recipeObs->getBoilSize_l(), Units::liters) );
   lineEdit_efficiency->setText( Brewtarget::displayAmount(recipeObs->getEfficiency_pct(), 0) );
   
   label_calcBatchSize->setText( Brewtarget::displayAmount(recipeObs->estimateFinalVolume_l(), Units::liters) );
   label_calcBoilSize->setText( Brewtarget::displayAmount(recipeObs->estimateBoilVolume_l(), Units::liters) );
   // Color manipulation
   if( 0.95*recipeObs->getBatchSize_l() <= recipeObs->estimateFinalVolume_l() && recipeObs->estimateFinalVolume_l() <= 1.05*recipeObs->getBatchSize_l() )
      label_calcBatchSize->setPalette(lcdPalette_good);
   else if( recipeObs->estimateFinalVolume_l() < 0.95*recipeObs->getBatchSize_l() )
      label_calcBatchSize->setPalette(lcdPalette_tooLow);
   else
      label_calcBatchSize->setPalette(lcdPalette_tooHigh);
   if( 0.95*recipeObs->getBoilSize_l() <= recipeObs->estimateBoilVolume_l() && recipeObs->estimateBoilVolume_l() <= 1.05*recipeObs->getBoilSize_l() )
      label_calcBoilSize->setPalette(lcdPalette_good);
   else if( recipeObs->estimateBoilVolume_l() < 0.95* recipeObs->getBoilSize_l() )
      label_calcBoilSize->setPalette(lcdPalette_tooLow);
   else
      label_calcBoilSize->setPalette(lcdPalette_tooHigh);

   /*
   lcdNumber_og->display(Brewtarget::displayOG(recipeObs->getOg()));
   lcdNumber_boilSG->display(Brewtarget::displayOG(recipeObs->getBoilGrav()));
   lcdNumber_fg->display(Brewtarget::displayFG(recipeObs->getFg(), recipeObs->getOg()));
   lcdNumber_abv->display(QString("%1").arg(recipeObs->getABV_pct(), 0, 'f', 1));
   lcdNumber_ibu->display(QString("%1").arg(recipeObs->getIBU(), 0, 'f', 1));
   lcdNumber_srm->display(QString("%1").arg(recipeObs->getColor_srm(), 0, 'f', 1));
   lcdNumber_ibugu->display(QString("%1").arg(recipeObs->getIBU()/((recipeObs->getOg()-1)*1000), 0, 'f', 2));
   */
   lcdNumber_og->display(Brewtarget::displayOG(recipeObs->getOg()));
   lcdNumber_boilSG->display(Brewtarget::displayOG(recipeObs->getBoilGrav()));
   lcdNumber_fg->display(Brewtarget::displayFG(recipeObs->getFg(), recipeObs->getOg()));
   lcdNumber_abv->display(recipeObs->getABV_pct(), 1);
   lcdNumber_ibu->display(recipeObs->getIBU(), 1);
   lcdNumber_srm->display(Brewtarget::displayColor(recipeObs->getColor_srm(),false));
   lcdNumber_ibugu->display(recipeObs->getIBU()/((recipeObs->getOg()-1)*1000), 2);

   // Want to do some manipulation based on selected style.
   Style* recipeStyle = recipeObs->getStyle();
   if( recipeStyle != 0 )
   {
      lcdNumber_ogLow->display(Brewtarget::displayOG(recipeStyle->getOgMin()));
      lcdNumber_ogHigh->display(Brewtarget::displayOG(recipeStyle->getOgMax()));
      lcdNumber_og->setLowLim(Brewtarget::displayOG(recipeStyle->getOgMin()).toDouble());
      lcdNumber_og->setHighLim(Brewtarget::displayOG(recipeStyle->getOgMax()).toDouble());

      lcdNumber_fgLow->display(Brewtarget::displayFG(recipeStyle->getFgMin(), recipeObs->getOg()));
      lcdNumber_fgHigh->display(Brewtarget::displayFG(recipeStyle->getFgMax(), recipeObs->getOg()));
      lcdNumber_fg->setLowLim(Brewtarget::displayFG(recipeStyle->getFgMin(), recipeObs->getOg()).toDouble());
      lcdNumber_fg->setHighLim(Brewtarget::displayFG(recipeStyle->getFgMax(), recipeObs->getOg()).toDouble());

      lcdNumber_abvLow->display(recipeStyle->getAbvMin_pct(), 1);
      lcdNumber_abvHigh->display(recipeStyle->getAbvMax_pct(), 1);
      lcdNumber_abv->setLowLim(recipeStyle->getAbvMin_pct());
      lcdNumber_abv->setHighLim(recipeStyle->getAbvMax_pct());

      lcdNumber_ibuLow->display(recipeStyle->getIbuMin(), 1);
      lcdNumber_ibuHigh->display(recipeStyle->getIbuMax(), 1);
      lcdNumber_ibu->setLowLim(recipeStyle->getIbuMin());
      lcdNumber_ibu->setHighLim(recipeStyle->getIbuMax());

      lcdNumber_srmLow->display(Brewtarget::displayColor(recipeStyle->getColorMin_srm(), false));
      lcdNumber_srmHigh->display(Brewtarget::displayColor(recipeStyle->getColorMax_srm(), false));
      lcdNumber_srm->setLowLim(Brewtarget::displayColor(recipeStyle->getColorMin_srm(), false).toDouble());
      lcdNumber_srm->setHighLim(Brewtarget::displayColor(recipeStyle->getColorMax_srm(), false).toDouble());
   }

   // See if we need to change the mash in the table.
   if( info.toInt() == Recipe::MASH && recipeObs->getMash() != 0 )
      mashStepTableWidget->getModel()->setMash(recipeObs->getMash());
}

void MainWindow::save()
{
   Database::savePersistent();
}

void MainWindow::clear()
{
   if( QMessageBox::question(this, tr("Sure about that?"),
                             tr("You are about to obliterate the recipe. Is that ok?"),
                             QMessageBox::Ok,
                             QMessageBox::No)
       == QMessageBox::Ok )
   {
      recipeObs->clear();
      setRecipe(recipeObs); // This will update tables and everything.
   }
}

void MainWindow::updateRecipeName()
{
   if( recipeObs == 0 )
      return;
   
   recipeObs->setName(lineEdit_name->text());
}

void MainWindow::updateRecipeStyle()
{
   if( recipeObs == 0 )
      return;
}

void MainWindow::updateRecipeEquipment(const QString& /*equipmentName*/)
{
   if( recipeObs == 0 )
      return;

   // equip may be null.
   Equipment* equip = equipmentComboBox->getSelected();
   if( equip == 0 )
      return;

   // Notice that we are using a reference from the database, not a copy.
   // So, if the equip in the database is changed, this one will change also.
   recipeObs->setEquipment(equip);

   if( QMessageBox::question(this,
                             tr("Equipment request"),
                             tr("Would you like to set the batch and boil size to that requested by the equipment?"),
                             QMessageBox::Yes,
                             QMessageBox::No)
        == QMessageBox::Yes
     )
   {
      if( recipeObs )
      {
         recipeObs->setBatchSize_l( equip->getBatchSize_l() );
         recipeObs->setBoilSize_l( equip->getBoilSize_l() );
      }
   }
}

void MainWindow::updateRecipeStyle(const QString& /*styleName*/)
{
   if( recipeObs == 0 )
      return;

   // style may be null.
   Style* style = styleComboBox->getSelected();
   if( style == 0 )
      return;

   // Notice that we are using a reference from the database, not a copy.
   // So, if the style in the database is changed, this one will change also.
   recipeObs->setStyle(style);
}

void MainWindow::updateRecipeBatchSize()
{
   if( recipeObs == 0 )
      return;
   
   recipeObs->setBatchSize_l( Brewtarget::volQStringToSI(lineEdit_batchSize->text()) );
}

void MainWindow::updateRecipeBoilSize()
{
   if( recipeObs == 0 )
      return;
   
   recipeObs->setBoilSize_l( Brewtarget::volQStringToSI(lineEdit_boilSize->text()) );
}

void MainWindow::updateRecipeEfficiency()
{
   if( recipeObs == 0 )
      return;
   
   recipeObs->setEfficiency_pct( lineEdit_efficiency->text().toDouble() );
}

void MainWindow::addFermentableToRecipe(Fermentable* ferm)
{
   recipeObs->addFermentable(ferm);
   fermentableTable->getModel()->addFermentable(ferm);
}

void MainWindow::addHopToRecipe(Hop *hop)
{
   recipeObs->addHop(hop);
   hopTable->getModel()->addHop(hop);
}

void MainWindow::addMiscToRecipe(Misc* misc)
{
   recipeObs->addMisc(misc);
   miscTable->getModel()->addMisc(misc);
}

void MainWindow::addYeastToRecipe(Yeast* yeast)
{
   recipeObs->addYeast(yeast);
   yeastTable->getModel()->addYeast(yeast);
}

void MainWindow::exportRecipe()
{
   const char* filename;
   QFile outFile;
   QDomDocument doc;

   if( recipeObs == 0 )
      return;
   
   if( fileSaver->exec() )
     filename = fileSaver->selectedFiles()[0].toAscii();
   else
      return;

   outFile.setFileName(filename);
   
   if( ! outFile.open(QIODevice::WriteOnly | QIODevice::Truncate) )
   {
      Brewtarget::log(Brewtarget::WARNING, tr("Could not open %1 for writing.").arg(filename));
      return;
   }
   
   QTextStream out(&outFile);
   QString xmlHead = QString("version=\"1.0\" encoding=\"%1\"").arg(QTextCodec::codecForLocale()->name().data());

   // Create the headers to make other BeerXML parsers happy
   QDomProcessingInstruction inst = doc.createProcessingInstruction("xml", xmlHead);
   QDomComment beerxml = doc.createComment("BeerXML generated by brewtarget");

   doc.appendChild(inst);
   doc.appendChild(beerxml);

   QDomElement recipes = doc.createElement("RECIPES"); // The root element.
   doc.appendChild(recipes);
   recipeObs->toXml(doc, recipes);
   
   out << doc.toString();
   
   outFile.close();
}

void MainWindow::removeSelectedFermentable()
{
   QModelIndexList selected = fermentableTable->selectedIndexes();
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   Fermentable* ferm = fermentableTable->getModel()->getFermentable(row);
   fermentableTable->getModel()->removeFermentable(ferm);
   recipeObs->removeFermentable(ferm);
}

void MainWindow::editSelectedFermentable()
{
   QModelIndexList selected = fermentableTable->selectedIndexes();
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   Fermentable* ferm = fermentableTable->getModel()->getFermentable(row);
   fermEditor->setFermentable(ferm);
   fermEditor->show();
}

void MainWindow::editSelectedMisc()
{
   QModelIndexList selected = miscTable->selectedIndexes();
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   Misc* m = miscTable->getModel()->getMisc(row);
   miscEditor->setMisc(m);
   miscEditor->show();
}

void MainWindow::editSelectedHop()
{
   QModelIndexList selected = hopTable->selectedIndexes();
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   Hop* h = hopTable->getModel()->getHop(row);
   hopEditor->setHop(h);
   hopEditor->show();
}

void MainWindow::editSelectedYeast()
{
   QModelIndexList selected = yeastTable->selectedIndexes();
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   Yeast* y = yeastTable->getModel()->getYeast(row);
   yeastEditor->setYeast(y);
   yeastEditor->show();
}

void MainWindow::removeSelectedHop()
{
   QModelIndexList selected = hopTable->selectedIndexes();
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   Hop* hop = hopTable->getModel()->getHop(row);
   hopTable->getModel()->removeHop(hop);
   recipeObs->removeHop(hop);
}

void MainWindow::removeSelectedMisc()
{
   QModelIndexList selected = miscTable->selectedIndexes();
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   Misc* misc = miscTable->getModel()->getMisc(row);
   miscTable->getModel()->removeMisc(misc);
   recipeObs->removeMisc(misc);
}

void MainWindow::removeSelectedYeast()
{
   QModelIndexList selected = yeastTable->selectedIndexes();
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   Yeast* yeast = yeastTable->getModel()->getYeast(row);
   yeastTable->getModel()->removeYeast(yeast);
   recipeObs->removeYeast(yeast);
}

void MainWindow::newRecipe()
{
   QString name = QInputDialog::getText(this, tr("Recipe name"),
                                          tr("Recipe name:"));
   if( name.isEmpty() )
      return;

   Recipe* recipe = new Recipe();

   // Set the following stuff so everything appears nice
   // and the calculations don't divide by zero... things like that.
   recipe->setName(name);
   recipe->setBatchSize_l(18.93); // 5 gallons
   recipe->setBoilSize_l(23.47);  // 6.2 gallons
   recipe->setEfficiency_pct(70.0);

   db->addRecipe(recipe, false);
   setRecipe(recipe);

   recipeComboBox->setIndexByRecipeName(name);
}

void MainWindow::backup()
{
   QString dir = QFileDialog::getExistingDirectory(this, tr("Backup Database"));
   
   bool success = Database::backupToDir(dir);
   
   if( ! success )
      QMessageBox::warning( this, tr("Oops!"), tr("Could not copy the files for some reason."));
}

void MainWindow::restoreFromBackup()
{
   if( QMessageBox::question( this, tr("A Warning"),
         tr("This will obliterate your current set of recipes and ingredients. Do you want to continue?") )
       == QMessageBox::No
      )
   {
      return;
   }
   
   QString dir = QFileDialog::getExistingDirectory(this, tr("Restore Database"));
   
   bool success = Database::restoreFromDir(dir);
   
   if( ! success )
      QMessageBox::warning( this, tr("Oops!"), tr("For some reason, the operation failed.") );
}

// Imports all the recipes from a file into the database.
void MainWindow::importRecipes()
{
//   const char* filename;
   unsigned int numRecipes, i;
   //std::fstream in;
   //QVector<XmlNode*> nodes;
   Recipe* newRec;
   QFile inFile;
   QDomDocument xmlDoc;
   QDomNodeList list;
   QString err;
   int line, col;

   if ( ! fileOpener->exec() )
	   return;
   
//   if( fileOpener->exec() )
//     filename = fileOpener->selectedFiles()[0].toAscii();
//   else
//      return;
   foreach ( QString filename, fileOpener->selectedFiles())
   {
	   inFile.setFileName(filename);
	   if( ! inFile.open(QIODevice::ReadOnly) )
	   {
		  Brewtarget::log(Brewtarget::WARNING, tr("Could not open %1 for reading.").arg(filename));
		  return;
	   }

	   if( ! xmlDoc.setContent(&inFile, false, &err, &line, &col) )
		  Brewtarget::log(Brewtarget::WARNING, tr("Bad document formatting in %1 %2:%3. %4").arg(filename).arg(line).arg(col).arg(err) );

	   list = xmlDoc.elementsByTagName("RECIPE");
	   numRecipes = list.size();

	   // Tell how many recipes there were in the status bar.
	   statusBar()->showMessage( tr("Found %1 recipes.").arg(numRecipes), 5000 );

	   for( i = 0; i < numRecipes; ++i )
	   {
		  newRec = new Recipe(list.at(i));

		  if( QMessageBox::question(this, tr("Import recipe?"),
         tr("Import %1?").arg(newRec->getName()),
		   QMessageBox::Yes,
		   QMessageBox::No)
		 == QMessageBox::Yes )
		  {
		 db->addRecipe( newRec, true ); // Copy all subelements of the recipe into the db also.
		  }
	   }

	   inFile.close();
   }
}

void MainWindow::addMashStep()
{
   Mash* mash;
   if( recipeObs != 0 && recipeObs->getMash() != 0 )
   {
      mash = recipeObs->getMash();
   }
   else
   {
      QMessageBox::information(this, tr("No mash"), tr("Trying to add a mash step without a mash. Please create a mash first.") );
      return;
   }

   MashStep* step = new MashStep();
   mash->addMashStep(step);
   mashStepEditor->setMashStep(step);
   mashStepEditor->setVisible(true);
}

void MainWindow::removeSelectedMashStep()
{
   Mash* mash;
   if( recipeObs && recipeObs->getMash() )
   {
      mash = recipeObs->getMash();
   }
   else
   {
      return;
   }
   
   QModelIndexList selected = mashStepTableWidget->selectedIndexes();
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   MashStep* step = mash->getMashStep(row); //mashStepTableWidget->getModel()->getMashStep(row);
   //mashStepTableWidget->getModel()->removeMashStep(step);
   mash->removeMashStep(step);
}

void MainWindow::editSelectedMashStep()
{
   Mash* mash;
   if( recipeObs && recipeObs->getMash() )
   {
      mash = recipeObs->getMash();
   }
   else
   {
      return;
   }

   QModelIndexList selected = mashStepTableWidget->selectedIndexes();
   int row, size, i;

   size = selected.size();
   if( size == 0 )
      return;

   // Make sure only one row is selected.
   row = selected[0].row();
   for( i = 1; i < size; ++i )
   {
      if( selected[i].row() != row )
         return;
   }

   MashStep* step = mash->getMashStep(row);//mashStepTableWidget->getModel()->getMashStep(row);
   mashStepEditor->setMashStep(step);
   mashStepEditor->setVisible(true);
}

void MainWindow::removeMash()
{

	if( mashComboBox->currentIndex() == -1)
		return;
	//due to way this is designed, we can't have a NULL mash, so
	//we need to remove all the mash steps and then remove the mash
	//from the database.
	//remove from db
	Mash *m = mashComboBox->getSelectedMash();
	m->removeAllMashSteps();
	db->removeMash(m);
	
	Mash* defaultMash = new Mash();
	recipeObs->setMash( defaultMash );
	MashStepTableModel *model = mashStepTableWidget->getModel();
	model->setMash(defaultMash);
	
	//remove from combobox handled automatically by qt
	//if( db->getNumMashs() < 1 )
		mashComboBox->setIndex( -1 );
	/*else
		mashComboBox->setIndex( 0 );
	 */
	recipeObs->forceNotify();
}

/*
void MainWindow::brewDayMode()
{
   if( QMessageBox::question(this, tr("New instructions?"), tr("Generate new instructions?"), QMessageBox::Yes, QMessageBox::No )
       == QMessageBox::Yes )
   {
      if( recipeObs != 0 )
         recipeObs->generateInstructions();
   }

   brewDayDialog->show();
}
*/

void MainWindow::closeEvent(QCloseEvent* /*event*/)
{
   if( QMessageBox::question(this, tr("Save database?"),
                             tr("Do you want to save the changes made? If not, you will lose anything you changed in this session."),
                             QMessageBox::Yes,
                             QMessageBox::No)
       == QMessageBox::Yes )
   {
      Database::savePersistent();
   }

   Brewtarget::savePersistentOptions();

   setVisible(false);
}

void MainWindow::forceRecipeUpdate()
{
   if( recipeObs == 0 )
      return;

   recipeObs->hasChanged();
}

void MainWindow::copyRecipe()
{
   QString name = QInputDialog::getText( this, tr("Copy Recipe"), tr("Enter a unique name for the copy.") );
   
   if( name.isEmpty() )
      return;
   
   Recipe* newRec = new Recipe(recipeObs); // Create a deep copy.
   newRec->setName(name);
   
   (Database::getDatabase())->addRecipe( newRec, false );
}

void MainWindow::setMashByName(const QString& name)
{
   if( recipeObs == 0 )
      return;
   
   Mash* mash = (Database::getDatabase())->findMashByName(name);
   
   // Do nothing if the mash retrieved is null or if it has the same name as the one in the recipe.
   if( mash == 0 || (recipeObs->getMash() != 0 && recipeObs->getMash()->getName() == mash->getName()) )
		return;

   Mash* newMash = new Mash();
   
   newMash->deepCopy(mash); // Make a copy so we don't modify the database version.
   
   recipeObs->setMash(newMash);
}

void MainWindow::saveMash()
{
   if( recipeObs == 0 || recipeObs->getMash() == 0 )
      return;
   
   Mash* mash = recipeObs->getMash();
   Mash* newMash = new Mash(); // Make a copy to go in the database.
   
   // Ensure the mash has a name.
   if( mash->getName() == "" )
   {
      QMessageBox::information( this, tr("Oops!"), tr("Please give your mash a name before saving.") );
      return;
   }
   
   newMash->deepCopy(mash);
   
   (Database::getDatabase())->addMash(newMash);
}

void MainWindow::openDonateLink()
{
   QDesktopServices::openUrl(QUrl("http://sourceforge.net/project/project_donations.php?group_id=249733"));
}

void MainWindow::printPreview()
{
   recipeFormatter->printPreview();

   /*
   QTabWidget* currentTab = reinterpret_cast<QTabWidget*>(tabWidget_recipeView->currentWidget());
   if( currentTab == tab_recipe || currentTab == tab_extras )
      recipeFormatter->printPreview();
   else if( currentTab == tab_brewday )
      brewDayScrollWidget->printPreview();
   */
}

void MainWindow::printRecipe()
{
   QPrintDialog printerDialog(printer, this);

   recipeFormatter->print( printer, &printerDialog );

   /*
   QTabWidget* currentTab = reinterpret_cast<QTabWidget*>(tabWidget_recipeView->currentWidget());
   if( currentTab == tab_recipe || currentTab == tab_extras )
      recipeFormatter->print( printer, &printerDialog );
   else if( currentTab == tab_brewday )
      brewDayScrollWidget->print( printer, &printerDialog );
   */
}

