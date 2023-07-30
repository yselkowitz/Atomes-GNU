/* This file is part of Atomes.

Atomes is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Atomes is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with Atomes.
If not, see <https://www.gnu.org/licenses/> */

/*
* This file: 'update_p.c'
*
*  Contains:
*

 -

*
*  List of subroutines:

  int update_project ();

  void prep_calc_actions ();
  void active_project_changed (int id);
  void opengl_project_changed (int id);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "workspace.h"

extern GtkWidget * work_menu (int p, int c);
extern GtkTreeStore * tool_model;

/*
*  void prep_calc_actions ()
*
*  Usage:
*/
void prep_calc_actions ()
{
  int i;
  // Depends on the number of calculations available
  for (i=0; i<NCALCS-3; i++)
  {
    if (i < AN)
    {
      if (active_project -> runok[i])
      {
        add_action (analyze_actions[i]);
      }
      else
      {
        remove_action (analyze_action_names[i]);
      }
    }
    else
    {
      if (active_project -> runok[i+1])
      {
        add_action (analyze_actions[i]);
      }
      else
      {
        remove_action (analyze_action_names[i]);
      }
    }
  }
}

/*
*  int update_project ()
*
*  Usage: update project: send data to Fortran90, and update calculation interactors
*/
int update_project ()
{
#ifdef DEBUG
  g_debug ("UPDATE_PROJECT: to update");
#endif
  int i, j;
  if (! active_project -> newproj && active_project -> natomes)
  {
    i = alloc_data_ (& active_project -> natomes,
                     & active_project -> nspec,
                     & active_project -> steps);
    if (i == 1)
    {
      to_read_pos ();
      int * lot = allocint (active_project -> natomes);
      for (j=0; j<active_project -> natomes; j++) lot[j] = active_project -> atoms[0][j].sp;
      read_data_ (lot, active_chem -> nsps);
      g_free (lot);
      read_chem_ (active_chem -> chem_prop[CHEM_M], active_chem -> chem_prop[CHEM_R],
                  active_chem -> chem_prop[CHEM_N], active_chem -> chem_prop[CHEM_X]);
      prep_spec_ (active_chem -> chem_prop[CHEM_Z], active_chem -> nsps);
    }
    else
    {
      return 0;
    }
    if (active_project -> run)
    {
      active_project -> dmtx = FALSE;
      j = (active_cell -> npt) ? active_project -> steps : 1;
      for (i=0; i<j; i++)
      {
        lattice_ (& j, & i,
                  active_cell -> box[i].vect,
                  active_cell -> box[i].param[0],
                  active_cell -> box[i].param[1],
                  & active_cell -> ltype,
                  & active_cell -> frac,
                  & active_cell -> pbc);

      }
      cutoffsend ();
    }
  }
  if (active_project -> numwid > 0)
  {
    if (active_cell -> has_a_box)
    {
      active_project -> runok[GR] = TRUE;
      active_project -> runok[SK] = TRUE;
    }
    else
    {
      active_project -> runok[GR] = FALSE;
      active_project -> runok[SQ] = FALSE;
      active_project -> runok[SK] = FALSE;
      active_project -> runok[GK] = FALSE;
    }
    if (active_project -> natomes)
    {
      active_project -> runok[BD] = TRUE;
      active_project -> runok[RI] = TRUE;
      active_project -> runok[CH] = TRUE;
      active_project -> runok[SP] = TRUE;
      if (active_project -> steps > 1) active_project -> runok[MS] = TRUE;
    }
  }
#ifdef DEBUG
  g_debug ("UPDATE_PROJECT: updated");
#endif
  return 1;
}

/*
*  void active_project_changed (int id)
*
*  Usage: change the active project
*
*  int id : the id of the new active project
*/
void active_project_changed (int id)
{
  char * errp = NULL;
  if (id != inactep && inactep < nprojects && ! atomes_logo) clean_view ();
  gtk_tree_store_clear (tool_model);
  activep = id;
  active_project = get_project_by_id (id);
  active_chem = active_project -> chemistry;
  active_coord = active_project -> coord;
  active_cell = & active_project -> cell;
  active_box = NULL;
  active_glwin = NULL;
  active_image = NULL;
  if (active_project -> modelgl != NULL)
  {
    active_glwin = active_project -> modelgl;
    if (active_glwin -> anim != NULL)
    {
      active_image = active_glwin -> anim -> last -> img;
    }
  }

  if (active_cell -> box)
  {
    if (active_project -> steps == 1 || ! active_cell -> npt || ! active_image)
    {
      active_box = & active_cell -> box[0];
    }
    else if (active_cell -> npt)
    {
      active_box = & active_cell -> box[active_image -> step];
    }
  }
  if (update_project() == 0)
  {
    errp = g_strdup_printf ("Impossible to update project: %s\n", active_project -> name);
    show_error (errp, 0, MainWindow);
    g_free (errp);
  }
  else
  {
    if (active_project -> numwid > 0)
    {
      prep_calc_actions ();
      add_action (edition_actions[0]);
      if (active_cell -> npt)
      {
        remove_action (edition_action_names[1]);
      }
      else
      {
        add_action (edition_actions[1]);
      }
      add_action (edition_actions[2]);
      fill_tool_model ();
      correct_this_window_title (curvetoolbox, g_strdup_printf ("Toolboxes - %s", prepare_for_title(active_project -> name)));
      correct_this_window_title (MainWindow, g_strdup_printf ("%s - %s", PACKAGE, prepare_for_title (active_project -> name)));
    }
    inactep = activep;
  }
  activew = activep;
}

/*
*  void opengl_project_changed (int id)
*
*  Usage: change the OpenGL project
*
*  int id : the id of the new OpenGL project
*/
void opengl_project_changed (int id)
{
  activev = id;
  opengl_project = get_project_by_id(id);
}
