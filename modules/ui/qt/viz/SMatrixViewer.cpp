/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "SMatrixViewer.hpp"

#include <data/Matrix4.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>

namespace sight::module::ui::qt
{

namespace viz
{

static const service::IService::KeyType s_MATRIX_INPUT = "matrix";

// ------------------------------------------------------------------------------

SMatrixViewer::SMatrixViewer() noexcept
{
}

// ------------------------------------------------------------------------------

SMatrixViewer::~SMatrixViewer() noexcept
{
}

// ------------------------------------------------------------------------------

void SMatrixViewer::configuring()
{
    sight::ui::base::IGuiContainer::initialize();

    const auto config = this->getConfigTree();

    m_title = config.get<std::string>("title", "matrix");
    const std::string displayMode = config.get("display", "matrix");
    if(displayMode == "line")
    {
        m_displayMode = Display::LINE;
    }
    else
    {
        m_displayMode = Display::MATRIX;
        SIGHT_ASSERT("Only 'matrix' and 'line' values are managed for 'display' option.", displayMode == "matrix");
    }
}

// ------------------------------------------------------------------------------

void SMatrixViewer::starting()
{
    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    if(!m_title.empty())
    {
        QPointer<QLabel> description = new QLabel(QString::fromStdString(m_title));
        mainLayout->addWidget(description);
    }

    if(m_displayMode == Display::MATRIX)
    {
        QGridLayout* gridLayout = new QGridLayout();

        for(int i = 0 ; i < 4 ; ++i)
        {
            for(int j = 0 ; j < 4 ; ++j)
            {
                QLabel* label = new QLabel("");
                m_matrixLabels.push_back(label);
                gridLayout->addWidget(label, i, j);
            }
        }

        mainLayout->addLayout(gridLayout);
    }
    else
    {
        QHBoxLayout* layout = new QHBoxLayout();

        for(int i = 0 ; i < 16 ; ++i)
        {
            QLabel* label = new QLabel("");
            m_matrixLabels.push_back(label);
            layout->addWidget(label);
        }

        mainLayout->addLayout(layout);
    }

    qtContainer->setLayout(mainLayout);

    this->updateFromMatrix();
}

// ------------------------------------------------------------------------------

void SMatrixViewer::stopping()
{
    this->destroy();
}

// ------------------------------------------------------------------------------

void SMatrixViewer::updating()
{
    this->updateFromMatrix();
}

// ------------------------------------------------------------------------------

void SMatrixViewer::updateFromMatrix()
{
    auto matrix = this->getLockedInput<data::Matrix4>(s_MATRIX_INPUT);
    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        for(unsigned int j = 0 ; j < 4 ; ++j)
        {
            const int index = static_cast<int>(i * 4 + j);
            m_matrixLabels[index]->setText(QString("%1").arg(matrix->getCoefficient(i, j), 10));
        }
    }
}

// ------------------------------------------------------------------------------

void SMatrixViewer::clearLabels()
{
    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        for(unsigned int j = 0 ; j < 4 ; ++j)
        {
            const int index = static_cast<int>(i * 4 + j);
            m_matrixLabels[index]->setText(QString(""));
        }
    }
}

// ------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SMatrixViewer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_MATRIX_INPUT, data::Matrix4::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ------------------------------------------------------------------------------

} // namespace viz

} // namespace sight::module::ui::qt
