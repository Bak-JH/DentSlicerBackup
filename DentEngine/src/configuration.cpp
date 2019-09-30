#include "configuration.h"
//#include "viewer3d.h"

#include <QQmlContext>
#include <QTimer>
#include <Qt3DCore>
SlicingConfiguration* scfg = new SlicingConfiguration();

SlicingConfiguration::SlicingConfiguration()
{
	setBedX(123.8f);
	setBedY(69.2f );
	_bedBound.setZMin(0.0f);
	setBedHeight(120.0f);

	setResolutionX(2560);
	setResolutionY(1440);

	setScreenX(120.96f);
	setScreenY(68.04f);
}

float SlicingConfiguration::raft_base_radius() const
{
	return support_radius_max* 3.0f;
}

void SlicingConfiguration::setBedX(float val)
{
	_bedX = val;
	_bedBound.setXLength(val);
}

void SlicingConfiguration::setBedY(float val)
{
	_bedY = val;
	_bedBound.setYLength(val);
}

void SlicingConfiguration::setBedHeight(float val)
{
	_bedBound.setZMax(val);
}


void SlicingConfiguration::setResolutionX(float val)
{
	_resolutionX = val;
}

void SlicingConfiguration::setResolutionY(float val)
{
	_resolutionY = val;
}

void SlicingConfiguration::setScreenX(float val)
{
	_screenX = val;
	_pixelPerMMX = _resolutionX / _screenX;
}

void SlicingConfiguration::setScreenY(float val)
{
	_screenY = val;
	_pixelPerMMY = _resolutionY / _screenY;
}

float SlicingConfiguration::screenX() const
{
	return _resolutionX;
}
float SlicingConfiguration::screenY() const
{
	return _resolutionY;
}

float SlicingConfiguration::resolutionX() const
{
	return _resolutionX;
}

float SlicingConfiguration::resolutionY() const
{
	return _resolutionY;
}

float SlicingConfiguration::bedX() const
{
	return _bedX;
}

float SlicingConfiguration::bedY() const
{
	return _bedY;
}

float SlicingConfiguration::bedHeight() const
{
	return _bedBound.zMax();
}

const Hix::Engine3D::Bounds3D& SlicingConfiguration::bedBound() const
{
	return _bedBound;
}

float SlicingConfiguration::pixelPerMMX()const
{
	return _pixelPerMMX;
}
float SlicingConfiguration::pixelPerMMY()const
{
	return _pixelPerMMY;
}
