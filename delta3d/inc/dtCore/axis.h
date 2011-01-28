/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2011 MOVES Institute
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

#ifndef axis_h__
#define axis_h__

// axis.h: Declaration of the Axis class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/export.h>
#include <dtCore/inputdevicefeature.h>
#include <list>

namespace dtCore
{
   class AxisListener;
   class AxisObserver;

   class DT_CORE_EXPORT Axis : public InputDeviceFeature
   {
   public:
      typedef std::list<AxisListener*> AxisListenerList;
      typedef std::list<AxisObserver*> AxisObserverList;

      /**
       * Constructor.
       *
       * @param owner the owner of this axis
       * @param description a description of this axis
       */
      Axis(InputDevice* owner, const std::string& description);

   protected:

      virtual ~Axis() {}

   public:

      /**
       * Sets the state of this axis.  The delta value is used to
       * indicate stateless motion, such as mouse scrolling.
       *
       * @param state the new state
       * @param delta the optional delta value
       *
       * @return Whether the state was changed or not
       */
      bool SetState(double state, double delta = 0.0);

      /**
       * Returns the current state of this axis.
       *
       * @return the current state of this axis
       */
      double GetState() const;

      /**
       * Notifies all the axis listeners of a state change
       *
       * @param delta   the optional delta value
       *
       * @return whether the state change was handled or not
       */
      bool NotifyStateChange(double delta = 0.0);

      /**
       * Adds an axis listener.
       *
       * @param axisListener a pointer to the listener to add
       */
      void AddAxisListener(AxisListener* axisListener);

      /// Inserts the listener into the list at a position BEFORE pos.
      void InsertAxisListener(const AxisListenerList::value_type& pos, AxisListener* al);

      /**
       * Removes an axis listener.
       *
       * @param axisListener a pointer to the listener to remove
       */
      void RemoveAxisListener(AxisListener* axisListener);

      const AxisListenerList& GetListeners() const { return mAxisListeners; }

      /**
       * Adds an axis observer.
       *
       * @param axisObserver a pointer to the observer to add
       */
      void AddAxisObserver(AxisObserver* axisObserver);

      /// Inserts the observer into the list at a position BEFORE pos.
      void InsertAxisObserver(const AxisObserverList::value_type& pos, AxisObserver* al);

      /**
       * Removes an axis observer.
       *
       * @param axisObserver a pointer to the observer to remove
       */
      void RemoveAxisObserver(AxisObserver* axisObserver);

      const AxisObserverList& GetObservers() const { return mAxisObservers; }

   private:
      double mState;  ///< The state of this axis.
      double mPrevState; ///< The previous state of the axis.
      AxisListenerList mAxisListeners;  ///< Listeners to this axis.
      AxisObserverList mAxisObservers;  ///< Observers to this axis.
   };
}

#endif // axis_h__
