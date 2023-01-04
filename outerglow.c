/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2023 Beaver (GEGL outerglow)
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES




property_color (color, _("Color"), "#00ff15")

property_double (radius, _("Lens Blur to Glow"), 11.0)
  description (_("Blur radius"))
  value_range (0.0, G_MAXDOUBLE)
  ui_range    (0.0, 50.0)
  ui_gamma    (2.0)
  ui_meta     ("unit", "pixel-distance")


property_double (gradius, _("Gaussian Blur to Glow"), 0.0)
  description (_("Blur radius"))
  value_range (0.0, G_MAXDOUBLE)
  ui_range    (0.0, 50.0)
  ui_gamma    (2.0)
  ui_meta     ("unit", "pixel-distance")

property_double (opacity, _("Make Opacity over 100%"), 1)
  value_range   (1, 3.0)
  ui_steps      (1, 3.0)


property_double (tile_size, _("Aura Size"), 3.8)
    description (_("Average diameter of each tile (in pixels)"))
    value_range (3.0, 16.0)
    ui_meta     ("unit", "pixel-distance")

property_double (tile_saturation, _("Gaps in Aura"), 5.4)
    description (_("Expand tiles by this amount"))
    value_range (3.0, 8.0)


property_int    (mask_radius, _("Internal Oilify filter for Aura"), 4)
    description (_("Radius of circle around pixel, can also be scaled per pixel by a buffer on the aux pad."))
    value_range (1, 7)
    ui_range (1, 7)
    ui_meta     ("unit", "pixel-distance")

property_seed (seed, _("Random seed for Aura"), rand)




#else

#define GEGL_OP_META
#define GEGL_OP_NAME     outerglow
#define GEGL_OP_C_SOURCE outerglow.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *color, *oilify, *cubism, *xor, *gblur, *lblur, *nop, *opacity;


  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");


  color   = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

  opacity   = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity",
                                  NULL);


  xor   = gegl_node_new_child (gegl,
                                  "operation", "gegl:xor",
                                  NULL);

  oilify   = gegl_node_new_child (gegl,
                                  "operation", "gegl:oilify",
                                  NULL);

  cubism   = gegl_node_new_child (gegl,
                                  "operation", "gegl:cubism",
                                  NULL);

  lblur   = gegl_node_new_child (gegl,
                                  "operation", "gegl:lens-blur",
                                  NULL);

  nop   = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

  gblur    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur",
                                  NULL);




  gegl_operation_meta_redirect (operation, "color", color, "value");
  gegl_operation_meta_redirect (operation, "radius", lblur, "radius");
  gegl_operation_meta_redirect (operation, "opacity", opacity, "value");
  gegl_operation_meta_redirect (operation, "tile_size", cubism, "tile-size");
  gegl_operation_meta_redirect (operation, "tile_saturation", cubism, "tile-saturation");
  gegl_operation_meta_redirect (operation, "mask_radius", oilify, "mask-radius");
  gegl_operation_meta_redirect (operation, "seed", cubism, "seed");
  gegl_operation_meta_redirect (operation, "gradius", gblur, "std-dev-x");
  gegl_operation_meta_redirect (operation, "gradius", gblur, "std-dev-y");




  gegl_node_link_many (input, nop, cubism, oilify, lblur, gblur, xor, color, opacity, output, NULL);
  gegl_node_connect_from (xor, "aux", nop, "output");





}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:aura",
    "title",       _("Aura"),
    "categories",  "Artistic",
    "reference-hash", "3oudo6vg25ara040vxn3vv5sb2a",
    "description", _("GEGL does both an Outer Glow and Inner Glow aura-ish effect. Apply filter on duplicate layer above transparent subject/object. Filter is intended to be used with Gimp blend modes HSV Hue, HSL Color and Grain Merge. Filter benefits from the Union Composite Mode when used in transparent backgrounds."
                     ""),
    NULL);
}

#endif
