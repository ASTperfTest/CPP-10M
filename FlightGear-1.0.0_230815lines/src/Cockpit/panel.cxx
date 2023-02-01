//  panel.cxx - default, 2D single-engine prop instrument panel
//
//  Written by David Megginson, started January 2000.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License as
//  published by the Free Software Foundation; either version 2 of the
//  License, or (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//  $Id: panel.cxx,v 1.44 2006-09-05 20:28:48 curt Exp $

//JVK
// On 2D panels all instruments include light sources were in night displayed
// with a red mask (instrument light). It is not correct for light sources
// (bulbs). There is added new layer property "emissive" (boolean) (only for
// textured layers).
// If a layer has to shine set it in the "instrument_def_file.xml" inside the
// <layer> tag by adding <emissive>true</emissive> tag. When omitted the default
// value is for backward compatibility set to false.

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_WINDOWS_H          
#  include <windows.h>
#endif

#include <stdio.h>	// sprintf
#include <string.h>

#include <simgear/compiler.h>

#include SG_GLU_H

#include <plib/ssg.h>
#include <plib/fnt.h>

#include <simgear/debug/logstream.hxx>
#include <simgear/misc/sg_path.hxx>

#include <Main/globals.hxx>
#include <Main/fg_props.hxx>
#include <Main/viewmgr.hxx>
#include <Time/light.hxx>
#include <GUI/new_gui.hxx>	// FGFontCache

#include "hud.hxx"
#include "panel.hxx"

#define WIN_X 0
#define WIN_Y 0
#define WIN_W 1024
#define WIN_H 768

// The number of polygon-offset "units" to place between layers.  In
// principle, one is supposed to be enough.  In practice, I find that
// my hardware/driver requires many more.
#define POFF_UNITS 8

////////////////////////////////////////////////////////////////////////
// Local functions.
////////////////////////////////////////////////////////////////////////


/**
 * Calculate the aspect adjustment for the panel.
 */
static float
get_aspect_adjust (int xsize, int ysize)
{
  float ideal_aspect = float(WIN_W) / float(WIN_H);
  float real_aspect = float(xsize) / float(ysize);
  return (real_aspect / ideal_aspect);
}



////////////////////////////////////////////////////////////////////////
// Global functions.
////////////////////////////////////////////////////////////////////////

bool
fgPanelVisible ()
{
     if (globals->get_current_panel() == 0)
	return false;
     if (globals->get_current_panel()->getVisibility() == 0)
	return false;
     if (globals->get_viewmgr()->get_current() != 0)
	return false;
     if (globals->get_current_view()->getHeadingOffset_deg() * SGD_DEGREES_TO_RADIANS != 0)
	return false;
     return true;
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGTextureManager.
////////////////////////////////////////////////////////////////////////

map<string,ssgTexture *> FGTextureManager::_textureMap;

ssgTexture *
FGTextureManager::createTexture (const string &relativePath)
{
  ssgTexture * texture = _textureMap[relativePath];
  if (texture == 0) {
    SG_LOG( SG_COCKPIT, SG_DEBUG,
            "Texture " << relativePath << " does not yet exist" );
    SGPath tpath(globals->get_fg_root());
    tpath.append(relativePath);
    texture = new ssgTexture((char *)tpath.c_str(), false, false);
    _textureMap[relativePath] = texture;
    if (_textureMap[relativePath] == 0) 
      SG_LOG( SG_COCKPIT, SG_ALERT, "Texture *still* doesn't exist" );
    SG_LOG( SG_COCKPIT, SG_DEBUG, "Created texture " << relativePath
            << " handle=" << texture->getHandle() );
  }

  return texture;
}




////////////////////////////////////////////////////////////////////////
// Implementation of FGCropped Texture.
////////////////////////////////////////////////////////////////////////


FGCroppedTexture::FGCroppedTexture ()
  : _path(""), _texture(0),
    _minX(0.0), _minY(0.0), _maxX(1.0), _maxY(1.0)
{
}


FGCroppedTexture::FGCroppedTexture (const string &path,
				    float minX, float minY,
				    float maxX, float maxY)
  : _path(path), _texture(0),
    _minX(minX), _minY(minY), _maxX(maxX), _maxY(maxY)
{
}


FGCroppedTexture::~FGCroppedTexture ()
{
}


ssgTexture *
FGCroppedTexture::getTexture ()
{
  if (_texture == 0) {
    _texture = FGTextureManager::createTexture(_path);
  }
  return _texture;
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGPanel.
////////////////////////////////////////////////////////////////////////

static fntRenderer text_renderer;
static sgVec4 panel_color;
static sgVec4 emissive_panel_color = {1,1,1,1};

/**
 * Constructor.
 */
FGPanel::FGPanel ()
  : _mouseDown(false),
    _mouseInstrument(0),
    _width(WIN_W), _height(int(WIN_H * 0.5768 + 1)),
    _view_height(int(WIN_H * 0.4232)),
    _visibility(fgGetNode("/sim/panel/visibility", true)),
    _x_offset(fgGetNode("/sim/panel/x-offset", true)),
    _y_offset(fgGetNode("/sim/panel/y-offset", true)),
    _jitter(fgGetNode("/sim/panel/jitter", true)),
    _flipx(fgGetNode("/sim/panel/flip-x", true)),
    _xsize_node(fgGetNode("/sim/startup/xsize", true)),
    _ysize_node(fgGetNode("/sim/startup/ysize", true)),
    _enable_depth_test(false)
{
}


/**
 * Destructor.
 */
FGPanel::~FGPanel ()
{
  for (instrument_list_type::iterator it = _instruments.begin();
       it != _instruments.end();
       it++) {
    delete *it;
    *it = 0;
  }
}


/**
 * Add an instrument to the panel.
 */
void
FGPanel::addInstrument (FGPanelInstrument * instrument)
{
  _instruments.push_back(instrument);
}


/**
 * Initialize the panel.
 */
void
FGPanel::init ()
{
}


/**
 * Bind panel properties.
 */
void
FGPanel::bind ()
{
  fgSetArchivable("/sim/panel/visibility");
  fgSetArchivable("/sim/panel/x-offset");
  fgSetArchivable("/sim/panel/y-offset");
  fgSetArchivable("/sim/panel/jitter");
}


/**
 * Unbind panel properties.
 */
void
FGPanel::unbind ()
{
}


/**
 * Update the panel.
 */
void
FGPanel::update (double dt)
{
				// Do nothing if the panel isn't visible.
    if ( !fgPanelVisible() ) {
        return;
    }

    updateMouseDelay();

				// Now, draw the panel
    float aspect_adjust = get_aspect_adjust(_xsize_node->getIntValue(),
                                            _ysize_node->getIntValue());
    if (aspect_adjust <1.0)
        update(WIN_X, int(WIN_W * aspect_adjust), WIN_Y, WIN_H);
    else
        update(WIN_X, WIN_W, WIN_Y, int(WIN_H / aspect_adjust));
}

/**
 * Handle repeatable mouse events.  Called from update() and from
 * fgUpdate3DPanels().  This functionality needs to move into the
 * input subsystem.  Counting a tick every two frames is clumsy...
 */
void FGPanel::updateMouseDelay()
{
    if (_mouseDown) {
        _mouseDelay--;
        if (_mouseDelay < 0) {
            _mouseInstrument->doMouseAction(_mouseButton, 0, _mouseX, _mouseY);
            _mouseDelay = 2;
        }
    }
}


void
FGPanel::update (GLfloat winx, GLfloat winw, GLfloat winy, GLfloat winh)
{
				// Calculate accelerations
				// and jiggle the panel accordingly
				// The factors and bounds are just
				// initial guesses; using sqrt smooths
				// out the spikes.
  double x_offset = _x_offset->getIntValue();
  double y_offset = _y_offset->getIntValue();

#if 0
  if (_jitter->getFloatValue() != 0.0) {
    double a_x_pilot = current_aircraft.fdm_state->get_A_X_pilot();
    double a_y_pilot = current_aircraft.fdm_state->get_A_Y_pilot();
    double a_z_pilot = current_aircraft.fdm_state->get_A_Z_pilot();

    double a_zx_pilot = a_z_pilot - a_x_pilot;
    
    int x_adjust = int(sqrt(fabs(a_y_pilot) * _jitter->getFloatValue())) *
		   (a_y_pilot < 0 ? -1 : 1);
    int y_adjust = int(sqrt(fabs(a_zx_pilot) * _jitter->getFloatValue())) *
		   (a_zx_pilot < 0 ? -1 : 1);

				// adjustments in screen coordinates
    x_offset += x_adjust;
    y_offset += y_adjust;
  }
#endif

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  if ( _flipx->getBoolValue() ) {
    gluOrtho2D(winx + winw, winx, winy + winh, winy); /* up side down */
  } else {
    gluOrtho2D(winx, winx + winw, winy, winy + winh); /* right side up */
  }
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glTranslated(x_offset, y_offset, 0);
  
  draw();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  ssgForceBasicState();
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void
FGPanel::draw()
{
  // In 3D mode, it's possible that we are being drawn exactly on top
  // of an existing polygon.  Use an offset to prevent z-fighting.  In
  // 2D mode, this is a no-op.
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(-1, -POFF_UNITS);

  // save some state
  glPushAttrib( GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT
                | GL_TEXTURE_BIT | GL_PIXEL_MODE_BIT | GL_CULL_FACE 
                | GL_DEPTH_BUFFER_BIT );

  // Draw the background
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  if ( _enable_depth_test )
      glDepthFunc(GL_ALWAYS);
  else
    glDisable(GL_DEPTH_TEST);

  FGLight *l = (FGLight *)(globals->get_subsystem("lighting"));
  sgCopyVec4( panel_color, l->scene_diffuse());
  if ( fgGetDouble("/systems/electrical/outputs/instrument-lights") > 1.0 ) {
      if ( panel_color[0] < 0.7 ) panel_color[0] = 0.7;
      if ( panel_color[1] < 0.2 ) panel_color[1] = 0.2;
      if ( panel_color[2] < 0.2 ) panel_color[2] = 0.2;
  }
  glColor4fv( panel_color );
  if (_bg != 0) {
    glBindTexture(GL_TEXTURE_2D, _bg->getHandle());
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex2f(WIN_X, WIN_Y);
    glTexCoord2f(1.0, 0.0); glVertex2f(WIN_X + _width, WIN_Y);
    glTexCoord2f(1.0, 1.0); glVertex2f(WIN_X + _width, WIN_Y + _height);
    glTexCoord2f(0.0, 1.0); glVertex2f(WIN_X, WIN_Y + _height);
    glEnd();
  } else {
    for (int i = 0; i < 4; i ++) {
      // top row of textures...(1,3,5,7)
      glBindTexture(GL_TEXTURE_2D, _mbg[i*2]->getHandle());
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBegin(GL_POLYGON);
      glTexCoord2f(0.0, 0.0); glVertex2f(WIN_X + (_width/4) * i, WIN_Y + (_height/2));
      glTexCoord2f(1.0, 0.0); glVertex2f(WIN_X + (_width/4) * (i+1), WIN_Y + (_height/2));
      glTexCoord2f(1.0, 1.0); glVertex2f(WIN_X + (_width/4) * (i+1), WIN_Y + _height);
      glTexCoord2f(0.0, 1.0); glVertex2f(WIN_X + (_width/4) * i, WIN_Y + _height);
      glEnd();
      // bottom row of textures...(2,4,6,8)
      glBindTexture(GL_TEXTURE_2D, _mbg[(i*2)+1]->getHandle());
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glBegin(GL_POLYGON);
      glTexCoord2f(0.0, 0.0); glVertex2f(WIN_X + (_width/4) * i, WIN_Y);
      glTexCoord2f(1.0, 0.0); glVertex2f(WIN_X + (_width/4) * (i+1), WIN_Y);
      glTexCoord2f(1.0, 1.0); glVertex2f(WIN_X + (_width/4) * (i+1), WIN_Y + (_height/2));
      glTexCoord2f(0.0, 1.0); glVertex2f(WIN_X + (_width/4) * i, WIN_Y + (_height/2));
      glEnd();
    }
  }

  // Draw the instruments.
  // Syd Adams: added instrument clipping
  instrument_list_type::const_iterator current = _instruments.begin();
  instrument_list_type::const_iterator end = _instruments.end();

  GLdouble blx[4]={1.0,0.0,0.0,0.0};
  GLdouble bly[4]={0.0,1.0,0.0,0.0};
  GLdouble urx[4]={-1.0,0.0,0.0,0.0};
  GLdouble ury[4]={0.0,-1.0,0.0,0.0};

  for ( ; current != end; current++) {
    FGPanelInstrument * instr = *current;
    glPushMatrix();
    glTranslated(instr->getXPos(), instr->getYPos(), 0);

    int ix= instr->getWidth();
    int iy= instr->getHeight();
    glPushMatrix();
    glTranslated(-ix/2,-iy/2,0);
    glClipPlane(GL_CLIP_PLANE0,blx);
    glClipPlane(GL_CLIP_PLANE1,bly);
    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_PLANE1);

    glTranslated(ix,iy,0);
    glClipPlane(GL_CLIP_PLANE2,urx);
    glClipPlane(GL_CLIP_PLANE3,ury);
    glEnable(GL_CLIP_PLANE2);
    glEnable(GL_CLIP_PLANE3);
    glPopMatrix();
    instr->draw();

    glPopMatrix();
  }

  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);
  glDisable(GL_CLIP_PLANE3);


  // Draw yellow "hotspots" if directed to.  This is a panel authoring
  // feature; not intended to be high performance or to look good.
  if ( fgGetBool("/sim/panel-hotspots") ) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(1, 1, 0);
    
    for ( unsigned int i = 0; i < _instruments.size(); i++ )
      _instruments[i]->drawHotspots();
  }


  // restore some original state
  if ( _enable_depth_test )
    glDepthFunc(GL_LESS);
  glPopAttrib();
  glPolygonOffset(0, 0);
  glDisable(GL_POLYGON_OFFSET_FILL);
}

/**
 * Set the panel's visibility.
 */
void
FGPanel::setVisibility (bool visibility)
{
  _visibility->setBoolValue( visibility );
}


/**
 * Return true if the panel is visible.
 */
bool
FGPanel::getVisibility () const
{
  return _visibility->getBoolValue();
}


/**
 * Set the panel's background texture.
 */
void
FGPanel::setBackground (ssgTexture * texture)
{
  _bg = texture;
}

/**
 * Set the panel's multiple background textures.
 */
void
FGPanel::setMultiBackground (ssgTexture * texture, int idx)
{
  _bg = 0;
  _mbg[idx] = texture;
}

/**
 * Set the panel's x-offset.
 */
void
FGPanel::setXOffset (int offset)
{
  if (offset <= 0 && offset >= -_width + WIN_W)
    _x_offset->setIntValue( offset );
}


/**
 * Set the panel's y-offset.
 */
void
FGPanel::setYOffset (int offset)
{
  if (offset <= 0 && offset >= -_height)
    _y_offset->setIntValue( offset );
}

/**
 * Handle a mouse action in panel-local (not screen) coordinates.
 * Used by the 3D panel code in Model/panelnode.cxx, in situations
 * where the panel doesn't control its own screen location.
 */
bool
FGPanel::doLocalMouseAction(int button, int updown, int x, int y)
{
  // Note a released button and return
  if (updown == 1) {
    if (_mouseInstrument != 0)
        _mouseInstrument->doMouseAction(_mouseButton, 1, _mouseX, _mouseY);
    _mouseDown = false;
    _mouseInstrument = 0;
    return false;
  }

  // Search for a matching instrument.
  for (int i = 0; i < (int)_instruments.size(); i++) {
    FGPanelInstrument *inst = _instruments[i];
    int ix = inst->getXPos();
    int iy = inst->getYPos();
    int iw = inst->getWidth() / 2;
    int ih = inst->getHeight() / 2;
    if (x >= ix - iw && x < ix + iw && y >= iy - ih && y < iy + ih) {
      _mouseDown = true;
      _mouseDelay = 20;
      _mouseInstrument = inst;
      _mouseButton = button;
      _mouseX = x - ix;
      _mouseY = y - iy;
      // Always do the action once.
      return _mouseInstrument->doMouseAction(_mouseButton, 0,
                                             _mouseX, _mouseY);
    }
  }
  return false;
}

/**
 * Perform a mouse action.
 */
bool
FGPanel::doMouseAction (int button, int updown, int x, int y)
{
				// FIXME: this same code appears in update()
  int xsize = _xsize_node->getIntValue();
  int ysize = _ysize_node->getIntValue();
  float aspect_adjust = get_aspect_adjust(xsize, ysize);

				// Scale for the real window size.
  if (aspect_adjust < 1.0) {
    x = int(((float)x / xsize) * WIN_W * aspect_adjust);
    y = int(WIN_H - ((float(y) / ysize) * WIN_H));
  } else {
    x = int(((float)x / xsize) * WIN_W);
    y = int((WIN_H - ((float(y) / ysize) * WIN_H)) / aspect_adjust);
  }

				// Adjust for offsets.
  x -= _x_offset->getIntValue();
  y -= _y_offset->getIntValue();

  // Having fixed up the coordinates, fall through to the local
  // coordinate handler.
  return doLocalMouseAction(button, updown, x, y);
} 

void FGPanel::setDepthTest (bool enable) {
    _enable_depth_test = enable;
}



////////////////////////////////////////////////////////////////////////.
// Implementation of FGPanelAction.
////////////////////////////////////////////////////////////////////////

FGPanelAction::FGPanelAction ()
{
}

FGPanelAction::FGPanelAction (int button, int x, int y, int w, int h,
                              bool repeatable)
    : _button(button), _x(x), _y(y), _w(w), _h(h), _repeatable(repeatable)
{
}

FGPanelAction::~FGPanelAction ()
{
  for (unsigned int i = 0; i < 2; i++) {
      for (unsigned int j = 0; j < _bindings[i].size(); j++)
          delete _bindings[i][j];
  }
}

void
FGPanelAction::addBinding (FGBinding * binding, int updown)
{
  _bindings[updown].push_back(binding);
}

bool
FGPanelAction::doAction (int updown)
{
  if (test()) {
    if ((updown != _last_state) || (updown == 0 && _repeatable)) {
        int nBindings = _bindings[updown].size();
        for (int i = 0; i < nBindings; i++)
            _bindings[updown][i]->fire();
    }
    _last_state = updown;
    return true;
  } else {
    return false;
  }
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGPanelTransformation.
////////////////////////////////////////////////////////////////////////

FGPanelTransformation::FGPanelTransformation ()
  : table(0)
{
}

FGPanelTransformation::~FGPanelTransformation ()
{
  delete table;
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGPanelInstrument.
////////////////////////////////////////////////////////////////////////


FGPanelInstrument::FGPanelInstrument ()
{
  setPosition(0, 0);
  setSize(0, 0);
}

FGPanelInstrument::FGPanelInstrument (int x, int y, int w, int h)
{
  setPosition(x, y);
  setSize(w, h);
}

FGPanelInstrument::~FGPanelInstrument ()
{
  for (action_list_type::iterator it = _actions.begin();
       it != _actions.end();
       it++) {
    delete *it;
    *it = 0;
  }
}

void
FGPanelInstrument::drawHotspots()
{
  for ( unsigned int i = 0; i < _actions.size(); i++ ) {
    FGPanelAction* a = _actions[i];
    float x1 = getXPos() + a->getX();
    float x2 = x1 + a->getWidth();
    float y1 = getYPos() + a->getY();
    float y2 = y1 + a->getHeight();

    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x1, y2);
    glVertex2f(x2, y2);
    glVertex2f(x2, y1);
    glEnd();
  }
}

void
FGPanelInstrument::setPosition (int x, int y)
{
  _x = x;
  _y = y;
}

void
FGPanelInstrument::setSize (int w, int h)
{
  _w = w;
  _h = h;
}

int
FGPanelInstrument::getXPos () const
{
  return _x;
}

int
FGPanelInstrument::getYPos () const
{
  return _y;
}

int
FGPanelInstrument::getWidth () const
{
  return _w;
}

int
FGPanelInstrument::getHeight () const
{
  return _h;
}

void
FGPanelInstrument::addAction (FGPanelAction * action)
{
  _actions.push_back(action);
}

				// Coordinates relative to centre.
bool
FGPanelInstrument::doMouseAction (int button, int updown, int x, int y)
{
  if (test()) {
    action_list_type::iterator it = _actions.begin();
    action_list_type::iterator last = _actions.end();
    for ( ; it != last; it++) {
      if ((*it)->inArea(button, x, y) &&
          (*it)->doAction(updown))
        return true;
    }
  }
  return false;
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGLayeredInstrument.
////////////////////////////////////////////////////////////////////////

FGLayeredInstrument::FGLayeredInstrument (int x, int y, int w, int h)
  : FGPanelInstrument(x, y, w, h)
{
}

FGLayeredInstrument::~FGLayeredInstrument ()
{
  for (layer_list::iterator it = _layers.begin(); it != _layers.end(); it++) {
    delete *it;
    *it = 0;
  }
}

void
FGLayeredInstrument::draw ()
{
  if (!test())
    return;
  
  for (int i = 0; i < (int)_layers.size(); i++) {
    glPushMatrix();
    _layers[i]->draw();
    glPopMatrix();
  }
}

int
FGLayeredInstrument::addLayer (FGInstrumentLayer *layer)
{
  int n = _layers.size();
  if (layer->getWidth() == -1) {
    layer->setWidth(getWidth());
  }
  if (layer->getHeight() == -1) {
    layer->setHeight(getHeight());
  }
  _layers.push_back(layer);
  return n;
}

int
FGLayeredInstrument::addLayer (const FGCroppedTexture &texture,
			       int w, int h)
{
  return addLayer(new FGTexturedLayer(texture, w, h));
}

void
FGLayeredInstrument::addTransformation (FGPanelTransformation * transformation)
{
  int layer = _layers.size() - 1;
  _layers[layer]->addTransformation(transformation);
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGSpecialInstrument.
////////////////////////////////////////////////////////////////////////

FGSpecialInstrument::FGSpecialInstrument (DCLGPS* sb)
  : FGPanelInstrument()
{
  complex = sb;
}

FGSpecialInstrument::~FGSpecialInstrument ()
{
}

void
FGSpecialInstrument::draw ()
{
  complex->draw();
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGInstrumentLayer.
////////////////////////////////////////////////////////////////////////

FGInstrumentLayer::FGInstrumentLayer (int w, int h)
  : _w(w),
    _h(h)
{
}

FGInstrumentLayer::~FGInstrumentLayer ()
{
  for (transformation_list::iterator it = _transformations.begin();
       it != _transformations.end();
       it++) {
    delete *it;
    *it = 0;
  }
}

void
FGInstrumentLayer::transform () const
{
  transformation_list::const_iterator it = _transformations.begin();
  transformation_list::const_iterator last = _transformations.end();
  while (it != last) {
    FGPanelTransformation *t = *it;
    if (t->test()) {
      float val = (t->node == 0 ? 0.0 : t->node->getFloatValue());

      if (t->has_mod)
          val = fmod(val, t->mod);
      if (val < t->min) {
	val = t->min;
      } else if (val > t->max) {
	val = t->max;
      }

      if (t->table==0) {
	val = val * t->factor + t->offset;
      } else {
	val = t->table->interpolate(val) * t->factor + t->offset;
      }
      
      switch (t->type) {
      case FGPanelTransformation::XSHIFT:
	glTranslatef(val, 0.0, 0.0);
	break;
      case FGPanelTransformation::YSHIFT:
	glTranslatef(0.0, val, 0.0);
	break;
      case FGPanelTransformation::ROTATION:
	glRotatef(-val, 0.0, 0.0, 1.0);
	break;
      }
    }
    it++;
  }
}

void
FGInstrumentLayer::addTransformation (FGPanelTransformation * transformation)
{
  _transformations.push_back(transformation);
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGGroupLayer.
////////////////////////////////////////////////////////////////////////

FGGroupLayer::FGGroupLayer ()
{
}

FGGroupLayer::~FGGroupLayer ()
{
  for (unsigned int i = 0; i < _layers.size(); i++)
    delete _layers[i];
}

void
FGGroupLayer::draw ()
{
  if (test()) {
    transform();
    int nLayers = _layers.size();
    for (int i = 0; i < nLayers; i++)
      _layers[i]->draw();
  }
}

void
FGGroupLayer::addLayer (FGInstrumentLayer * layer)
{
  _layers.push_back(layer);
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGTexturedLayer.
////////////////////////////////////////////////////////////////////////


FGTexturedLayer::FGTexturedLayer (const FGCroppedTexture &texture, int w, int h)
  : FGInstrumentLayer(w, h),
    _emissive(false)
{
  setTexture(texture);
}


FGTexturedLayer::~FGTexturedLayer ()
{
}


void
FGTexturedLayer::draw ()
{
  if (test()) {
    int w2 = _w / 2;
    int h2 = _h / 2;
    
    transform();
    glBindTexture(GL_TEXTURE_2D, _texture.getTexture()->getHandle());
    glBegin(GL_POLYGON);

    if (_emissive) {
      glColor4fv( emissive_panel_color );
    } else {
				// From Curt: turn on the panel
				// lights after sundown.
      glColor4fv( panel_color );
    }

    glTexCoord2f(_texture.getMinX(), _texture.getMinY()); glVertex2f(-w2, -h2);
    glTexCoord2f(_texture.getMaxX(), _texture.getMinY()); glVertex2f(w2, -h2);
    glTexCoord2f(_texture.getMaxX(), _texture.getMaxY()); glVertex2f(w2, h2);
    glTexCoord2f(_texture.getMinX(), _texture.getMaxY()); glVertex2f(-w2, h2);
    glEnd();
  }
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGTextLayer.
////////////////////////////////////////////////////////////////////////

FGTextLayer::FGTextLayer (int w, int h)
  : FGInstrumentLayer(w, h), _pointSize(14.0), _font_name("Helvetica.txf")
{
  _then.stamp();
  _color[0] = _color[1] = _color[2] = 0.0;
  _color[3] = 1.0;
}

FGTextLayer::~FGTextLayer ()
{
  chunk_list::iterator it = _chunks.begin();
  chunk_list::iterator last = _chunks.end();
  for ( ; it != last; it++) {
    delete *it;
  }
}

void
FGTextLayer::draw ()
{
  if (test()) {
    glColor4fv(_color);
    transform();

    FGFontCache *fc = globals->get_fontcache();
    text_renderer.setFont(fc->getTexFont(_font_name.c_str()));

    text_renderer.setPointSize(_pointSize);
    text_renderer.begin();
    text_renderer.start3f(0, 0, 0);

    _now.stamp();
    long diff = _now - _then;

    if (diff > 100000 || diff < 0 ) {
      // ( diff < 0 ) is a sanity check and indicates our time stamp
      // difference math probably overflowed.  We can handle a max
      // difference of 35.8 minutes since the returned value is in
      // usec.  So if the panel is left off longer than that we can
      // over flow the math with it is turned back on.  This (diff <
      // 0) catches that situation, get's us out of trouble, and
      // back on track.
      recalc_value();
      _then = _now;
    }

    // Something is goofy.  The code in this file renders only CCW
    // polygons, and I have verified that the font code in plib
    // renders only CCW trianbles.  Yet they come out backwards.
    // Something around here or in plib is either changing the winding
    // order or (more likely) pushing a left-handed matrix onto the
    // stack.  But I can't find it; get out the chainsaw...
    glFrontFace(GL_CW);
    text_renderer.puts((char *)(_value.c_str()));
    glFrontFace(GL_CCW);

    text_renderer.end();
    glColor4f(1.0, 1.0, 1.0, 1.0);	// FIXME
  }
}

void
FGTextLayer::addChunk (FGTextLayer::Chunk * chunk)
{
  _chunks.push_back(chunk);
}

void
FGTextLayer::setColor (float r, float g, float b)
{
  _color[0] = r;
  _color[1] = g;
  _color[2] = b;
  _color[3] = 1.0;
}

void
FGTextLayer::setPointSize (float size)
{
  _pointSize = size;
}

void
FGTextLayer::setFontName(const string &name)
{
  _font_name = name + ".txf";
}


void
FGTextLayer::setFont(fntFont * font)
{
  text_renderer.setFont(font);
}


void
FGTextLayer::recalc_value () const
{
  _value = "";
  chunk_list::const_iterator it = _chunks.begin();
  chunk_list::const_iterator last = _chunks.end();
  for ( ; it != last; it++) {
    _value += (*it)->getValue();
  }
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGTextLayer::Chunk.
////////////////////////////////////////////////////////////////////////

FGTextLayer::Chunk::Chunk (const string &text, const string &fmt)
  : _type(FGTextLayer::TEXT), _fmt(fmt)
{
  _text = text;
  if (_fmt.empty()) 
    _fmt = "%s";
}

FGTextLayer::Chunk::Chunk (ChunkType type, const SGPropertyNode * node,
			   const string &fmt, float mult, float offs,
                           bool truncation)
  : _type(type), _fmt(fmt), _mult(mult), _offs(offs), _trunc(truncation)
{
  if (_fmt.empty()) {
    if (type == TEXT_VALUE)
      _fmt = "%s";
    else
      _fmt = "%.2f";
  }
  _node = node;
}

const char *
FGTextLayer::Chunk::getValue () const
{
  if (test()) {
    _buf[0] = '\0';
    switch (_type) {
    case TEXT:
      sprintf(_buf, _fmt.c_str(), _text.c_str());
      return _buf;
    case TEXT_VALUE:
      sprintf(_buf, _fmt.c_str(), _node->getStringValue());
      break;
    case DOUBLE_VALUE:
      double d = _offs + _node->getFloatValue() * _mult;
      if (_trunc)  d = (d < 0) ? -floor(-d) : floor(d);
      sprintf(_buf, _fmt.c_str(), d);
      break;
    }
    return _buf;
  } else {
    return "";
  }
}



////////////////////////////////////////////////////////////////////////
// Implementation of FGSwitchLayer.
////////////////////////////////////////////////////////////////////////

FGSwitchLayer::FGSwitchLayer ()
  : FGGroupLayer()
{
}

void
FGSwitchLayer::draw ()
{
  if (test()) {
    transform();
    int nLayers = _layers.size();
    for (int i = 0; i < nLayers; i++) {
      if (_layers[i]->test()) {
          _layers[i]->draw();
          return;
      }
    }
  }
}


// end of panel.cxx
