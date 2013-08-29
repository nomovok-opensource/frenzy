/* 
 * Copyright 2013 by Nomovok Ltd.
 * 
 * Contact: info@nomovok.com
 * 
 * This Source Code Form is subject to the
 * terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with
 * this file, You can obtain one at
 * http://mozilla.org/MPL/2.0/.
 * 
 */

#include "WRATHConfig.hpp"
#include "WRATHLayer.hpp"
#include "WRATHLayerItemWidgetsTranslate.hpp"

#include <iostream>
#include <fstream>

#include <SDL.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "dom/graphics.hpp"
#include "dom/document.hpp"
#include "parser/htmlparser.hpp"

WRATHTripleBufferEnabler::handle tr;
WRATHLayer* contents;
typedef WRATHLayerTranslateFamilySet::PlainFamily Family;

using namespace frenzy;
using namespace frenzy::dom;
using namespace frenzy::graphics;
using namespace frenzy::parser;

struct wrath_translation : translation
{
  wrath_translation()
    : widget(WRATHNew Family::NodeWidget(contents))
  {
  }

  virtual void position(frenzy::vec2 pos)
  {
    widget->translation(::vec2(pos.x.n, pos.y.n));
  }

  virtual frenzy::vec2 position()
  {
    ::vec2 wrathvec = widget->translation();
    return frenzy::vec2(wrathvec.x(), wrathvec.y());
  }

  virtual void relative_to(boost::shared_ptr<translation> t)
  {
    boost::shared_ptr<wrath_translation> wt = boost::static_pointer_cast<wrath_translation>(t);
    parent = t;
    if (!t)
      widget->parent(NULL);
    else
      widget->parent(wt->widget);
  }

  boost::shared_ptr<translation> relative_to()
  {
    return parent.lock();
  }

  boost::weak_ptr<translation> parent;
  Family::NodeWidget* widget;
};

struct wrath_text : text
{
  wrath_text(boost::shared_ptr<translation> position)
    : widget(WRATHNew Family::TextWidget(boost::static_pointer_cast<wrath_translation>(position)->widget,
					 WRATHTextItemTypes::text_transparent))
  {
  }

  virtual void data(ustring str)
  {
    WRATHTextDataStream ostr;

    ostr.stream() << WRATHText::set_color(0, 0, 0, 0xFF);
    ostr.append(str.begin(), str.end());

    widget->properties()->clear();
    widget->properties()->add_text(ostr);

    WRATHFormatter::pen_position_return_type penpos = ostr.end_text_pen_position();
    ::vec2 exact = penpos.m_exact_pen_position;
    size = frenzy::vec2(exact.x(), exact.y());
  }

  virtual frenzy::vec2 autowrap(frenzy::vec width)
  {
    (void)width;
    return size;
  }

  Family::TextWidget* widget;
  frenzy::vec2 size;
};

struct wrath_factory : factory
{
  virtual frenzy::vec page_width()
  {
    return 800;
  }

  virtual boost::shared_ptr<translation> create_translation()
  {
    return boost::shared_ptr<translation>(new wrath_translation());
  }

  virtual boost::shared_ptr<text> create_text(boost::shared_ptr<translation> position)
  {
    return boost::shared_ptr<text>(new wrath_text(position));
  }
};

void init_wrath()
{
  tr = WRATHNew WRATHTripleBufferEnabler();
  contents = WRATHNew WRATHLayer(tr);

  float_orthogonal_projection_params projection(0, 800, 600, 0);
  contents->simulation_matrix(WRATHLayer::projection_matrix, float4x4(projection));

}

void draw_wrath()
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  tr->signal_complete_simulation_frame();
  tr->signal_begin_presentation_frame();
  contents->clear_and_draw();
  SDL_GL_SwapBuffers();
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " filename\n";
    return 1;
  }

  std::ifstream input(argv[1]);
  if (!input || !input.is_open())
  {
    std::cerr << "Failure opening " << argv[1] << "\n";
    return 1;
  }

  std::cout << "Loading " << argv[1] << "...\n";
  Documentp doc = Document::create();

  htmlparser parser(doc);

  bytestring inputstring((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

  parser.pass_bytes(inputstring);
  parser.pass_bytes(bytestring());

  if (!parser.stopped())
  {
    std::cerr << "HTML parser jammed\n";
    return 1;
  }

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 1);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

  SDL_Surface* window = SDL_SetVideoMode(800, 600, 0, SDL_OPENGL | SDL_RESIZABLE);

  if (!window)
  {
    std::cerr << "Window creation failed\n";
    return 1;
  }

  SDL_WM_SetCaption("Frenzy", NULL);

  {
    int val;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &val);
    std::cout << "Doublebuffering: " << val << "\n";
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &val);
    std::cout << "Depth: " << val << "\n";
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &val);
    std::cout << "Stencil: " << val << "\n";
  }

  init_wrath();

  boost::shared_ptr<factory> fact(new wrath_factory());

  doc->graphicsfactory(fact);

  bool done = false;
  while (!done)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_QUIT:
	done = true;
	break;
      case SDL_KEYDOWN:
	if (event.key.keysym.sym == SDLK_ESCAPE)
	  done = true;
	break;
      default:
	break;
      }
    }

    draw_wrath();
    SDL_Delay(0);
  }

  SDL_Quit();
}
