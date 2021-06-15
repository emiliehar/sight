/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/ui/qt/calibration/SImagesSelector.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/tools/fwID.hpp>

#include <data/helper/Vector.hpp>
#include <data/Image.hpp>
#include <data/Vector.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::qt::calibration
{

const core::com::Slots::SlotKeyType SImagesSelector::s_ADD_SLOT    = "add";
const core::com::Slots::SlotKeyType SImagesSelector::s_REMOVE_SLOT = "remove";
const core::com::Slots::SlotKeyType SImagesSelector::s_RESET_SLOT  = "reset";

const service::IService::KeyType s_SELECTION_INOUT = "selection";

//------------------------------------------------------------------------------
SImagesSelector::SImagesSelector() noexcept :
    m_captureIdx(0)
{
    newSlot(s_ADD_SLOT, &SImagesSelector::add, this);
    newSlot(s_REMOVE_SLOT, &SImagesSelector::remove, this);
    newSlot(s_RESET_SLOT, &SImagesSelector::reset, this);
}

//------------------------------------------------------------------------------

SImagesSelector::~SImagesSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SImagesSelector::configuring()
{
    sight::ui::base::IGuiContainer::initialize();
}

//------------------------------------------------------------------------------

void SImagesSelector::starting()
{
    m_frameTL = this->getInput<data::FrameTL>("frameTL");
    SIGHT_ASSERT("Frame timeline is not found.", m_frameTL);

    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    // Main container, VBox
    QVBoxLayout* vLayout = new QVBoxLayout();

    //   First HBox, displays number of items and the remove button
    QHBoxLayout* nbItemsHBox = new QHBoxLayout();

    //     Fill the nbItemsHBox
    QLabel* label = new QLabel("nb captures:");
    nbItemsHBox->addWidget(label);

    m_nbCapturesLabel = new QLabel("0");
    nbItemsHBox->addWidget(m_nbCapturesLabel);
    nbItemsHBox->addStretch();

    //   The ListWidget
    m_capturesListWidget = new QListWidget();

    // Fill the main VBox
    vLayout->addLayout(nbItemsHBox);
    vLayout->addWidget(m_capturesListWidget);

    qtContainer->setLayout(vLayout);

    this->updating();
}

//------------------------------------------------------------------------------

void SImagesSelector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SImagesSelector::updating()
{
    data::Vector::sptr vector = this->getInOut<data::Vector>(s_SELECTION_INOUT);

    m_capturesListWidget->clear();
    unsigned int captureIdx = 0;
    for(data::Object::sptr obj : vector->getContainer())
    {
        data::Image::sptr image = data::Image::dynamicCast(obj);
        if(image)
        {
            QString countString;

            countString = QString("%1. %2").arg(captureIdx).arg(QString::fromStdString(image->getID()));
            m_capturesListWidget->addItem(countString);
            ++captureIdx;
        }
    }

    m_nbCapturesLabel->setText(QString("%1").arg(captureIdx));
}

// ----------------------------------------------------------------------------

void SImagesSelector::remove()
{
    int idx = m_capturesListWidget->currentRow();

    if(idx >= 0)
    {
        data::Vector::sptr vector = this->getInOut<data::Vector>(s_SELECTION_INOUT);
        data::Object::sptr obj    = vector->getContainer()[idx];

        data::helper::Vector vectorHelper(vector);
        vectorHelper.remove(obj);
        vectorHelper.notify();

        this->updating();
    }
}

// ----------------------------------------------------------------------------

void SImagesSelector::reset()
{
    data::Vector::sptr vector = this->getInOut<data::Vector>(s_SELECTION_INOUT);

    data::helper::Vector vectorHelper(vector);
    vectorHelper.clear();
    vectorHelper.notify();

    m_capturesListWidget->clear();
    m_nbCapturesLabel->setText(QString("0"));
}

//------------------------------------------------------------------------------

void SImagesSelector::add(core::HiResClock::HiResClockType timestamp)
{
    CSPTR(data::FrameTL::BufferType) buffer = m_frameTL->getClosestBuffer(timestamp);

    if(!buffer)
    {
        SIGHT_INFO("Buffer not found with timestamp " << timestamp);
        return;
    }

    data::Image::sptr image = data::Image::New();

    data::Image::Size size;
    size[0] = m_frameTL->getWidth();
    size[1] = m_frameTL->getHeight();
    size[2] = 1;

    data::Image::PixelFormat format;
    // FIXME since frameTL does not have format information, we assume that image are Grayscale, RGB or RGBA according
    // to the number of components.
    switch(m_frameTL->getNumberOfComponents())
    {
        case 1:
            format = data::Image::GRAY_SCALE;
            break;

        case 3:
            format = data::Image::RGB;
            break;

        case 4:
            format = data::Image::RGBA;
            break;

        default:
            format = data::Image::UNDEFINED;
    }

    image->resize(size, m_frameTL->getType(), format);
    const data::Image::Origin origin = {0., 0., 0.};
    image->setOrigin2(origin);
    const data::Image::Spacing spacing = {1., 1., 1.};
    image->setSpacing2(spacing);
    image->setWindowWidth(100);
    image->setWindowCenter(0);

    const auto dumpLock = image->lock();

    const std::uint8_t* frameBuff = &buffer->getElement(0);
    std::uint8_t* imgBuffer       = static_cast<std::uint8_t*>(image->getBuffer());
    std::copy(frameBuff, frameBuff + buffer->getSize(), imgBuffer);

    data::Vector::sptr vector = this->getInOut<data::Vector>(s_SELECTION_INOUT);

    sight::data::helper::Vector vectorHelper(vector);
    vectorHelper.add(image);
    vectorHelper.notify();

    this->updating();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration
