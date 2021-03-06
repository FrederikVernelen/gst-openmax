/*
 * Copyright (C) 2007-2008 Nokia Corporation.
 *
 * Author: Felipe Contreras <felipe.contreras@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <gst/check/gstcheck.h>

#define BUFFER_SIZE 0x1000
#define BUFFER_COUNT 0x100
#define FLUSH_AT 0x10

static gboolean
bus_cb (GstBus *bus,
        GstMessage *msg,
        gpointer data)
{
    switch (GST_MESSAGE_TYPE (msg))
    {
        case GST_MESSAGE_EOS:
            /* g_debug ("end-of-stream"); */
            break;
        case GST_MESSAGE_ERROR:
            {
                gchar *debug;
                GError *err;

                gst_message_parse_error (msg, &err, &debug);
                g_free (debug);

                g_warning ("Error: %s", err->message);
                g_error_free (err);
                break;
            }
        default:
            /* g_debug ("message-type: %s", GST_MESSAGE_TYPE_NAME (msg)); */
            break;
    }

    return TRUE;
}

static GstStaticPadTemplate sinktemplate =
GST_STATIC_PAD_TEMPLATE ("sink",
                         GST_PAD_SINK,
                         GST_PAD_ALWAYS,
                         GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate srctemplate =
GST_STATIC_PAD_TEMPLATE ("src",
                         GST_PAD_SRC,
                         GST_PAD_ALWAYS,
                         GST_STATIC_CAPS_ANY);

static void
helper (gboolean flush)
{
    GstElement *filter;
    GstBus *bus;
    GstPad *mysrcpad;
    GstPad *mysinkpad;

    /* init */
    filter = gst_check_setup_element ("omx_dummy");
    mysrcpad = gst_check_setup_src_pad (filter, &srctemplate, NULL);
    mysinkpad = gst_check_setup_sink_pad (filter, &sinktemplate, NULL);

    gst_pad_set_active (mysrcpad, TRUE);
    gst_pad_set_active (mysinkpad, TRUE);

    g_object_set (G_OBJECT (filter), "library-name", "libomxil-foo.so", NULL);

    /* start */

    fail_unless_equals_int (gst_element_set_state (filter, GST_STATE_PLAYING),
                            GST_STATE_CHANGE_SUCCESS);

    bus = gst_bus_new ();

    gst_element_set_bus (filter, bus);

    /* send buffers in order*/
    {
        guint i;
        for (i = 0; i < BUFFER_COUNT; i++)
        {
            GstBuffer *inbuffer;
            inbuffer = gst_buffer_new_and_alloc (BUFFER_SIZE);
            GST_BUFFER_DATA(inbuffer)[0] = i;
            ASSERT_BUFFER_REFCOUNT (inbuffer, "inbuffer", 1);

            fail_unless (gst_pad_push (mysrcpad, inbuffer) == GST_FLOW_OK);

            if (flush && i % FLUSH_AT == 0)
            {
                gst_pad_push_event (mysrcpad, gst_event_new_flush_start ());
                gst_pad_push_event (mysrcpad, gst_event_new_flush_stop ());
                i += FLUSH_AT;
            }
        }
    }

    {
        GstMessage *message;
        fail_if ((message = gst_bus_pop (bus)) != NULL);

        /* make sure there's no error on the bus */
        message = gst_bus_poll (bus, GST_MESSAGE_ERROR, 0);
        fail_if (message);
    }

    gst_pad_push_event (mysrcpad, gst_event_new_eos ());

    /* check the order of the buffers*/
    if (!flush)
    {
        GList *cur;
        guint i;
        for (cur = buffers, i = 0; cur; cur = g_list_next (cur), i++)
        {
            GstBuffer *buffer;
            buffer = cur->data;
            fail_unless (GST_BUFFER_DATA(buffer)[0] == i);
        }
        fail_unless (i == BUFFER_COUNT);
    }

    /* cleanup */
    gst_bus_set_flushing (bus, TRUE);
    gst_element_set_bus (filter, NULL);
    gst_object_unref (GST_OBJECT (bus));
    gst_check_drop_buffers ();

    /* deinit */
    gst_element_set_state (filter, GST_STATE_NULL);

    gst_pad_set_active (mysrcpad, FALSE);
    gst_pad_set_active (mysinkpad, FALSE);
    gst_check_teardown_src_pad (filter);
    gst_check_teardown_sink_pad (filter);
    gst_check_teardown_element (filter);
}

GST_START_TEST (test_flush)
{
    helper (TRUE);
}
GST_END_TEST

GST_START_TEST (test_basic)
{
    helper (FALSE);
}
GST_END_TEST

static Suite *
gstomx_suite (void)
{
  Suite *s = suite_create ("gstomx");
  TCase *tc_chain = tcase_create ("general");

  tcase_set_timeout (tc_chain, 10);
  tcase_add_test (tc_chain, test_basic);
  tcase_add_test (tc_chain, test_flush);
  suite_add_tcase (s, tc_chain);

  return s;
}

GST_CHECK_MAIN (gstomx);
