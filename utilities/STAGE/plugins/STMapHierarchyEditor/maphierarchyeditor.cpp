/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
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
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "maphierarchyeditor.h"
#include <dtCore/environment.h>
#include <dtCore/map.h>
#include <dtCore/transformable.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/viewportmanager.h>
#include <dtQt/nodegraphview.h>



////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
const std::string SceneHierarchyEditorPlugin::PLUGIN_NAME = "Scene Hierarchy Editor";



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
SceneHierarchyEditorPlugin::SceneHierarchyEditorPlugin(dtEditQt::MainWindow* mw)
   : mUI()
   , mMainWindow(mw)
{
   //apply layout made with QtDesigner
   mUI.setupUi(this);

   mNodeGraphViewer = new dtQt::NodeGraphViewerPanel;
   mUI.dockWidgetContents->layout()->addWidget(mNodeGraphViewer);

   //add dock widget to STAGE main window
   mw->addDockWidget(Qt::BottomDockWidgetArea, this);
   
   connect(&dtEditQt::EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
      this, SLOT(onCurrentMapChanged()));

   QGraphicsScene* scene = mNodeGraphViewer->GetNodeGraphView().GetNodeGraphScene();
   connect(scene, SIGNAL(selectionChanged()),
      this, SLOT(OnSelectionChanged()));

   UpdateUI();
}

////////////////////////////////////////////////////////////////////////////////
SceneHierarchyEditorPlugin::~SceneHierarchyEditorPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
// remove our stuff from GUI
void SceneHierarchyEditorPlugin::Destroy()
{
   mMainWindow->removeDockWidget(this);
}


////////////////////////////////////////////////////////////////////////////////
// user has closed the dock widget. Stop the plugin
void SceneHierarchyEditorPlugin::closeEvent(QCloseEvent* event)
{
   mMainWindow->GetPluginManager()->StopPlugin(PLUGIN_NAME);
}

////////////////////////////////////////////////////////////////////////////////
// HELPER PRED
struct NodeNameComparePred
{
   bool operator() (const dtQt::BaseNodeWrapper* nodeA, dtQt::BaseNodeWrapper* nodeB)
   {
      return dtUtil::StrCompare(nodeA->GetName(), nodeB->GetName(), false) < 0;
   }
};

////////////////////////////////////////////////////////////////////////////////
void SceneHierarchyEditorPlugin::UpdateUI()
{
   dtCore::Map* m = dtEditQt::EditorData::GetInstance().getCurrentMap();
   if (m != NULL)
   {
      dtCore::BaseActorObject* rootActor = m->GetEnvironmentActor();

      typedef std::map<dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> > IdActorMap;
      const IdActorMap& actors = m->GetAllProxies();
      
      if (rootActor == nullptr)
      {
         LOG_WARNING("Could not access root actor: NULL");
      }
      
      if ( ! actors.empty())
      {
         // Reserve enough node elements for all the actors.
         dtQt::BaseNodeWrapperArray nodes;
         nodes.reserve(actors.size());

         // Wrap each of the root actors in a node wrapper.
         std::for_each(actors.begin(), actors.end(),
            [&](IdActorMap::value_type iter)
            {
               dtCore::BaseActorObject* actor = iter.second.get();

               // TODO: Add options and filtering mechanism to limit the node display for large scenes.

               // Accept only scene level actors (those without parents).
               dtGame::GameActorProxy* gameActor = dynamic_cast<dtGame::GameActorProxy*>(actor);
               if (gameActor == nullptr || gameActor->GetParentActor() == nullptr)
               {
                  nodes.push_back(new dtQt::ActorNodeWrapper(*actor));
               }
            }
         );

         // Sort the nodes by name.
         NodeNameComparePred pred;
         std::sort(nodes.begin(), nodes.end(), pred);
         
         // Send  the node wrappers to be displayed in the node hierarchy graph.
         mNodeGraphViewer->GetNodeGraphView().GetNodeGraphScene()->SetSceneNodes(nodes);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void SceneHierarchyEditorPlugin::onCurrentMapChanged()
{
   UpdateUI();
}

////////////////////////////////////////////////////////////////////////////////
void SceneHierarchyEditorPlugin::OnSelectionChanged()
{
   dtQt::NodeGraphScene* scene = mNodeGraphViewer->GetNodeGraphView().GetNodeGraphScene();

   // Get the selected graphics objects as node wrappers...
   dtQt::BaseNodeWrapperArray nodes;
   if (scene->GetSelectedNodes(nodes) > 0)
   {
      ActorRefPtrVector actors;
      actors.reserve(nodes.size());

      // Then for each wrapper, access it as an actor...
      std::for_each(nodes.begin(), nodes.end(),
         [&](dtQt::BaseNodeWrapper* nodeWrapper)
         {
            dtQt::ActorNodeWrapper* actorNode = dynamic_cast<dtQt::ActorNodeWrapper*>(nodeWrapper);

            // If there is an actor, add it to the list for selection in the editor.
            if (actorNode != nullptr && actorNode->Get() != nullptr)
            {
               actors.push_back(actorNode->Get());
            }
         }
      );

      // Signal to the editor that actors were selected.
      dtEditQt::EditorEvents::GetInstance().selectedActors(actors);
   }
}



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
namespace SceneHierarchyEditor
{
   class DELTA_SCENE_HIERARCHY_EDITOR_EXPORT PluginFactory : public dtEditQt::PluginFactory
   {
   public:
      PluginFactory() {}
      ~PluginFactory() {}

      /** get the name of the plugin */
      virtual std::string GetName() { return SceneHierarchyEditorPlugin::PLUGIN_NAME; }

      /** get a description of the plugin */
      virtual std::string GetDescription() { return "Scene Hierarchy Editor"; }

      virtual void GetDependencies(std::list<std::string>& deps)
      {
         // just for testing
         deps.push_back("Plugin Manager");
      }

       /** construct the plugin and return a pointer to it */
      virtual dtEditQt::Plugin* Create(dtEditQt::MainWindow* mw)
      {
         mPlugin = new SceneHierarchyEditorPlugin(mw);
         return mPlugin;
      }

      virtual void Destroy()
      {
         delete mPlugin;
      }

   private:

      dtEditQt::Plugin* mPlugin;
   };

} //namespace SceneHierarchyEditor
           
extern "C" DELTA_SCENE_HIERARCHY_EDITOR_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new SceneHierarchyEditor::PluginFactory;
}