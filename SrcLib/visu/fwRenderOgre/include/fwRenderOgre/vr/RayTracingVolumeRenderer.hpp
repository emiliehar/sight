/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_VR_RAYTRACINGVOLUMERENDERER_HPP__
#define __FWRENDEROGRE_VR_RAYTRACINGVOLUMERENDERER_HPP__

#include "fwRenderOgre/compositor/listener/AutoStereo.hpp"
#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/R2VBRenderable.hpp"
#include "fwRenderOgre/vr/IVolumeRenderer.hpp"
#include "fwRenderOgre/vr/SATVolumeIllumination.hpp"

#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRectangle2D.h>
#include <OGRE/OgreTechnique.h>

#include <vector>

namespace fwRenderOgre
{

namespace vr
{

/**
 * @brief Implements a simple GPU ray-tracing renderer.
 */
class FWRENDEROGRE_CLASS_API RayTracingVolumeRenderer : public IVolumeRenderer
{
public:

    enum class IDVRCSGModulationMethod
    {
        AVERAGE_GRAYSCALE,
        LIGHTNESS_GRAYSCALE,
        LUMINOSITY_GRAYSCALE,
        COLOR1,
        COLOR2,
        COLOR3,
        COLOR4
    };

    /**
     * @brief Constructor.
     * @param parentId             ID of the service using this renderer
     * @param sceneManager         The scene manager being used.
     * @param volumeNode           This object's node.
     * @param imageTexture         Texture holding the 3D image to be rendered.
     * @param gpuTF                Texture holding the transfer function.
     * @param preintegrationTable  Texture holding the pre-integration table.
     * @param mode3D               Stereoscopic volume rendering flag.
     * @param ambientOcclusion     Ambient occlusion flag.
     * @param colorBleeding        Color bleeding flag.
     * @param shadows              Soft shadows flag.
     */
    FWRENDEROGRE_API RayTracingVolumeRenderer(std::string parentId,
                                              Layer::sptr layer,
                                              ::Ogre::SceneNode* parentNode,
                                              ::Ogre::TexturePtr imageTexture,
                                              ::Ogre::TexturePtr maskTexture,
                                              TransferFunction& gpuTF,
                                              PreIntegrationTable& preintegrationTable,
                                              ::fwRenderOgre::Layer::StereoModeType stereoMode,
                                              bool ambientOcclusion = false,
                                              bool colorBleeding = false,
                                              bool shadows = false,
                                              double aoFactor = 1.f,
                                              double colorBleedingFactor = 1.f);

    /// Does nothing.
    FWRENDEROGRE_API virtual ~RayTracingVolumeRenderer();

    /// Function called when a new image is being rendered.
    FWRENDEROGRE_API virtual void imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf);

    /// Called when the transfer function is updated.
    FWRENDEROGRE_API virtual void tfUpdate(::fwData::TransferFunction::sptr tf);

    /// Sets the number of samples per view ray.
    FWRENDEROGRE_API virtual void setSampling(uint16_t nbSamples);

    /// Sets the opacity correction factor.
    FWRENDEROGRE_API void setOpacityCorrection(int opacityCorrection);

    /// Ambient occlusion factor setter.
    FWRENDEROGRE_API virtual void setAOFactor(double aoFactor);

    /// Color bleeding factor setter.
    FWRENDEROGRE_API virtual void setColorBleedingFactor(double colorBleedingFactor);

    FWRENDEROGRE_API virtual void setIlluminationVolume(SATVolumeIllumination* illuminationVolume);

    /// Sets pre-integrated mode.
    FWRENDEROGRE_API virtual void setPreIntegratedRendering(bool preIntegratedRendering);

    /// Sets ambient occlusion usage.
    FWRENDEROGRE_API virtual void setAmbientOcclusion(bool ambientOcclusion);

    /// Sets color bleeding usage.
    FWRENDEROGRE_API virtual void setColorBleeding(bool colorBleeding);

    /// Sets soft shadows usage.
    FWRENDEROGRE_API virtual void setShadows(bool shadows);

    /// Sets the focal distance used for stereo rendering.
    /// A focal length of 0 focuses on the front of the image and a length of 1 focuses on the back.
    FWRENDEROGRE_API void setFocalLength(float focalLength);

    /// Allows to setup the importance driven method used during the rendering.
    FWRENDEROGRE_API void setIDVRMethod(std::string method);

    /// Computes image positions, updates the proxy geometry.
    FWRENDEROGRE_API virtual void clipImage(const ::Ogre::AxisAlignedBox& clippingBox);

    /// Slot: Called when the size of the viewport changes.
    FWRENDEROGRE_API virtual void resizeViewport(int w, int h);

    /// IllumVolume getter.
    FWRENDEROGRE_API SATVolumeIllumination* getIllumVolume();

    /// Toggle countersink geometry when using Importance Driven Volume Rendering.
    FWRENDEROGRE_API void toggleIDVRCountersinkGeometry(bool);

    /// Setup the countersink geometry slope used in the MImP method.
    FWRENDEROGRE_API void setIDVRCountersinkSlope(double);

    /// Setup the countersink geometry blur weight factor used in the MImP method.
    FWRENDEROGRE_API void setIDVRCSGBlurWeight(double);

    /// Toggle countersink geometry border used in the MImP method.
    FWRENDEROGRE_API void toggleIDVRCSGBorder(bool);

    /// Toggle context discard when using MImP countersink geometry.
    FWRENDEROGRE_API void toggleIDVRCSGDisableContext(bool);

    /// Setup the countersink geometry border thickness used in the MImP method.
    FWRENDEROGRE_API void setIDVRCSGBorderThickness(double);

    /// Setup the countersink geometry border color used in the MImP method.
    FWRENDEROGRE_API void setIDVRCSGBorderColor(std::array<std::uint8_t, 4>);

    /// Toggle the grayscale modulation for MImP countersink geometry.
    FWRENDEROGRE_API void toggleIDVRCSGModulation(bool);

    /// Setup the grayscale modulation method used for MImP countersink geometry.
    FWRENDEROGRE_API void setIDVRCSModulationMethod(IDVRCSGModulationMethod);

    /// Setup the wheighting factor for MImP CSG color modulation.
    FWRENDEROGRE_API void setIDVRCSGModulationFactor(double);

    /// Toggle the opacity decrease for MImP countersink geometry.
    FWRENDEROGRE_API void toggleIDVRCSGOpacityDecrease(bool);

    /// Setup the opacity decrease factor used in the MImP CSG.
    FWRENDEROGRE_API void setIDVRCSGOpacityDecreaseFactor(double);

    /// Toggle the depth lines for MImP countersink geometry.
    FWRENDEROGRE_API void toggleIDVRDepthLines(bool);

    /// Setup the depth lines gradation threshold used in the MImP CSG.
    FWRENDEROGRE_API void setIDVRCSGDepthLinesThreshold(double);

    /// Setup the alpha correction factor used in the VPImC method.
    FWRENDEROGRE_API void setIDVRAImCAlphaCorrection(double);

    /// Setup the alpha correction factor used in the VPImC method.
    FWRENDEROGRE_API void setIDVRVPImCAlphaCorrection(double);

    /// Layer getter
    ::fwRenderOgre::Layer::sptr getLayer() const;

private:

    /// When using AutoStereo compositor, initialize the raytracing material.
    void createRayTracingMaterial();

    /// Initializes the compositors used after the step computing the ray entry points
    void initCompositors();

    /// Creates the proxy geometry defining the entry points for rays.
    void initEntryPoints();

    /// Creates a new grid texture and sets shader parameters for volume bricking.
    void createGridTexture();

    /// Renders the proxy geometry to fill the entry point texture.
    void computeEntryPointsTexture();

    /// Compute the focal length in camera space.
    void computeRealFocalLength();

    /// Computes the shear warp to apply to a frustum for multi-view rendering based on the angle with the original
    /// camera.
    ::Ogre::Matrix4 frustumShearTransform(float angle) const;

    /// Updates the ray traced and volume illumination materials according to pre-integration and volume illumination
    /// flags.
    void updateVolIllumMat();

    /// Updates the current compositor name according to VR effects flags.
    /// @return tuple containing a
    /// - Comma separated list of preprocessor defines to use in vertex shaders.
    /// - Comma separated list of preprocessor defines to use in fragment shaders.
    /// - Hash allowing to identify the material
    std::tuple<std::string, std::string, size_t> computeRayTracingDefines() const;

    /// Sets the default diffuse, specular and shininess in the material.
    void setMaterialLightParams(::Ogre::MaterialPtr mtl);

    /// Creates and adds importance compositing compositors to the chain (MImP + JFA, AImC or VPImC).
    void buildICCompositors();

    /// Removes all listeners and compositors from the current chain.
    void cleanCompositorChain();

    /// Texture of the segmentation mask.
    ::Ogre::TexturePtr m_maskTexture;

    /// Object containing the proxy geometry, this is a cube for now.
    ::Ogre::ManualObject* m_entryPointGeometry;

    /// Creates and updates the proxy geometry.
    R2VBRenderable* m_proxyGeometryGenerator;

    /// Entity holding the source geometry used for proxy geometry rendering.
    ::Ogre::Entity* m_r2vbSource;

    /// Grid defining volume bricks.
    ::Ogre::TexturePtr m_gridTexture;

    /// Ray entry and exit points for each pixel of each viewpoint.
    std::vector< ::Ogre::TexturePtr> m_entryPointsTextures;

    std::vector< ::Ogre::TextureUnitState* > m_rayTracedTexUnitStates;

    /// Render operation used to compute the brick grid.
    ::Ogre::RenderOperation m_gridRenderOp;

    /// Image dimensions.
    ::fwData::Image::SizeType m_imageSize;

    /// Brick Grid dimensions.
    std::array< int, 3 > m_gridSize;

    /// Size of a volume brick.
    std::array< int, 3 > m_brickSize;

    /// Sets stereoscopic volume rendering for autostereoscopic monitors.
    ::fwRenderOgre::Layer::StereoModeType m_stereoMode;

    /// Sets usage of ambient occlusion.
    bool m_ambientOcclusion;

    /// Sets usage of color bleeding.
    bool m_colorBleeding;

    /// Sets usage of soft shadows.
    bool m_shadows;

    /// Sets usage of countersink geometry for MImP.
    bool m_idvrCSG;

    /// Sets countersink geometry slope for MImP.
    float m_idvrCSGSlope;

    /// Sets countersink geometry blur weight for MImP.
    float m_idvrCSGBlurWeight;

    /// Sets usage of countersink geometry border for MImP.
    bool m_idvrCSGBorder;

    /// Sets whether or not the context should be discarded when using MImP countersink geometry.
    bool m_idvrCSGDisableContext;

    /// Sets countersink geometry border thickness for MImP.
    float m_idvrCSGBorderThickness;

    /// Sets countersink geometry border color for MImP.
    ::Ogre::ColourValue m_idvrCSGBorderColor;

    /// Sets usage of modulation for MImP CSG.
    bool m_idvrCSGModulation;

    /// Name of the method used to compute the new color values in CSG.
    IDVRCSGModulationMethod m_idvrCSGModulationMethod;

    /// Sets the wheighting factor for MImP CSG color modulation.
    float m_idvrCSGModulationFactor;

    /// Sets usage of opacity decrease for MImP CSG.
    bool m_idvrCSGOpacityDecrease;

    /// Sets the opacity decrease factor used in MImP CSG.
    float m_idvrCSGOpacityDecreaseFactor;

    /// Sets usage of depth lines for MImP CSG.
    bool m_idvrCSGDepthLines;

    /// Sets the gradation threshold of MImP CSG's depth lines.
    float m_idvrCSGDepthLinesThreshold;

    /// Sets the alpha correction for AImC.
    float m_idvrAImCAlphaCorrection;

    /// Sets the alpha correction for VPImC.
    float m_idvrVPImCAlphaCorrection;

    /// Factor parameter used to weight ambient occlusion (A channel) and color bleeding (RGB channels).
    ::Ogre::Vector4 m_volIllumFactor;

    /// Inverse of the sampling rate accounted by the TF.
    float m_opacityCorrectionFactor;

    /// Shared parameters used for Ray tracing. This should help avoiding using the listener.
    /// We resort to those parameters because setting them using:
    /// ::Ogre::MaterialManager::getSingletonPtr()->getByName("RTV_Mat")->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant(paramName,
    /// m_idvrAlphaCorrection);
    /// Only seems to update them when instancing the corresponding material
    ::Ogre::GpuSharedParametersPtr m_RTVSharedParameters;

    SATVolumeIllumination* m_illumVolume;

    /// Name of the method used for Importance driven volume rendering
    std::string m_idvrMethod;

    /// Focal distance in object space : 0 = object front, 1 = object back.
    float m_focalLength;

    /// Lobe offset parameter used in 3D mode
    float m_lobeOffset;

    /// Camera listener class used to compute the entry points textures before rendering.
    struct CameraListener;
    CameraListener* m_cameraListener;

    /// Compositor listener classes used to upload uniforms for mono/stereo ray tracing materials.
    class JFACompositorListener;

    /// List of all listeners associated to the VR's compositor chain.
    /// If a compositor has no listener, we store a nullptr in the corresponding entry.
    std::vector< ::Ogre::CompositorInstance::Listener*> m_compositorListeners;

    /// Name of the material
    std::string m_currentMtlName;

    ::fwRenderOgre::Layer::wptr m_layer;

    /// Autostereo listener
    compositor::listener::AutoStereoCompositorListener* m_autostereoListener;

    ::Ogre::Rectangle2D* m_fullScreenQuad;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline ::fwRenderOgre::vr::SATVolumeIllumination* RayTracingVolumeRenderer::getIllumVolume()
{
    return m_illumVolume;
}

//-----------------------------------------------------------------------------

inline ::fwRenderOgre::Layer::sptr RayTracingVolumeRenderer::getLayer() const
{
    return m_layer.lock();
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_VR_RAYTRACINGVOLUMERENDERER_HPP__
