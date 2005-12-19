/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author Matthew W. Campbell, William E. Johnson II, and David Guthrie
*/
#ifndef DELTA_GAMEMANANGER
#define DELTA_GAMEMANANGER

#include <set>
#include <map>
#include <queue>
#include "dtGame/export.h"
#include "dtGame/gmcomponent.h"
#include "dtGame/gameactor.h"
#include "dtGame/messagefactory.h"
#include "dtGame/message.h"
#include "dtGame/machineinfo.h"
#include <dtCore/refptr.h>
#include <dtCore/base.h>
#include <dtCore/scene.h>
#include <dtCore/timer.h>
#include <dtDAL/librarymanager.h>
#include <dtUtil/log.h>

// Forward declarations
namespace dtDAL
{
   class ActorType;
   class ActorPluginRegistry;
}

namespace dtGame 
{
   //class Message;

   class DT_GAME_EXPORT GameManager : public dtCore::Base 
   {
      DECLARE_MANAGEMENT_LAYER(GameManager)

      public:

         /// Constructor
         GameManager(dtCore::Scene &scene);

         /// Destructor
         virtual ~GameManager();

         /**
          * Loads an actor registry
          * @param libName the name of the library to load
          */
         void LoadActorRegistry(const std::string &libName);

         /**
          * Unloads an actor registry
          * @param libName the name of the library to unload
          */
         void UnloadActorRegistry(const std::string &libName);

         /**
          * Returns a list of all the actor types the library manager knows how 
          * to create.
          * @param actorTypes A vector to fill
          */
         void GetActorTypes(std::vector<dtCore::RefPtr<dtDAL::ActorType> > &actorTypes);

         /**
          * Gets a single actor type that matches the name and category specified.
          * @param category Category of the actor type to find.
          * @param name Name of the actor type.
          * @return A valid smart pointer if the actor type was found.
          */
         dtCore::RefPtr<dtDAL::ActorType> FindActorType(const std::string &category, const std::string &name);

         /**
          * Gets a registry currently loaded by the library manager.  
          * @param name The name of the library.  Note, this name is the
          * system independent name.
          * @return A handle to the registry or NULL if it is not currently loaded.
          */
         dtDAL::ActorPluginRegistry* GetRegistry(const std::string &name);

         /**
          * @param actorType the actor type to get the registry for.
          */
         dtDAL::ActorPluginRegistry* GetRegistryForType(dtDAL::ActorType& actorType);

         /**
          * Determines which platform we are running on and returns a
          * platform dependent library name.
          * @param libBase Platform independent library name.
          * @return A platform dependent library name.
          * @note
          *  For example.  If the platform independent library name is
          *  ExampleActors then on Windows platforms the resulting dependent
          *  library name would be ExampleActors.dll, however, on Unix based
          *  platforms, the resulting name would be libExampleActors.so.
          */
         std::string GetPlatformSpecificLibraryName(const std::string &libBase);

         /**
          * Strips off the path and platform specific library prefixs and extensions
          * and returns a system independent file name.
          * @param libName The platform specific library name.
          * @return A platform independent library name.
          */
         std::string GetPlatformIndependentLibraryName(const std::string &libName);


         /**
          * Called by the dtCore::Base class
          * @param The data from the message
          * @see dtCore::Base
          */
         virtual void OnMessage(MessageData *data);

         /**
          * Calls SendMessage on all of the components
          * @param The message to send
          */
         void SendMessage(const Message& message);

         /**
          * Calls ProcessMessage on all of the components
          * @param The message to process
          */
         void ProcessMessage(const Message& message);

         /**
          * Adds a component to the list of components the game mananger
          * will communicate with
          * @param The component to add
          */
         void AddComponent(GMComponent& component);

         /**
          * Removes a component to the list of components the game mananger
          * will communicate with
          * @param The component to remove
          */
         void RemoveComponent(GMComponent& component);

         /**
          * Fills a vector with all of the components in the game manager.
          * @param toFill the vector to fill.
          */
         void GetAllComponents(std::vector<GMComponent*>& toFill);

         /**
          * Fills a vector with all of the components in the game manager.
          * @param toFill the vector to fill.
          */
         void GetAllComponents(std::vector<const GMComponent*>& toFill) const;

         /**
          * Creates ana actor based on the actor type.
          * @param The actor type to create.
          * @throws dtDAL::ExceptionEnum::ObjectFactoryUnknownType
          */
         dtCore::RefPtr<dtDAL::ActorProxy> CreateActor(dtDAL::ActorType& actorType) throw(dtUtil::Exception);

         /**
          * Adds an actor to the list of actors that the game manager knows about
          * @param actorProxy  The actor proxy to add
          */
         void AddActor(dtDAL::ActorProxy& actorProxy);

         /**
          * Adds a game actor to the list of actors that the game manager knows about
          * @param actorProxy The actor proxy to add
          * @param isRemote true if the actor is remote, false if not
          * @param publish true if the actor should be immediately published.
          * @throws ExceptionEnum::ACTOR_IS_REMOTE if the actor is remote and publish is true.
          * @throws ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if the actor is flagged as a game actor, but is not a GameActorProxy.
          */
         void AddActor(GameActorProxy& gameActorProxy, bool isRemote, bool publish) throw(dtUtil::Exception);

         /**
          * Publishes an actor to the world.  Remote actors may not be published
          * @param The actor to publish
          * @throws ExceptionEnum::ACTOR_IS_REMOTE if the actor is remote.
          */
         void PublishActor(GameActorProxy& gameActorProxy);

         /**
          * Removes a game actor from the game manager.
          * The actor is not actually removed until the end of the current frame so that
          * messages can propogate.  
          * An INFO_ACTOR_DELETE message is only sent if the actor is local.
          * @param The actor to remove
          */
         void DeleteActor(GameActorProxy& gameActorProxy);

         /**
          * Removes all game actors and actors from the game manager
          * Currently all actors are removed immediately, but this should not be 
          * assumed to be true going forward.
          * INFO_ACTOR_DELETE messages are only sent for local actors.
          */
         void DeleteAllActors() { DeleteAllActors(true); }

         /**
          * Removes an actor from the game manager
          * @param The actor to remove
          */
         void DeleteActor(dtDAL::ActorProxy& actorProxy);

         /**
          * Returns a list of actor types that have been registered with the
          * game manager
          * @param The vector to fill
          */
         void GetActorTypes(std::vector<const dtDAL::ActorType*> &vec) const;

         /**
          * Gets the number of game actors currently managed by this game
          * manager.
          * @return The number of game actors in the system.
          */
         unsigned int GetNumGameActors() const
         {
            return mGameActorProxyMap.size();
         }
         
         /**
          * Retrieves all the game actors added to the GM
          * @param toFill The vector to fill
          */
         void GetAllGameActors(std::vector<dtCore::RefPtr<GameActorProxy> > &toFill);

         /**
          * Retrieves all the non game actors added to the GM
          * @param toFill The vector to fill
          */
         void GetAllNonGameActors(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);

         /**
          * Retrieves all the actors added to the GM
          * @param toFill The vector to fill
          */
         void GetAllActors(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);

         /**
          * Fills a vector with the game proxys whose names match the name parameter
          * @param The name to search for
          * @param The vector to fill
          */
         void FindActorsByName(const std::string &name, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);

         /**
          * Fills a vector with the game proxys whose types match the type parameter
          * @param The type to search for
          * @param The vector to fill
          */
         void FindActorsByType(const dtDAL::ActorType &type, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);

         /**
          * Fills a vector with the game proxys whose position is within the radius parameter
          * @param The radius to search in 
          * @param The vector to fill
          */
         void FindActorsWithinRadius(const float radius, std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &toFill);

         /**
          * Returns the game actor proxy whose is matches the parameter
          * @param The id of the proxy to find
          * @return The proxy, or NULL if not found
          */
         GameActorProxy* FindGameActorById(const dtCore::UniqueId &id);

         /**
          * Returns the actor proxy whose is matches the parameter. This will search both the game actors and the
          * regular actor proxies.
          * @param The id of the proxy to find
          * @return The proxy, or NULL if not found
          */
         dtDAL::ActorProxy* FindActorById(const dtCore::UniqueId &id);

         /**
          * Saves the game state
          * @return True if saved successfully, false if error
          */
         bool SaveGameState();

         /**
          * Loads a game state
          * @return True if loaded successfully, false if error
          */
         bool LoadGameState();

         /**
          * Changes the map being used by the Game Manager
          * @param mapName The name of the map
          * @param enableDatabasePaging optional parameter to enable database paging for paged LODs usually used in
          *                             large terrain databases.  Passing false will not disable paging if it is already enabled.
          * @throws ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION if an actor is flagged as a game actor, but is not a GameActorProxy.
          */
         void ChangeMap(const std::string &mapName, bool enableDatabasePaging = true) throw(dtUtil::Exception);

         /**
          * Sets the timer of the game mananger.  It will send out a timer message when it expires.
          * @param name The name of the timer
          * @param aboutActor the actor to put in the about field of the message.
          * @param time The time of the timer in milliseconds
          * @param repeat True to repeat the timer, false if once only
          * @param realTime True if this time should use real time, or false if it should use simulation time.
          */
         void SetTimer(
            const std::string& name, 
            GameActorProxy& aboutActor, 
            float time, 
            bool repeat = false, 
            bool realTime = false);

         /**
          * Removes the timer with the given name.  If no timer by that name exists, this is a no-op.
          * @param name the name of the timer to remove.
          */
         void ClearTimer(const std::string& name);

         /**
          * Accessor to the scene member of the class
          * @return The scene
          */
         dtCore::Scene& GetScene() { return *mScene; }

         /**
          * Accessor to the scene member of the class
          * @return The scene
          */
         const dtCore::Scene& GetScene() const { return *mScene; }

         /**
          * Sets the scene member of the class
          * @param The new scene to assign
          */
         void SetScene(dtCore::Scene &newScene) { mScene = &newScene; }

         /**
          * Retrieves the message factor that is controlled by the GameManager
          * @return mFactory he message factory
          * @see class dtGame::MessageFactory
          */
         MessageFactory& GetMessageFactory()  { return mFactory; } 

         /**
          * Retrieves the message factor that is controlled by the GameManager
          * @return mFactory he message factory
          * @see class dtGame::MessageFactory
          */
         const MessageFactory& GetMessageFactory() const { return mFactory; } 
         
         const MachineInfo& GetMachineInfo() const { return *mMachineInfo; }
         MachineInfo& GetMachineInfo() { return *mMachineInfo; }
         
         const std::string& GetCurrentMap() const { return mLoadedMap; }
         
         /// @return the scale of realtime the GameManager is running at.
         float GetTimeScale() const;
                  
         /**
          * @return the current simulation time
          * @see dtCore::System#GetSimulationTime
          */
         double GetSimulationTime() const;

         /**
          * @return the current simulation wall-clock time
          * @see dtCore::System#GetSimulationClockTime
          */
         dtCore::Timer_t GetSimulationClockTime() const;
         
         /**
          * Change the time settings.
          * @param newTime The new simulation time.
          * @param newTimeScale the new simulation time progression as a factor of real time.
          * @param newClockTime  The new simulation wall-clock time.
          */
         void ChangeTimeSettings(double newTime, float newTimeScale, dtCore::Timer_t newClockTime);
         
         /**
          * Get all of the GameActorProxies registered to receive all messages of a certain type.
          * @param type the message type to query for.
          * @param toFill a vector to fill with pairs GameActorProxies and the name of the invokable.
          */
         void GetGlobalMessageListeners(const MessageType& type, std::vector<std::pair<GameActorProxy*, std::string > >& toFill);
         
         /**
          * Get all of the GameActorProxies registered to receive messages of a given type for a given GameActor.
          * @param type the message type to query for.
          * @param targetActorId the id of the GameActor to query for.
          * @param toFill a vector to fill with the GameActorProxies.
          */
         void GetGameActorMessageListeners(
            const MessageType& type, 
            const dtCore::UniqueId& targetActorId, 
            std::vector<std::pair<GameActorProxy*, std::string > >& toFill);

         /**
          * @param type
          * @param proxy
          * @param invokableName
          */ 
         void RegisterGlobalMessageListener(const MessageType& type, GameActorProxy& proxy, const std::string& invokableName);
         
         /**
          * @param type
          * @param proxy
          * @param invokableName
          */ 
         void UnregisterGlobalMessageListener(const MessageType& type, GameActorProxy& proxy, const std::string& invokableName);
      
         /**
          * @param type
          * @param targetActorId
          * @param proxy
          * @param invokableName
          */ 
         void RegisterGameActorMessageListener(
            const MessageType& type, 
            const dtCore::UniqueId& targetActorId, 
            GameActorProxy& proxy, 
            const std::string& invokableName);
         
         /**
          * @param type
          * @param targetActorId
          * @param proxy
          * @param invokableName
          */ 
         void UnregisterGameActorMessageListener(
            const MessageType& type, 
            const dtCore::UniqueId& targetActorId, 
            GameActorProxy& proxy, 
            const std::string& invokableName);
            
         /**
          * @param proxy
          */
         void UnregisterAllMessageListenersForActor(GameActorProxy& proxy);
         
         /**
          * @return true if the GameManager is paused
          */
         bool IsPaused() const { return mPaused; }
         
         /**
          * Pauses or unpauses this GameManager.
          * @param pause true of false if this GM should be paused. If this value is 
          *              the same as the current state, this call is a noop.
          */
         void SetPaused(bool pause);

         /**
          * Handles a reject message.  This is typically called by a component (usually server side)
          * when it has determined that a request message is invalid and it needs to reject it.
          * The method creates a ServerMessageRejected - SERVER_REQUEST_REJECTED message.  If the 
          * reasonMessage has a MachineInfo that indicates it came from this server, then this method
          * does a ProcessMessage on the new rejected message.  Otherwise, it does a SendMessage.
          * The resulting reject message will eventually make its way back to the source client-component.
          * @param toReject the Message that you are trying to reject.
          * @param rejectDescription A text message describing why the message was rejected.
          */
         void RejectMessage(const dtGame::Message &reasonMessage, const std::string &rejectDescription);

      protected:

         struct TimerInfo 
         {
            std::string name;
            dtCore::UniqueId aboutActor;
            double time;
            bool repeat;
            bool realTime;
         };

         dtUtil::Log* mLogger;

         /**
          * Implements the functionality that will happen on the PreFrame event
          * @param deltaSimTime the change in simulation time since the last frame.
          * @param deltaRealTime the change in real time since the last frame.
          */
         virtual void PreFrame(double deltaSimTime, double deltaRealTime);

         /// Implements the functionality that will happen on the PostFrame event
         virtual void PostFrame();

         /**
          * Removes all game actors and actors from the game manager
          * Currently all actors are removed immediately, but this should not be 
          * assumed to be true going forward.
          * @param sendMessage true if the message about deleting should be sent.
          */
         void DeleteAllActors(bool sendMessage);
      
      private:

         dtCore::RefPtr<MachineInfo> mMachineInfo; 

         std::map<dtCore::UniqueId, dtCore::RefPtr<GameActorProxy> > mGameActorProxyMap;
         std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> > mActorProxyMap;
         std::vector<dtCore::RefPtr<GameActorProxy> > mDeleteList;

         std::multimap<std::string, TimerInfo> mTimers;

         std::multimap<const MessageType*, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> > mGlobalMessageListeners;
         std::map<const MessageType*, std::multimap<dtCore::UniqueId, std::pair<dtCore::RefPtr<GameActorProxy>, std::string> > > mActorMessageListeners;

         std::vector<dtCore::RefPtr<GMComponent> > mComponentList; 
         std::queue<dtCore::RefPtr<const Message> > mSendMessageQueue;
         std::queue<dtCore::RefPtr<const Message> > mProcessMessageQueue;
         dtCore::RefPtr<dtCore::Scene> mScene;
         dtCore::RefPtr<dtDAL::LibraryManager> mLibMgr;
         MessageFactory mFactory;
         
         bool mPaused;
         std::string mLoadedMap;
   };
}

#endif
