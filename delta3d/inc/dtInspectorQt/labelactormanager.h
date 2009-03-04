/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
 
#ifndef labelactormanager_h__
#define labelactormanager_h__

#include <dtInspectorQt/imanager.h>
#include <QtCore/QObject>
#include <dtCore/observerptr.h>
#include <dtABC/labelactor.h>

namespace Ui
{
   class InspectorWidget;
}

namespace dtInspectorQt
{
   ///Handles the properties of dtABC::LabelActor
   class LabelActorManager : public IManager
   {
      Q_OBJECT

   public:
      LabelActorManager(Ui::InspectorWidget &ui);
      virtual ~LabelActorManager();

      virtual void OperateOn(dtCore::Base* b);

   protected slots:
      void OnTextChanged();
      void OnAlignmentChanged(int newAlignment);
      void OnFontSizeChanged(double newValue);
      void OnLightingToggled(int checked);
      void OnBackdropSizeChanged(double newValue);
      void OnTextColorChanged(double newValue);
      void OnTextColorPickerClicked();
      void OnBackdropColorChanged(double newValue);
      void OnBackdropColorPickerClicked();

   private:
      void Update();

      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtABC::LabelActor> mOperateOn;

   };
}  
#endif //labelactormanager_h__
