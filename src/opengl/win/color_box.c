/* This file is part of Atomes.

Atomes is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Atomes is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with Atomes.
If not, see <https://www.gnu.org/licenses/> */

#include "global.h"
#include "interface.h"
#include "project.h"
#include "glwindow.h"
#include "color_box.h"
#include "glview.h"

void get_color (ColRGBA * but, int cid)
{
  int rid = cid / 4;
  int bid = cid - rid*4;
  but -> red = (rid < 6) ? 0.0 : (rid < 11) ? 0.2 * (rid - 5) : 1.0;
  but -> green = (rid < 6) ? 0.2*rid : (rid < 11) ? 1.0 - 0.2 * (rid - 5) :  0.2 * (rid - 10);
  but -> blue = bid * 0.333;
  if (bid == 3) but -> blue = 1.0;
}

cairo_surface_t * col_surface (double r, double g, double b, int x, int y)
{
  cairo_surface_t * cst;
  cairo_t * tcst;
  cst = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y);
  tcst = cairo_create (cst);
  cairo_set_source_rgb (tcst, r, g, b);
  cairo_paint (tcst);
  cairo_destroy (tcst);
  return cst;
}

G_MODULE_EXPORT void set_back_color (GtkWidget * widg, gpointer data)
{
  tint * col = (tint *) data;
  struct project * this_proj = get_project_by_id(col -> a);
  get_color (& this_proj -> modelgl -> anim -> last -> img -> backcolor, col -> b);
  this_proj -> modelgl -> create_shaders[MEASU] = TRUE;
  update (this_proj -> modelgl);
}

G_MODULE_EXPORT void set_box_color (GtkWidget * widg, gpointer data)
{
  tint * col = (tint *) data;
  struct project * this_proj = get_project_by_id(col -> a);
  get_color (& this_proj -> modelgl -> anim -> last -> img -> box_color, col -> b);
  this_proj -> modelgl -> create_shaders[MDBOX] = TRUE;
  update (this_proj -> modelgl);
}

G_MODULE_EXPORT void set_at_color (GtkWidget * widg, gpointer data)
{
  tint * col = (tint *) data;
  struct project * this_proj = get_project_by_id(col -> a);
  get_color (& this_proj -> modelgl -> anim -> last -> img -> at_color[col -> c], col -> b);
  int shaders[2] = {ATOMS, BONDS};
  re_create_md_shaders (2, shaders, this_proj);
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  if (this_proj -> modelgl -> anim -> last -> img -> color_map[1] == 0) this_proj -> modelgl -> create_shaders[POLYS] = TRUE;
  update (this_proj -> modelgl);
}

G_MODULE_EXPORT void set_rings_color (GtkWidget * widg, gpointer data)
{
  qint * col = (qint *)data;
  struct project * this_proj = get_project_by_id(col -> a);
  get_color (& this_proj -> modelgl -> anim -> last -> img -> spcolor[4+col -> b][0][col -> c], col -> d);
  int shaders[1] = {RINGS};
  re_create_md_shaders (1, shaders, this_proj);
  update (this_proj -> modelgl);
}

G_MODULE_EXPORT void set_total_coord_color (GtkWidget * widg, gpointer data)
{
  qint * col = (qint *)data;
  struct project * this_proj = get_project_by_id(col -> a);
  get_color (& this_proj -> modelgl -> anim -> last -> img -> spcolor[0][col -> b][col -> c], col -> d);
  int shaders[2] = {ATOMS, BONDS};
  re_create_md_shaders (2, shaders, this_proj);
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  if (this_proj -> modelgl -> anim -> last -> img -> color_map[1] == 1) this_proj -> modelgl -> create_shaders[POLYS] = TRUE;
  update (this_proj -> modelgl);
}

G_MODULE_EXPORT void set_partial_coord_color (GtkWidget * widg, gpointer data)
{
  qint * col = (qint *)data;
  struct project * this_proj = get_project_by_id(col -> a);
  get_color (& this_proj -> modelgl -> anim -> last -> img -> spcolor[1][col -> b][col -> c], col -> d);
  int shaders[2] = {ATOMS, BONDS};
  re_create_md_shaders (2, shaders, this_proj);
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  if (this_proj -> modelgl -> anim -> last -> img -> color_map[1] == 2) this_proj -> modelgl -> create_shaders[POLYS] = TRUE;
  update (this_proj -> modelgl);
}

G_MODULE_EXPORT void set_frag_mol_color (GtkWidget * widg, gpointer data)
{
  qint * col = (qint *)data;
  struct project * this_proj = get_project_by_id(col -> a);
  get_color (& this_proj -> modelgl -> anim -> last -> img -> spcolor[col -> b][0][col -> c], col -> d);
  int shaders[2] = {ATOMS, BONDS};
  re_create_md_shaders (2, shaders, this_proj);
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  if (this_proj -> modelgl -> anim -> last -> img -> color_map[1] == col -> b + 1) this_proj -> modelgl -> create_shaders[POLYS] = TRUE;
  update (this_proj -> modelgl);
}

#ifdef GTK4
void color_box (glwin * view, int ideo, int spec, int geo)
#else
GtkWidget * color_box (glwin * view, int ideo, int spec, int geo)
#endif
{
  int l, m, n, p;
  p = view -> proj;
#ifdef GTK3
  GtkWidget * but;
  GtkWidget * coltable;
// #ifdef GTK3
  coltable = gtk_menu_new ();
// #else
//  coltable = gtk_grid_new ();
// #endif
  ColRGBA but_col;
#endif
  struct project * this_proj = get_project_by_id(p);
  l = 0;
  for (l=0; l<64; l++)
  {
#ifdef GTK3
    get_color (& but_col, l);
    cairo_surface_t * surface = col_surface (but_col.red, but_col.green, but_col.blue, 12, 12);
// #ifdef GTK3
    but = gtk3_menu_item (NULL, NULL, IMG_SURFACE, (gpointer)surface, NULL, NULL, FALSE, 0, 0, FALSE, FALSE, FALSE);
/* #else
    GtkWidget * but_img =  create_image_from_data (IMG_SURFACE, surface);
    gtk_widget_show (but_img);
    but = gtk_button_new ();
    add_container_child (CONTAINER_BUT, but, but_img);
    gtk_button_set_has_frame ((GtkButton *)but, FALSE);
    gtk_widget_show (but);
#endif */
    cairo_surface_destroy (surface);
#endif
    if (ideo < -2)
    {
      view -> gcid[4+spec][geo][l].a = p;
      view -> gcid[4+spec][geo][l].b = spec;
      view -> gcid[4+spec][geo][l].c = geo;
      view -> gcid[4+spec][geo][l].d = l;
#ifdef GTK3
      g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(set_rings_color), & view -> gcid[4+spec][geo][l]);
#endif
    }
    else if (ideo == -2)
    {
#ifdef GTK3
      g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(set_back_color), & view -> colorp[l][0]);
#endif
    }
    else if (ideo == -1)
    {
#ifdef GTK3
      g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(set_box_color), & view -> colorp[l][0]);
#endif
    }
    else if (ideo < this_proj -> nspec*2)
    {
#ifdef GTK3
      g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(set_at_color), & view -> colorp[l][ideo]);
#endif
    }
    else if (ideo < 2*this_proj -> nspec + this_proj -> coord -> totcoord[0])
    {
      n = ideo - 2*this_proj -> nspec;
      view -> gcid[0][n][l].a = p;
      view -> gcid[0][n][l].b = spec;
      view -> gcid[0][n][l].c = geo;
      view -> gcid[0][n][l].d = l;
#ifdef GTK3
      g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(set_total_coord_color), & view -> gcid[0][n][l]);
#endif
    }
    else if (ideo < 2*this_proj -> nspec + this_proj -> coord -> totcoord[0] + this_proj -> coord -> totcoord[1])
    {
      n = ideo - 2*this_proj -> nspec - this_proj -> coord -> totcoord[0];
      view -> gcid[1][n][l].a = p;
      view -> gcid[1][n][l].b = spec;
      view -> gcid[1][n][l].c = geo;
      view -> gcid[1][n][l].d = l;
#ifdef GTK3
      g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(set_partial_coord_color), & view -> gcid[1][n][l]);
#endif
    }
    else
    {
      n = ideo - 2*this_proj -> nspec;
      for (m= 0; m<spec; m++)
      {
        n -= this_proj -> coord -> totcoord[m];
      }
      view -> gcid[spec][n][l].a = p;
      view -> gcid[spec][n][l].b = spec;
      view -> gcid[spec][n][l].c = geo;
      view -> gcid[spec][n][l].d = l;
#ifdef GTK3
      g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(set_frag_mol_color), & view -> gcid[spec][n][l]);
#endif
    }
    m = l/4;
#ifdef GTK3
    gtk_menu_attach (GTK_MENU(coltable), but, l-m*4, l+1-m*4, m, m+1);
// #else
    // gtk_grid_attach (GTK_GRID(coltable), but, l-m*4, m, 1, 1);
#endif
  }
#ifdef GTK3
  but = create_menu_item (FALSE, "More colors ...");
  add_menu_child (coltable, but);
// #endif
  if (ideo < -2)
  {
    g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(window_color_coord), & view -> gcid[4+spec][geo][4+spec]);
  }
  else if (ideo == -2)
  {
    g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(to_run_back_color_window), view);
  }
  else if (ideo == -1)
  {
    g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(to_run_box_color_window), view);
  }
  else if (ideo < this_proj -> nspec*2)
  {
    g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(to_run_atom_color_window), & view -> colorp[0][ideo]);
  }
  else if (ideo < this_proj -> nspec*2 + this_proj -> coord -> totcoord[0])
  {
    g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(window_color_coord), & view -> gcid[0][n][0]);
  }
  else if (ideo < 2*this_proj -> nspec + this_proj -> coord -> totcoord[0] + this_proj -> coord -> totcoord[1])
  {
    g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(window_color_coord), & view -> gcid[1][n][1]);
  }
  else
  {
    g_signal_connect (G_OBJECT (but), "activate", G_CALLBACK(window_color_coord), & view -> gcid[spec][n][spec]);
  }
  show_the_widgets (coltable);
  return coltable;
#endif
}
