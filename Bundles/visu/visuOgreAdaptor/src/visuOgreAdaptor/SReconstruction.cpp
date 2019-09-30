/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SReconstruction.hpp"

#include <visuOgreAdaptor/defines.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwRenderOgre/Material.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SReconstruction, ::fwData::Reconstruction)

const ::fwCom::Slots::SlotKeyType SReconstruction::s_CHANGE_MESH_SLOT = "changeMesh";
const ::fwCom::Slots::SlotKeyType SReconstruction::s_VISIBILITY_SLOT = "modifyVisibility";

static const std::string s_RECONSTRUCTION_INPUT = "reconstruction";

static const std::string s_AUTORESET_CAMERA_CONFIG = "autoresetcamera";
static const std::string s_QUERY_CONFIG            = "queryFlags";

//------------------------------------------------------------------------------

SReconstruction::SReconstruction() noexcept
{
    m_slots(s_CHANGE_MESH_SLOT, &SReconstruction::changeMesh, this);
    m_slots(s_VISIBILITY_SLOT, &SReconstruction::modifyVisibility, this);
}

//------------------------------------------------------------------------------

SReconstruction::~SReconstruction() noexcept
{
}

//------------------------------------------------------------------------------

void SReconstruction::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setTransformId(config.get<std::string>(::fwRenderOgre::ITransformable::s_TRANSFORM_CONFIG,
                                                 this->getID() + "_transform"));
    if (config.count(s_AUTORESET_CAMERA_CONFIG))
    {
        m_autoResetCamera = config.get<std::string>(s_AUTORESET_CAMERA_CONFIG) == "yes";
    }

    if(config.count(s_QUERY_CONFIG))
    {
        const std::string hexaMask = config.get<std::string>(s_QUERY_CONFIG);
        SLM_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2 &&
            hexaMask[0] == '0' && hexaMask[1] == 'x');
        m_queryFlags = static_cast< std::uint32_t >(std::stoul(hexaMask, nullptr, 16));
    }
}

//------------------------------------------------------------------------------

void SReconstruction::starting()
{
    this->initialize();

    createMeshService();
}

//------------------------------------------------------------------------------

void SReconstruction::createMeshService()
{
    // Retrieves the associated Reconstruction object
    ::fwData::Reconstruction::csptr reconstruction = this->getInput< ::fwData::Reconstruction >(s_RECONSTRUCTION_INPUT);
    ::fwData::Mesh::sptr mesh                      = reconstruction->getMesh();

    SLM_TRACE_IF("Mesh is null", !mesh);
    if (mesh)
    {
        // Creates an Ogre adaptor and associates it with the Sight mesh object
        auto meshAdaptor = this->registerService< ::visuOgreAdaptor::SMesh >("::visuOgreAdaptor::SMesh");
        meshAdaptor->registerInOut(mesh, "mesh", true);

        meshAdaptor->setID(this->getID() + meshAdaptor->getID());
        meshAdaptor->setLayerID(m_layerID);
        meshAdaptor->setRenderService(this->getRenderService());

        meshAdaptor->setIsReconstructionManaged(true);
        meshAdaptor->setMaterial(reconstruction->getMaterial());
        meshAdaptor->setMaterialTemplateName(m_materialTemplateName);
        meshAdaptor->setAutoResetCamera(m_autoResetCamera);
        meshAdaptor->setTransformId(this->getTransformId());
        meshAdaptor->updateVisibility(reconstruction->getIsVisible());
        meshAdaptor->setDynamic(m_isDynamic);
        meshAdaptor->setDynamicVertices(m_isDynamicVertices);
        meshAdaptor->setQueryFlags(m_queryFlags);

        meshAdaptor->start();

        m_meshAdaptor = meshAdaptor;

        OSLM_TRACE("Mesh is visible : " << reconstruction->getIsVisible());
        OSLM_TRACE("Mesh nb points : " << mesh->getNumberOfPoints());
    }
}

//------------------------------------------------------------------------------

void SReconstruction::updating()
{
    if (!m_meshAdaptor.expired())
    {
        auto reconstruction = this->getInput< ::fwData::Reconstruction >(s_RECONSTRUCTION_INPUT);
        ::visuOgreAdaptor::SMesh::sptr meshAdaptor = this->getMeshAdaptor();

        // Do nothing if the mesh is identical
        auto mesh = ::fwServices::OSR::getRegistered("mesh", ::fwServices::IService::AccessType::INOUT, meshAdaptor);
        if(mesh != reconstruction->getMesh())
        {
            // Updates the mesh adaptor according to the reconstruction
            meshAdaptor->setMaterial(reconstruction->getMaterial());
            meshAdaptor->swap(reconstruction->getMesh());
            meshAdaptor->updateVisibility(reconstruction->getIsVisible());
        }
    }
    else
    {
        // If m_meshService does not exists, we have to create it
        this->createMeshService();
    }
}

//------------------------------------------------------------------------------

void SReconstruction::stopping()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SReconstruction::setForceHide(bool _hide)
{
    if (!m_meshAdaptor.expired())
    {
        ::visuOgreAdaptor::SMesh::sptr meshAdaptor = this->getMeshAdaptor();

        if (meshAdaptor)
        {
            auto reconstruction = this->getInput< ::fwData::Reconstruction >(s_RECONSTRUCTION_INPUT);
            meshAdaptor->updateVisibility(_hide ? false : reconstruction->getIsVisible());
        }
    }
}

//------------------------------------------------------------------------------

void SReconstruction::changeMesh(::fwData::Mesh::sptr)
{
    if (!m_meshAdaptor.expired())
    {
        auto reconstruction = this->getInput< ::fwData::Reconstruction >(s_RECONSTRUCTION_INPUT);
        SLM_ASSERT("reconstruction not instantiated", reconstruction);
        this->updating();
    }
    else
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

void SReconstruction::modifyVisibility()
{
    if (!m_meshAdaptor.expired())
    {
        auto reconstruction = this->getInput< ::fwData::Reconstruction >(s_RECONSTRUCTION_INPUT);
        SLM_ASSERT("reconstruction not instantiated", reconstruction);

        this->setForceHide(!reconstruction->getIsVisible());
    }
}

//------------------------------------------------------------------------------

::visuOgreAdaptor::SMesh::sptr SReconstruction::getMeshAdaptor()
{
    // Retrieves the associated mesh adaptor
    ::fwRenderOgre::IAdaptor::sptr adaptor     = m_meshAdaptor.lock();
    ::visuOgreAdaptor::SMesh::sptr meshAdaptor = ::visuOgreAdaptor::SMesh::dynamicCast(adaptor);

    return meshAdaptor;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap visuOgreAdaptor::SReconstruction::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_RECONSTRUCTION_INPUT, ::fwData::Reconstruction::s_MESH_CHANGED_SIG, s_CHANGE_MESH_SLOT );
    connections.push( s_RECONSTRUCTION_INPUT, ::fwData::Reconstruction::s_VISIBILITY_MODIFIED_SIG, s_VISIBILITY_SLOT );
    return connections;
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
