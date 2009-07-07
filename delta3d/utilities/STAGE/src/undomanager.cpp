/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Curtiss Murphy
 */
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QAction>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/undomanager.h>
#include <dtEditQt/viewportmanager.h>
#include <dtCore/uniqueid.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actortype.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>
#include <dtDAL/project.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/undomanager.h>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   UndoManager::UndoManager()
      : mRecursePrevent(false)
   {
      LOG_INFO("Initializing the UndoManager.");

      // connect all my signals  that cause the undo list to be trashed.
      connect(&EditorEvents::GetInstance(), SIGNAL(editorInitiationEvent()),
         this, SLOT(clearAllHistories()));
      connect(&EditorEvents::GetInstance(), SIGNAL(editorCloseEvent()),
         this, SLOT(clearAllHistories()));
      connect(&EditorEvents::GetInstance(), SIGNAL(projectChanged()),
         this, SLOT(clearAllHistories()));
      connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
         this, SLOT(clearAllHistories()));
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryImported()),
         this, SLOT(clearAllHistories()));
      connect(&EditorEvents::GetInstance(), SIGNAL(mapLibraryRemoved()),
         this, SLOT(clearAllHistories()));

      // trap destry, create, change, and about to change
      connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyDestroyed(ActorProxyRefPtr)),
         this, SLOT(onActorProxyDestroyed(ActorProxyRefPtr)));
      connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)),
         this, SLOT(onActorProxyCreated(ActorProxyRefPtr, bool)));
      connect(&EditorEvents::GetInstance(),
         SIGNAL(actorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr)),
         this, SLOT(onActorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr)));
      connect(&EditorEvents::GetInstance(),
         SIGNAL(actorPropertyAboutToChange(ActorProxyRefPtr, ActorPropertyRefPtr,
         std::string, std::string)),
         this, SLOT(actorPropertyAboutToChange(ActorProxyRefPtr, ActorPropertyRefPtr,
         std::string, std::string)));
      connect(&EditorEvents::GetInstance(), SIGNAL(proxyNameChanged(ActorProxyRefPtr, std::string)),
         this, SLOT(onProxyNameChanged(ActorProxyRefPtr, std::string)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   UndoManager::~UndoManager()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::onActorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
      dtCore::RefPtr<dtDAL::ActorProperty> property)
   {
      if (!mRecursePrevent)
      {
         // clear the redo list anytime we add a new item to the undo list.
         clearRedoList();

         // verify that the about to change event is the same as this event
         if (mAboutToChangeEvent != NULL && mAboutToChangeEvent->mObjectId ==
             proxy->GetId().ToString() && mAboutToChangeEvent->mType ==
             ChangeEvent::PROPERTY_CHANGED && mAboutToChangeEvent->mUndoPropData.size() > 0)
         {
            // double check the actual property.
            dtCore::RefPtr<UndoPropertyData> propData = mAboutToChangeEvent->mUndoPropData[0];
            if (propData->mPropertyName == property->GetName())
            {
               // FINALLY, we get to add it to our undo list.
               mUndoStack.push(mAboutToChangeEvent);
            }
         }

         mAboutToChangeEvent = NULL;

         enableButtons();
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::actorPropertyAboutToChange(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
      dtCore::RefPtr<dtDAL::ActorProperty> property, std::string oldValue, std::string newValue)
   {
      if (!mRecursePrevent)
      {
         // clear the redo list anytime we add a new item to the undo list.
         clearRedoList();

         ChangeEvent* undoEvent = new ChangeEvent();
         undoEvent->mType       = ChangeEvent::PROPERTY_CHANGED;
         if (proxy.valid())
         {
            undoEvent->mObjectId          = proxy->GetId().ToString();
            undoEvent->mActorTypeName     = proxy->GetActorType().GetName();
            undoEvent->mActorTypeCategory = proxy->GetActorType().GetCategory();
         }

         UndoPropertyData* propData = new UndoPropertyData();
         propData->mPropertyName    = property->GetName();
         propData->mOldValue        = oldValue;
         propData->mNewValue        = newValue;
         undoEvent->mUndoPropData.push_back(propData);

         // mark this event as the current event without adding it to our undo stack
         mAboutToChangeEvent = undoEvent;

         enableButtons();
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::onActorProxyCreated(dtCore::RefPtr<dtDAL::ActorProxy> proxy, bool forceNoAdjustments)
   {
      if (!mRecursePrevent)
      {
         // clear the redo list anytime we add a new item to the undo list.
         clearRedoList();
         // clear any incomplete property change events
         mAboutToChangeEvent = NULL;

         ChangeEvent* undoEvent = createFullUndoEvent(proxy.get());
         undoEvent->mType = ChangeEvent::PROXY_CREATED;

         // add it to our main undo stack
         mUndoStack.push(undoEvent);

         enableButtons();
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::onProxyNameChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy, std::string oldName)
   {
      if (!mRecursePrevent)
      {
         // clear the redo list anytime we add a new item to the undo list.
         clearRedoList();
         // clear any incomplete property change events
         mAboutToChangeEvent = NULL;

         ChangeEvent* undoEvent       = new ChangeEvent();
         undoEvent->mType              = ChangeEvent::PROXY_NAME_CHANGED;
         undoEvent->mObjectId          = proxy->GetId().ToString();
         undoEvent->mActorTypeName     = proxy->GetActorType().GetName();
         undoEvent->mActorTypeCategory = proxy->GetActorType().GetCategory();
         undoEvent->mOldName           = oldName;

         // add it to our main undo stack
         mUndoStack.push(undoEvent);

         enableButtons();
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::onActorProxyDestroyed(dtCore::RefPtr<dtDAL::ActorProxy> proxy)
   {
      if (!mRecursePrevent)
      {
         std::vector<dtDAL::ActorProperty*> propList;
         std::vector<dtDAL::ActorProperty*>::const_iterator propIter;

         // clear the redo list anytime we add a new item to the undo list.
         clearRedoList();
         // clear any incomplete property change events
         mAboutToChangeEvent = NULL;

         ChangeEvent* undoEvent = createFullUndoEvent(proxy.get());
         undoEvent->mType = ChangeEvent::PROXY_DELETED;

         // add it to our main undo stack
         mUndoStack.push(undoEvent);

         enableButtons();
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::doRedo()
   {
      // clear any incomplete property change events
      mAboutToChangeEvent = NULL;

      if (!mRedoStack.empty())
      {
         dtCore::RefPtr<ChangeEvent> redoEvent = mRedoStack.top();
         mRedoStack.pop();

         // If the redo event was the beginning of a group, continue processing
         // undo events until we reach the beginning of the group.
         if (redoEvent->mType == ChangeEvent::UNDO_GROUP_BEGIN)
         {
            // Push this begin marker into the undo stack.
            mUndoStack.push(redoEvent);

            // Iterate through the group.
            while (!mRedoStack.empty()) 
            {
               redoEvent = mRedoStack.top();
               mRedoStack.pop();

               // We are finished when we reach the begining of the group.
               if (redoEvent->mType == ChangeEvent::UNDO_GROUP_END)
               {
                  mUndoStack.push(redoEvent);
                  enableButtons();
                  return;
               }

               handleUndoRedoEvent(redoEvent.get(), false);
            }

            return;
         }

         handleUndoRedoEvent(redoEvent.get(), false);
      }

      enableButtons();
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::doUndo()
   {
      // clear any incomplete property change events
      mAboutToChangeEvent = NULL;

      if (!mUndoStack.empty())
      {
         dtCore::RefPtr<ChangeEvent> undoEvent = mUndoStack.top();
         mUndoStack.pop();

         // If the undo event was the end of a group, continue processing
         // undo events until we reach the beginning of the group.
         if (undoEvent->mType == ChangeEvent::UNDO_GROUP_END)
         {
            // Push this end marker into the redo stack.
            mRedoStack.push(undoEvent);

            // Iterate through the group.
            while (!mUndoStack.empty()) 
            {
               undoEvent = mUndoStack.top();
               mUndoStack.pop();

               // We are finished when we reach the beginning of the group.
               if (undoEvent->mType == ChangeEvent::UNDO_GROUP_BEGIN)
               {
                  mRedoStack.push(undoEvent);
                  enableButtons();
                  return;
               }

               handleUndoRedoEvent(undoEvent.get(), true);
            }

            return;
         }

         handleUndoRedoEvent(undoEvent.get(), true);
      }

      enableButtons();
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::handleUndoRedoEvent(ChangeEvent* event, bool isUndo)
   {
      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();

      if (currMap.valid())
      {
         dtDAL::ActorProxy* proxy = currMap->GetProxyById(dtCore::UniqueId(event->mObjectId));

         // delete is special since the proxy is always NULL :)
         if (event->mType == ChangeEvent::PROXY_DELETED)
         {
            handleUndoRedoDeleteObject(event, isUndo);
            return; // best to return because event can be modified as a side effect
         }

         if (proxy != NULL)
         {
            switch (event->mType)
            {
            case ChangeEvent::PROXY_NAME_CHANGED:
               handleUndoRedoNameChange(event, proxy, isUndo);
               return; // best to return because event can be modified as a side effect
            case ChangeEvent::PROPERTY_CHANGED:
               handleUndoRedoPropertyValue(event, proxy, isUndo);
               return; // best to return because event can be modified as a side effect
            case ChangeEvent::PROXY_CREATED:
               handleUndoRedoCreateObject(event, proxy, isUndo);
               return; // best to return because event can be modified as a side effect
    
            default:
            break;
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::handleUndoRedoNameChange(ChangeEvent* event, dtDAL::ActorProxy* proxy, bool isUndo)
   {
      // NOTE - The undo/redo methods do both the undo and the redo.  If you are modifying
      // these methods, be VERY careful

      std::string currentName = proxy->GetName();

      // do the undo
      proxy->SetName(event->mOldName);
      // notify the world of our change to the data.
      dtCore::RefPtr<dtDAL::ActorProxy> ActorProxyRefPtr = proxy;
      mRecursePrevent = true;
      EditorEvents::GetInstance().emitProxyNameChanged(ActorProxyRefPtr, currentName);
      mRecursePrevent = false;

      // now turn the undo into a redo event
      event->mOldName = currentName;

      if (isUndo)
      {
         // add it REDO stack
         mRedoStack.push(event);
      }
      else
      {
         // add it UNDO stack
         mUndoStack.push(event);
      }

      enableButtons();
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::handleUndoRedoCreateObject(ChangeEvent* event, dtDAL::ActorProxy* proxy, bool isUndo)
   {
      // NOTE - The undo/redo methods do both the undo and the redo.  If you are modifying
      // these methods, be VERY careful

      // We are UNDO'ing a create, so we delete it. That means that we add a
      // Delete change event to the Undo or redo list.

      event->mType = ChangeEvent::PROXY_DELETED;

      if (isUndo)
      {
         // add it REDO stack
         mRedoStack.push(event);
      }
      else
      {
         // add it UNDO stack
         mUndoStack.push(event);
      }

      // Delete the sucker
      mRecursePrevent = true;
      EditorData::GetInstance().getMainWindow()->startWaitCursor();
      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();
      EditorActions::GetInstance().deleteProxy(proxy, currMap);

      //We are deleting an object, so clear the current selection for safety.
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > emptySelection;
      EditorEvents::GetInstance().emitActorsSelected(emptySelection);

      EditorData::GetInstance().getMainWindow()->endWaitCursor();
      mRecursePrevent = false;
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::handleUndoRedoDeleteObject(ChangeEvent* event, bool isUndo)
   {
      // NOTE - The undo/redo methods do both the undo and the redo.  If you are modifying
      // these methods, be VERY careful

      // Note, it seems a bit backwards, just like the create.  We are UNDO'ing a
      // delete event.  Which means, we need to recreate the object and set all its
      // properties. Then, we need to add a CREATE change event to the REDO list.

      std::vector<dtCore::RefPtr < UndoPropertyData > >::const_iterator undoPropIter;
      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();

      // figure out the actor type
      dtCore::RefPtr<const dtDAL::ActorType> actorType = dtDAL::LibraryManager::GetInstance().
         FindActorType(event->mActorTypeCategory, event->mActorTypeName);

      if (currMap.valid() && actorType.valid())
      {
         EditorData::GetInstance().getMainWindow()->startWaitCursor();

         // recreate the actor!
         dtCore::RefPtr<dtDAL::ActorProxy> proxy =
            dtDAL::LibraryManager::GetInstance().CreateActorProxy(*actorType.get()).get();
         if (proxy.valid())
         {
            // set all of our old properties before telling anyone else about it
            for (undoPropIter = event->mUndoPropData.begin(); undoPropIter != event->mUndoPropData.end(); ++undoPropIter)
            {
               dtCore::RefPtr<UndoPropertyData> undoProp = (*undoPropIter);
               // find the prop on the real actor
               dtDAL::ActorProperty* actorProp = proxy->GetProperty(undoProp->mPropertyName);

               // put our value back
               if (actorProp != NULL)
               {
                  actorProp->FromString(undoProp->mOldValue);
               }
            }

            proxy->SetId(dtCore::UniqueId(event->mObjectId));
            proxy->SetName(event->mOldName);
            currMap->AddProxy(*(proxy.get()));
            mRecursePrevent = true;
            EditorEvents::GetInstance().emitBeginChangeTransaction();
            EditorEvents::GetInstance().emitActorProxyCreated(proxy, true);
            //ViewportManager::GetInstance().placeProxyInFrontOfCamera(proxy.get());
            EditorEvents::GetInstance().emitEndChangeTransaction();
            mRecursePrevent = false;

            // create our redo event
            event->mType = ChangeEvent::PROXY_CREATED;
            if (isUndo)
            {
               mRedoStack.push(event);
            }
            else
            {
               mUndoStack.push(event);
            }
         }
      }
      EditorData::GetInstance().getMainWindow()->endWaitCursor();
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::handleUndoRedoPropertyValue(ChangeEvent* event, dtDAL::ActorProxy* proxy, bool isUndo)
   {
      // NOTE - The undo/redo methods do both the undo and the redo.  If you are modifying
      // these methods, be VERY careful

      if (event->mUndoPropData.size() == 0)
      {
         return;
      }

      // for property changed, simply find the old property and reset the value
      dtCore::RefPtr<UndoPropertyData> propData = event->mUndoPropData[0];
      if (propData.valid())
      {
         dtDAL::ActorProperty* property = proxy->GetProperty(propData->mPropertyName);
         if (property != NULL)
         {
            std::string currentValue = property->ToString();
            property->FromString(propData->mOldValue);

            // notify the world of our change to the data.
            dtCore::RefPtr<dtDAL::ActorProxy> ActorProxyRefPtr = proxy;
            dtCore::RefPtr<dtDAL::ActorProperty> ActorPropertyRefPtr = property;
            mRecursePrevent = true;
            EditorData::GetInstance().getMainWindow()->startWaitCursor();
            EditorEvents::GetInstance().emitActorPropertyChanged(ActorProxyRefPtr, ActorPropertyRefPtr);
            EditorData::GetInstance().getMainWindow()->endWaitCursor();
            mRecursePrevent = false;

            // Create the Redo event - reverse old and new.
            event->mType = UndoManager::ChangeEvent::PROPERTY_CHANGED;
            propData->mNewValue = propData->mOldValue;
            propData->mOldValue = currentValue;
            if (isUndo)
            {
               // add it REDO stack
               mRedoStack.push(event);
            }
            else
            {
               // add it UNDO stack
               mUndoStack.push(event);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   UndoManager::ChangeEvent* UndoManager::createFullUndoEvent(dtDAL::ActorProxy* proxy)
   {
      dtDAL::ActorProperty* curProp;
      std::vector<dtDAL::ActorProperty*> propList;
      std::vector<dtDAL::ActorProperty*>::const_iterator propIter;

      ChangeEvent* undoEvent       = new ChangeEvent();
      undoEvent->mObjectId          = proxy->GetId().ToString();
      undoEvent->mActorTypeName     = proxy->GetActorType().GetName();
      undoEvent->mActorTypeCategory = proxy->GetActorType().GetCategory();
      undoEvent->mOldName           = proxy->GetName();

      // for each property, create a property data object and add it to our event's list.
      proxy->GetPropertyList(propList);
      for (propIter = propList.begin(); propIter != propList.end(); ++propIter)
      {
         curProp = (*propIter);
         UndoPropertyData* undoData = new UndoPropertyData();
         undoData->mPropertyName = curProp->GetName();
         undoData->mOldValue = curProp->ToString();
         undoData->mNewValue = ""; // ain't a new value - put here for completeness and readability

         undoEvent->mUndoPropData.push_back(undoData);
      }

      return undoEvent;
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::enableButtons()
   {
      EditorActions::GetInstance().mActionEditUndo->setEnabled(!mUndoStack.empty());
      EditorActions::GetInstance().mActionEditRedo->setEnabled(!mRedoStack.empty());
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::clearAllHistories()
   {
      clearUndoList();
      clearRedoList();

      enableButtons();
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::clearUndoList()
   {
      // clear undo
      while (!mUndoStack.empty())
      {
         dtCore::RefPtr<ChangeEvent> undoEvent = mUndoStack.top();
         mUndoStack.pop();
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void UndoManager::clearRedoList()
   {
      // clear redo
      while (!mRedoStack.empty())
      {
         dtCore::RefPtr<ChangeEvent> redoEvent = mRedoStack.top();
         mRedoStack.pop();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void UndoManager::beginUndoGroup()
   {
      ChangeEvent* undoEvent = new ChangeEvent();
      undoEvent->mType       = ChangeEvent::UNDO_GROUP_BEGIN;

      mUndoStack.push(undoEvent);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void UndoManager::endUndoGroup()
   {
      ChangeEvent* undoEvent = new ChangeEvent();
      undoEvent->mType       = ChangeEvent::UNDO_GROUP_END;

      mUndoStack.push(undoEvent);
   }

   //////////////////////////////////////////////////////////////////////////////
   bool UndoManager::hasUndoItems()
   {
      return !mUndoStack.empty();
   }

   //////////////////////////////////////////////////////////////////////////////
   bool UndoManager::hasRedoItems()
   {
      return !mRedoStack.empty();
   }

} // namespace dtEditQt
