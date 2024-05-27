/*╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌
 * sortFilterProxyModels/RecipeAdditionFermentableSortFilterProxyModel.h is part of Brewtarget, and is copyright the following
 * authors 2009-2023:
 *   • Matt Young <mfsy@yahoo.com>
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
#ifndef SORTFILTERPROXYMODELS_RECIPEADDITIONFERMENTABLESORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODELS_RECIPEADDITIONFERMENTABLESORTFILTERPROXYMODEL_H
#pragma once

#include <QSortFilterProxyModel>

#include "sortFilterProxyModels/SortFilterProxyModelBase.h"
#include "tableModels/RecipeAdditionFermentableTableModel.h"
#include "listModels/RecipeAdditionFermentableListModel.h"

/*!
 * \class RecipeAdditionFermentableSortFilterProxyModel
 *
 * \brief Proxy model for sorting/filtering recipe hop additions
 */
class RecipeAdditionFermentableSortFilterProxyModel :
   public QSortFilterProxyModel,
   public SortFilterProxyModelBase<RecipeAdditionFermentableSortFilterProxyModel,
                                   RecipeAdditionFermentableTableModel,
                                   RecipeAdditionFermentableListModel> {
   Q_OBJECT

   SORT_FILTER_PROXY_MODEL_COMMON_DECL(RecipeAdditionFermentable)
};

#endif
