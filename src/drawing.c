
#include <math.h>
#include "drawing.h"
#include "main.h"

guint min (guint x, guint y)
{
  return (x > y) ? y : x;
}

void draw_line (GromitData *data,
		GdkDevice *dev,
		gint x1, gint y1,
		gint x2, gint y2)
{
  GdkRectangle rect;
  GromitDeviceData *devdata = g_hash_table_lookup(data->devdatatable, dev);

  rect.x = MIN (x1,x2) - data->maxwidth / 2;
  rect.y = MIN (y1,y2) - data->maxwidth / 2;
  rect.width = ABS (x1-x2) + data->maxwidth;
  rect.height = ABS (y1-y2) + data->maxwidth;

  if(data->debug)
    g_printerr("DEBUG: draw line from %d %d to %d %d\n", x1, y1, x2, y2);

  if (devdata->cur_context->paint_ctx)
    {
      cairo_set_line_width(devdata->cur_context->paint_ctx, data->maxwidth);
      cairo_set_line_cap(devdata->cur_context->paint_ctx, CAIRO_LINE_CAP_ROUND);
      cairo_set_line_join(devdata->cur_context->paint_ctx, CAIRO_LINE_JOIN_ROUND);
 
      cairo_move_to(devdata->cur_context->paint_ctx, x1, y1);
      cairo_line_to(devdata->cur_context->paint_ctx, x2, y2);
      cairo_stroke(devdata->cur_context->paint_ctx);

      data->modified = 1;

      gdk_window_invalidate_rect(gtk_widget_get_window(data->win), &rect, 0); 
    }

  data->painted = 1;
}


void draw_arrow (GromitData *data, 
		 GdkDevice *dev,
		 gint x1, gint y1,
		 gfloat width,
		 gfloat direction)
{
  GdkRectangle rect;
  GdkPoint arrowhead [4];

  /* get the data for this device */
  GromitDeviceData *devdata = g_hash_table_lookup(data->devdatatable, dev);

  width = width / 2;

  /* I doubt that calculating the boundary box more exact is very useful */
  rect.x = x1 - 4 * width - 1;
  rect.y = y1 - 4 * width - 1;
  rect.width = 8 * width + 2;
  rect.height = 8 * width + 2;

  arrowhead [0].x = x1 + 4 * width * cos (direction);
  arrowhead [0].y = y1 + 4 * width * sin (direction);

  arrowhead [1].x = x1 - 3 * width * cos (direction)
                       + 3 * width * sin (direction);
  arrowhead [1].y = y1 - 3 * width * cos (direction)
                       - 3 * width * sin (direction);

  arrowhead [2].x = x1 - 2 * width * cos (direction);
  arrowhead [2].y = y1 - 2 * width * sin (direction);

  arrowhead [3].x = x1 - 3 * width * cos (direction)
                       - 3 * width * sin (direction);
  arrowhead [3].y = y1 + 3 * width * cos (direction)
                       - 3 * width * sin (direction);

  if (devdata->cur_context->paint_ctx)
    {
      cairo_set_line_width(devdata->cur_context->paint_ctx, 1);
      cairo_set_line_cap(devdata->cur_context->paint_ctx, CAIRO_LINE_CAP_ROUND);
      cairo_set_line_join(devdata->cur_context->paint_ctx, CAIRO_LINE_JOIN_ROUND);
 
      cairo_move_to(devdata->cur_context->paint_ctx, arrowhead[0].x, arrowhead[0].y);
      cairo_line_to(devdata->cur_context->paint_ctx, arrowhead[1].x, arrowhead[1].y);
      cairo_line_to(devdata->cur_context->paint_ctx, arrowhead[2].x, arrowhead[2].y);
      cairo_line_to(devdata->cur_context->paint_ctx, arrowhead[3].x, arrowhead[3].y);
      cairo_fill(devdata->cur_context->paint_ctx);

      gdk_cairo_set_source_rgba(devdata->cur_context->paint_ctx, data->black);

      cairo_move_to(devdata->cur_context->paint_ctx, arrowhead[0].x, arrowhead[0].y);
      cairo_line_to(devdata->cur_context->paint_ctx, arrowhead[1].x, arrowhead[1].y);
      cairo_line_to(devdata->cur_context->paint_ctx, arrowhead[2].x, arrowhead[2].y);
      cairo_line_to(devdata->cur_context->paint_ctx, arrowhead[3].x, arrowhead[3].y);
      cairo_line_to(devdata->cur_context->paint_ctx, arrowhead[0].x, arrowhead[0].y);
      cairo_stroke(devdata->cur_context->paint_ctx);

      gdk_cairo_set_source_rgba(devdata->cur_context->paint_ctx, devdata->cur_context->paint_color);
    
      data->modified = 1;

      gdk_window_invalidate_rect(gtk_widget_get_window(data->win), &rect, 0); 
    }

  data->painted = 1;
}

void draw_frame (GromitData *data,
    GdkDevice *dev,
    guint x, guint y,
    guint xlength, guint ylength,
    guint radius, guint strokewidth,
    GdkRGBA *fill_color)
{
  GdkRectangle rect;
  GromitDeviceData *devdata = g_hash_table_lookup(data->devdatatable, dev);

  x = x - xlength / 2 + 1;
  y = y - ylength / 2 + 1;

  rect.x = x - strokewidth;
  rect.y = y - strokewidth;
  rect.width = xlength + strokewidth * 2;
  rect.height = ylength + strokewidth * 2;

  if (radius > min(xlength, ylength) / 2)
    radius = min(xlength, ylength) / 2;

  if(data->debug)
    g_printerr("DEBUG: draw frame with center %d, %d, width %d, height %d, corner radius %d and fill color %s\n", x, y, xlength, ylength, radius, gdk_rgba_to_string(fill_color));

  if (devdata->cur_context->paint_ctx)
    {
      double degrees = M_PI / 180.0;
      cairo_new_sub_path(devdata->cur_context->paint_ctx);
      cairo_arc(devdata->cur_context->paint_ctx, x + xlength - radius, y + radius, radius, -90 * degrees, 0 * degrees);
      cairo_arc(devdata->cur_context->paint_ctx, x + xlength - radius, y + ylength - radius, radius, 0 * degrees, 90 * degrees);
      cairo_arc(devdata->cur_context->paint_ctx, x + radius, y + ylength - radius, radius, 90 * degrees, 180 * degrees);
      cairo_arc(devdata->cur_context->paint_ctx, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
      cairo_close_path(devdata->cur_context->paint_ctx);
      if (fill_color)
        {
          gdk_cairo_set_source_rgba(devdata->cur_context->paint_ctx, devdata->cur_context->fill_color);
          cairo_fill_preserve(devdata->cur_context->paint_ctx);
          gdk_cairo_set_source_rgba(devdata->cur_context->paint_ctx, devdata->cur_context->paint_color);
        }
      cairo_stroke(devdata->cur_context->paint_ctx);

      data->modified = 1;

      gdk_window_invalidate_rect(gtk_widget_get_window(data->win), &rect, 0);
    }

  data->painted = 1;
}

void draw_counter (GromitData *data,
    GdkDevice *dev,
    guint x, guint y,
    guint xlength, guint ylength,
    guint radius, guint strokewidth,
    GdkRGBA *fill_color,
    gint count,
    guint font_size)
{
  draw_frame(data, dev, x, y, xlength, ylength, radius, strokewidth, fill_color);

  GdkRectangle rect;
  GromitDeviceData *devdata = g_hash_table_lookup(data->devdatatable, dev);

  rect.x = x - xlength / 2 - strokewidth;
  rect.y = y - ylength / 2 - strokewidth;
  rect.width = xlength + strokewidth * 2;
  rect.height = ylength + strokewidth * 2;

  if (devdata->cur_context->paint_ctx)
    {
      char countstr[4];
      guint fontsize;

      sprintf(countstr, "%d", count);

      /*
       * just prevent mis-configurations
       * auto-fit should only be necessary for 3+ digits
      */
      if (devdata->cur_context->font_size > ylength * 0.85)
        fontsize = ylength * 0.85;
      else
        fontsize = devdata->cur_context->font_size;

      if(data->debug)
        g_printerr("DEBUG: draw counter with center %d, %d, width %d, height %d, corner radius %d, fill color %s, count %u and font_size %d\n", x, y, xlength, ylength, radius, gdk_rgba_to_string(fill_color), count, fontsize);

      cairo_text_extents_t te;
      gdk_cairo_set_source_rgba(devdata->cur_context->paint_ctx, devdata->cur_context->paint_color);
      cairo_select_font_face (devdata->cur_context->paint_ctx, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
      cairo_set_font_size (devdata->cur_context->paint_ctx, fontsize);
      cairo_text_extents (devdata->cur_context->paint_ctx, countstr, &te);
      cairo_move_to (devdata->cur_context->paint_ctx, x + 1 - te.width / 2 - te.x_bearing, y - te.height / 2 - te.y_bearing);
      cairo_show_text (devdata->cur_context->paint_ctx, countstr);

      data->modified = 1;

      gdk_window_invalidate_rect(gtk_widget_get_window(data->win), &rect, 0);
    }

  data->painted = 1;
}
