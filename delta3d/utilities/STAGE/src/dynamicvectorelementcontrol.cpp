/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2005, BMH Associates, Inc. 
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
 * Curtiss Murphy
 */

#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/dynamicvectorelementcontrol.h>
#include <dtEditQt/dynamicsubwidgets.h>
#include <dtEditQt/editorevents.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/log.h>

#include <QtCore/QLocale>
#include <QtGui/QWidget>
#include <QtGui/QDoubleValidator>
#include <sstream>

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec2ActorProperty *newVectorProp, 
            int whichIndex, const std::string &newLabel)
            : label(newLabel), mElementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop  = newVectorProp;
        vec3Prop  = NULL;
        vec4Prop  = NULL;
        vec2fProp = NULL;
        vec3fProp = NULL;
        vec4fProp = NULL;
        vec2dProp = NULL;
        vec3dProp = NULL;
        vec4dProp = NULL;
        whichType = VEC2;
    }

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec2fActorProperty *newVectorProp, 
        int whichIndex, const std::string &newLabel)
        : label(newLabel), mElementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop  = NULL;
        vec3Prop  = NULL;
        vec4Prop  = NULL;
        vec2fProp = newVectorProp;
        vec3fProp = NULL;
        vec4fProp = NULL;
        vec2dProp = NULL;
        vec3dProp = NULL;
        vec4dProp = NULL;
        whichType = VEC2F;
    }

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec2dActorProperty *newVectorProp, 
        int whichIndex, const std::string &newLabel)
        : label(newLabel), mElementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop  = NULL;
        vec3Prop  = NULL;
        vec4Prop  = NULL;
        vec2fProp = NULL;
        vec3fProp = NULL;
        vec4fProp = NULL;
        vec2dProp = newVectorProp;
        vec3dProp = NULL;
        vec4dProp = NULL;
        whichType = VEC2D;
    }

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec3ActorProperty *newVectorProp, 
            int whichIndex, const std::string &newLabel)
            : label(newLabel), mElementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop  = NULL;
        vec3Prop  = newVectorProp;
        vec4Prop  = NULL;
        vec2fProp = NULL;
        vec3fProp = NULL;
        vec4fProp = NULL;
        vec2dProp = NULL;
        vec3dProp = NULL;
        vec4dProp = NULL;
        whichType = VEC3;
    }

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec3fActorProperty *newVectorProp, 
        int whichIndex, const std::string &newLabel)
        : label(newLabel), mElementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop  = NULL;
        vec3Prop  = NULL;
        vec4Prop  = NULL;
        vec2fProp = NULL;
        vec3fProp = newVectorProp;
        vec4fProp = NULL;
        vec2dProp = NULL;
        vec3dProp = NULL;
        vec4dProp = NULL;
        whichType = VEC3F;
    }

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec3dActorProperty *newVectorProp, 
        int whichIndex, const std::string &newLabel)
        : label(newLabel), mElementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop  = NULL;
        vec3Prop  = NULL;
        vec4Prop  = NULL;
        vec2fProp = NULL;
        vec3fProp = NULL;
        vec4fProp = NULL;
        vec2dProp = NULL;
        vec3dProp = newVectorProp;
        vec4dProp = NULL;
        whichType = VEC3D;
    }

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec4ActorProperty *newVectorProp, 
            int whichIndex, const std::string &newLabel)
            : label(newLabel), mElementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop  = NULL;
        vec3Prop  = NULL;
        vec4Prop  = newVectorProp;
        vec2fProp = NULL;
        vec3fProp = NULL;
        vec4fProp = NULL;
        vec2dProp = NULL;
        vec3dProp = NULL;
        vec4dProp = NULL;
        whichType = VEC4;
    }

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec4fActorProperty *newVectorProp, 
        int whichIndex, const std::string &newLabel)
        : label(newLabel), mElementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop  = NULL;
        vec3Prop  = NULL;
        vec4Prop  = NULL;
        vec2fProp = NULL;
        vec3fProp = NULL;
        vec4fProp = newVectorProp;
        vec2dProp = NULL;
        vec3dProp = NULL;
        vec4dProp = NULL;
        whichType = VEC4F;
    }

    ///////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec4dActorProperty *newVectorProp, 
        int whichIndex, const std::string &newLabel)
        : label(newLabel), mElementIndex(whichIndex), temporaryEditControl(NULL)
    {

        vec2Prop  = NULL;
        vec3Prop  = NULL;
        vec4Prop  = NULL;
        vec2fProp = NULL;
        vec3fProp = NULL;
        vec4fProp = NULL;
        vec2dProp = NULL;
        vec3dProp = NULL;
        vec4dProp = newVectorProp;
        whichType = VEC4D;
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicVectorElementControl::~DynamicVectorElementControl()
    {
    }


    /////////////////////////////////////////////////////////////////////////////////
    void DynamicVectorElementControl::initializeData(DynamicAbstractControl *newParent,
        PropertyEditorModel *newModel, dtDAL::ActorProxy *newProxy, dtDAL::ActorProperty *newProperty)
    {
        DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

        // determine whether we have floats or doubles 
        // Doesn't matter whether this is a vec2, 3, or 4. 
        osg::Vec3 testVect;
        isVecFloat = (sizeof(testVect.x()) == sizeof(float));
        if (isVecFloat) {
            toolTipTypeLabel = "FLOAT";
        } else {
            toolTipTypeLabel = "DOUBLE";
        }

    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicVectorElementControl::updateEditorFromModel(QWidget *widget)
    {
        if (widget != NULL && widget == temporaryEditControl)
        {
           std::ostringstream ss;
           ss << "Updating editor for index " << mElementIndex;
           LOGN_DEBUG("dynamicvectorelementcontrol.cpp", ss.str());
            // set the current value from our property
            double value = getValue();
            //QString strValue = QString::number(value, 'f', NUM_DECIMAL_DIGITS);
            QLocale locale(QLocale::C);
            QString strValue = locale.toString(value, 'f', NUM_DECIMAL_DIGITS);
            temporaryEditControl->setText(strValue);
            temporaryEditControl->selectAll();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicVectorElementControl::updateModelFromEditor(QWidget *widget)
    {
        bool dataChanged = false;

        if (widget != NULL && widget == temporaryEditControl) 
        {
           std::ostringstream ss;
           ss << "Updating model for index " << mElementIndex;
           LOGN_DEBUG("dynamicvectorelementcontrol.cpp", ss.str());

            // Note, don't use the temporary variable here.  It can cause errors with QT.
            SubQLineEdit *editBox = static_cast<SubQLineEdit *>(widget);
            bool success = false;
            //float floatResult = -999.999f; 
            double doubleResult = editBox->text().toDouble(&success);

            // set our value to our object
            if (success)
            {
                // Save the data if they are different.  Note, we also need to compare the QString value, 
                // else we get epsilon differences that cause the map to be marked dirty with no edits :(
                QString proxyValue = QString::number(getValue(), 'f', NUM_DECIMAL_DIGITS);
                QString newValue = editBox->text();
                if (doubleResult != getValue() && proxyValue != newValue)
                {
                    setValue(doubleResult);
                    dataChanged = true;
                }
            } 
            else 
            {
                LOGN_ERROR("dynamicvectorelementcontrol.cpp", "updateData() failed to convert our value successfully");
            }

            // reselect all the text when we commit.  
            // Gives the user visual feedback that something happened.
            editBox->selectAll();
        }

        // notify the world (mostly the viewports) that our property changed
        if (dataChanged) 
        {
           std::ostringstream ss;
           ss << "Updating model (data changed) for index " << mElementIndex;
           LOGN_DEBUG("dynamicvectorelementcontrol.cpp", ss.str());

            if (whichType == VEC2) 
            {
                EditorEvents::GetInstance().emitActorPropertyChanged(proxy, vec2Prop);
            } 
            else if (whichType == VEC2F) 
            {
                EditorEvents::GetInstance().emitActorPropertyChanged(proxy, vec2fProp);
            } 
            else if (whichType == VEC2D) 
            {
                EditorEvents::GetInstance().emitActorPropertyChanged(proxy, vec2dProp);
            } 
            else if (whichType == VEC3) 
            {
                EditorEvents::GetInstance().emitActorPropertyChanged(proxy, vec3Prop);
            } 
            else if (whichType == VEC3F) 
            {
                EditorEvents::GetInstance().emitActorPropertyChanged(proxy, vec3fProp);
            } 
            else if (whichType == VEC3D) 
            {
                EditorEvents::GetInstance().emitActorPropertyChanged(proxy, vec3dProp);
            } 
            else if (whichType == VEC4) 
            {
                EditorEvents::GetInstance().emitActorPropertyChanged(proxy, vec4Prop);
            } 
            else if (whichType == VEC4F) 
            {
                EditorEvents::GetInstance().emitActorPropertyChanged(proxy, vec4fProp);
            } 
            else if (whichType == VEC4D)
            { 
                EditorEvents::GetInstance().emitActorPropertyChanged(proxy, vec4dProp);
            }
        }

        return dataChanged;
    }


    /////////////////////////////////////////////////////////////////////////////////
    QWidget *DynamicVectorElementControl::createEditor(QWidget *parent, 
        const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        std::ostringstream ss;
        ss << "Creating editor for index " << mElementIndex;
        LOGN_DEBUG("dynamicvectorelementcontrol.cpp", ss.str());
        // create and init the edit box
        //editBox = new QLineEdit(parent);
        temporaryEditControl = new SubQLineEdit (parent, this);
        QDoubleValidator *validator = new QDoubleValidator(temporaryEditControl);
        validator->setDecimals(NUM_DECIMAL_DIGITS);
        temporaryEditControl->setValidator(validator);

        if (!initialized)  {
            LOGN_ERROR("dynamicvectorelementcontrol.cpp", "Tried to add itself to the parent widget before being initialized");
            return temporaryEditControl;
        }

        updateEditorFromModel(temporaryEditControl);

        temporaryEditControl->setToolTip(getDescription());

        return temporaryEditControl;
    }


    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVectorElementControl::getDisplayName()
    {
        return QString(tr(label.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVectorElementControl::getDescription() 
    {
        std::string tooltip;

        if (whichType == VEC2) 
        {
            tooltip = vec2Prop->GetDescription();
        } 
        else if (whichType == VEC2F) 
        {
            tooltip = vec2fProp->GetDescription();
        } 
        else if (whichType == VEC2D) 
        {
            tooltip = vec2dProp->GetDescription();
        } 
        else if (whichType == VEC3) 
        {
            tooltip = vec3Prop->GetDescription();
        } 
        else if (whichType == VEC3F) 
        {
            tooltip = vec3fProp->GetDescription();
        } 
        else if (whichType == VEC3D) 
        {
            tooltip = vec3dProp->GetDescription();
        } 
        else if (whichType == VEC4) 
        {
            tooltip = vec4Prop->GetDescription();
        } 
        else if (whichType == VEC4F) 
        {
            tooltip = vec4fProp->GetDescription();
        } 
        else if (whichType == VEC4D)
        {
            tooltip = vec4dProp->GetDescription();
        }

        tooltip += " [Type: " + label;
        return QString(tr(tooltip.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////
    const QString DynamicVectorElementControl::getValueAsString() 
    {
        double value = getValue();
        return QString::number(value, 'f', NUM_DECIMAL_DIGITS);
    }

    /////////////////////////////////////////////////////////////////////////////////
    bool DynamicVectorElementControl::isEditable()
    {
        if(whichType == VEC2)
            return !vec2Prop->IsReadOnly();
        else if(whichType == VEC2F)
            return !vec2fProp->IsReadOnly();
        else if(whichType == VEC2D)
            return !vec2dProp->IsReadOnly();
        else if(whichType == VEC3)
            return !vec3Prop->IsReadOnly();
        else if(whichType == VEC3F)
            return !vec3fProp->IsReadOnly();
        else if(whichType == VEC3D)
            return !vec3dProp->IsReadOnly();
        else if(whichType == VEC4)
            return !vec4Prop->IsReadOnly();
        else if(whichType == VEC4F)
            return !vec4fProp->IsReadOnly();
        else if(whichType == VEC2D)
            return !vec4dProp->IsReadOnly();

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////
    double DynamicVectorElementControl::getValue()
    {
        double result = 0.0;

        if (whichType == VEC2) 
        {
            osg::Vec2 vectorValue = vec2Prop->GetValue();
            result = vectorValue[mElementIndex];
        } 
        else if (whichType == VEC2F) 
        {
            osg::Vec2f vectorValue = vec2fProp->GetValue();
            result = vectorValue[mElementIndex];
        } 
        else if (whichType == VEC2D) 
        {
            osg::Vec2d vectorValue = vec2dProp->GetValue();
            result = vectorValue[mElementIndex];
        } 
        else if (whichType == VEC3) 
        {
            osg::Vec3 vectorValue = vec3Prop->GetValue();
            result = vectorValue[mElementIndex];
        } 
        else if (whichType == VEC3F) 
        {
            osg::Vec3f vectorValue = vec3fProp->GetValue();
            result = vectorValue[mElementIndex];
        } 
        else if (whichType == VEC3D) 
        {
            osg::Vec3d vectorValue = vec3dProp->GetValue();
            result = vectorValue[mElementIndex];
        } 
        else if (whichType == VEC4) 
        {
            osg::Vec4 vectorValue = vec4Prop->GetValue();
            result = vectorValue[mElementIndex];
        } 
        else if (whichType == VEC4F) 
        {
            osg::Vec4f vectorValue = vec4fProp->GetValue();
            result = vectorValue[mElementIndex];
        } 
        else if (whichType == VEC4D)
        { 
            osg::Vec4d vectorValue = vec4dProp->GetValue();
            result = vectorValue[mElementIndex];
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicVectorElementControl::setValue(double value)
    {
        if (whichType == VEC2) 
        {
            std::string oldValue = vec2Prop->ToString();
            osg::Vec2 vectorValue = vec2Prop->GetValue();
            vectorValue[mElementIndex] = value;
            vec2Prop->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, vec2Prop, 
                oldValue, vec2Prop->ToString());
        }
        else if (whichType == VEC2F) 
        {
            std::string oldValue = vec2fProp->ToString();
            osg::Vec2f vectorValue = vec2fProp->GetValue();
            vectorValue[mElementIndex] = value;
            vec2fProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, vec2fProp, 
                oldValue, vec2fProp->ToString());
        }
        else if (whichType == VEC2D) 
        {
            std::string oldValue = vec2dProp->ToString();
            osg::Vec2d vectorValue = vec2dProp->GetValue();
            vectorValue[mElementIndex] = value;
            vec2dProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, vec2dProp, 
                oldValue, vec2dProp->ToString());
        }
        else if (whichType == VEC3) 
        {
            std::string oldValue = vec3Prop->ToString();
            osg::Vec3 vectorValue = vec3Prop->GetValue();
            vectorValue[mElementIndex] = value;
            vec3Prop->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, vec3Prop, 
                oldValue, vec3Prop->ToString());
        } 
        else if (whichType == VEC3F) 
        {
            std::string oldValue = vec3fProp->ToString();
            osg::Vec3f vectorValue = vec3fProp->GetValue();
            vectorValue[mElementIndex] = value;
            vec3fProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, vec3fProp, 
                oldValue, vec3fProp->ToString());
        } 
        else if (whichType == VEC3D) 
        {
            std::string oldValue = vec3dProp->ToString();
            osg::Vec3d vectorValue = vec3dProp->GetValue();
            vectorValue[mElementIndex] = value;
            vec3dProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, vec3dProp, 
                oldValue, vec3dProp->ToString());
        } 
        else if (whichType == VEC4) 
        {
            std::string oldValue = vec4Prop->ToString();
            osg::Vec4 vectorValue = vec4Prop->GetValue();
            vectorValue[mElementIndex] = value;
            vec4Prop->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, vec4Prop, 
                oldValue, vec4Prop->ToString());
        } 
        else if (whichType == VEC4F) 
        {
            std::string oldValue = vec4fProp->ToString();
            osg::Vec4f vectorValue = vec4fProp->GetValue();
            vectorValue[mElementIndex] = value;
            vec4fProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, vec4fProp, 
                oldValue, vec4fProp->ToString());
        } 
        else if (whichType == VEC4D)
        {
            std::string oldValue = vec4dProp->ToString();
            osg::Vec4d vectorValue = vec4dProp->GetValue();
            vectorValue[mElementIndex] = value;
            vec4dProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, vec4dProp, 
                oldValue, vec4dProp->ToString());
        }
    }


    /////////////////////////////////////////////////////////////////////////////////
    // SLOTS
    /////////////////////////////////////////////////////////////////////////////////

    bool DynamicVectorElementControl::updateData(QWidget *widget)
    {
        if (!initialized || widget == NULL)  
        {
            LOG_ERROR("Tried to updateData before being initialized");
            return false;
        }

        return updateModelFromEditor(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicVectorElementControl::actorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
        dtCore::RefPtr<dtDAL::ActorProperty> property)
    {
        if (temporaryEditControl != NULL && proxy == this->proxy) 
        {
            if (whichType == VEC2 && property == vec2Prop) 
            {
                updateEditorFromModel(temporaryEditControl);
            } 
            else if (whichType == VEC2F && property == vec2fProp) 
            {
                updateEditorFromModel(temporaryEditControl);
            } 
            else if (whichType == VEC2D && property == vec2dProp) 
            {
                updateEditorFromModel(temporaryEditControl);
            } 
            else if (whichType == VEC3 && property == vec3Prop) 
            {
                updateEditorFromModel(temporaryEditControl);
            } 
            else if (whichType == VEC3F && property == vec3fProp) 
            {
                updateEditorFromModel(temporaryEditControl);
            } 
            else if (whichType == VEC3D && property == vec3dProp) 
            {
                updateEditorFromModel(temporaryEditControl);
            } 
            else if (whichType == VEC4 && property == vec4Prop)
            {
                updateEditorFromModel(temporaryEditControl);
            }
            else if (whichType == VEC4F && property == vec4fProp)
            {
                updateEditorFromModel(temporaryEditControl);
            }
            else if (whichType == VEC4D && property == vec4dProp)
            {
                updateEditorFromModel(temporaryEditControl);
            }
        }
    }
}
