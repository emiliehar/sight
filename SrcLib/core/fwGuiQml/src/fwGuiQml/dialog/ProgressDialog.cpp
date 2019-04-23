/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwGuiQml/dialog/ProgressDialog.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwContainer.hpp>
#include <fwGui/IFrameSrv.hpp>
#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <QGuiApplication>

fwGuiRegisterMacro( ::fwGuiQml::dialog::ProgressDialog, ::fwGui::dialog::IProgressDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

ProgressDialog::ProgressDialog( ::fwGui::GuiBaseObject::Key key, const std::string& title, const std::string& message) :
    m_title("")
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/dialog/ProgressDialog.qml");

    // load the qml ui component
    engine->getRootContext()->setContextProperty("progressDialog", this);
    m_dialog = engine->createComponent(dialogPath);
    this->setTitle(title);
    this->setMessage(message);
    QMetaObject::invokeMethod(m_dialog, "open");
    qGuiApp->processEvents();
}

//------------------------------------------------------------------------------

ProgressDialog::~ProgressDialog()
{
    delete m_dialog;
}

//------------------------------------------------------------------------------

void ProgressDialog::operator()(float percent, std::string msg)
{
    SLM_ASSERT("m_dialog not instanced", m_dialog);
    if (!m_visible)
    {
        this->cancelPressed();
        return;
    }
    int value = int(percent*100);
    if(value != this->m_value)
    {
        OSLM_TRACE( "ProgressDialog msg" << msg << " : " << value <<"%");
        this->setMessage(msg);

        this->m_value = value;
        Q_EMIT valueChanged();
        if ( m_processUserEvents )
        {
            qGuiApp->processEvents();
        }
        else
        {
            qGuiApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::setTitle(const std::string& title)
{
    m_title = QString::fromStdString(title);
    Q_EMIT titleChanged();
}

//------------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string& msg)
{

    m_message = "";
    if (!m_title.isEmpty())
    {
        m_message += m_title;
        m_message += " - ";
    }
    m_message = QString::fromStdString(msg);
    Q_EMIT messageChanged();
}

//------------------------------------------------------------------------------

void ProgressDialog::cancelPressed()
{
    IProgressDialog::cancelPressed();
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
