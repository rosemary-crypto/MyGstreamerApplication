// ************************************** Hard Coded ***************************************
#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#define DEBUGGING FALSE
#define MAX_PAD_COUNT 5

typedef struct {
    GstFormat format;
    gint64 start;
    gint64 stop;
    gint64 position;
    gboolean update;
} SegmentInfo;

SegmentInfo currentSegment = { GST_FORMAT_UNDEFINED, 0, 0, 0, FALSE };

static gboolean bus_call(GstBus* bus, GstMessage* msg, gpointer data) {
    GMainLoop* loop = (GMainLoop*)data;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        g_print("****APP: Bus End of stream\n");
        g_main_loop_quit(loop);
        break;

    case GST_MESSAGE_ERROR: {
        gchar* debug;
        GError* error;

        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);

        g_printerr("****APP: Bus Error: %s\n", error->message);
        g_error_free(error);

        g_main_loop_quit(loop);
        break;
    }

    default:
        break;
    }

    return TRUE;
}

static void on_pad_added(GstElement* element, GstPad* pad, gpointer data) {
    GstPad* sinkpad;
    GstElement* melement = (GstElement*)data;

    g_print("****APP: Dynamic pad created, elements: (%s,%s), pad: %s\n", gst_element_get_name(element), gst_element_get_name(melement), GST_PAD_NAME(pad));
    sinkpad = gst_element_get_static_pad(melement, "sink");
    if (!sinkpad) {
        g_print("****APP: Sink not created successfully!");
    }
    gst_pad_link(pad, sinkpad);
    gst_object_unref(sinkpad);
}

static void on_pad_added_for_demux(GstElement* element, GstPad* pad, gpointer data) {
    GstObject* parent = gst_element_get_parent(element);
    GstElement** queues = (GstElement**)data;
    GstPad* sinkpad;

    gint pad_count = atoi(strchr(gst_pad_get_name(pad), '_') + 1);
    g_print("****APP: Pad count is : %d\n", pad_count);

    sinkpad = gst_element_get_static_pad(queues[pad_count - 1], "sink");
    if (!sinkpad) {
        g_print("****APP: Sink not created successfully!");
    }
    gst_pad_link(pad, sinkpad);
    gst_object_unref(sinkpad);
}


int main(int argc, char* argv[]) {
    GMainLoop* loop;
    GstElement* queues[MAX_PAD_COUNT];

    GstBus* bus;
    guint bus_watch_id;

    gst_init(&argc, &argv);
    loop = g_main_loop_new(NULL, FALSE);

#if DEBUGGING
    // Set the GST_DEBUG environment variable
    g_setenv("GST_DEBUG", "videomux:4", TRUE);

    // Redirect standard output and error streams to a file
    FILE* log_file;
    if (freopen_s(&log_file, "debug_ms.log", "w", stdout) != 0) {
        g_printerr("****APP: Error opening log file.\n");
        return -1;
    }

    FILE* error_log_file;
    if (freopen_s(&error_log_file, "error_debug_ms.log", "w", stderr) != 0) {
        g_printerr("****APP: Error opening error log file.\n");
        return -1;
    }
#endif

    GstElement* pipeline = gst_pipeline_new("video-player");
    GstElement* source1 = gst_element_factory_make("filesrc", "file-source1");
    GstElement* decoder1 = gst_element_factory_make("decodebin", "decode-bin1");
    GstElement* muxer = gst_element_factory_make("videomux", "muxer");
    GstElement* sink1 = gst_element_factory_make("autovideosink", "video-output1");
    GstElement* videoscale1 = gst_element_factory_make("videoscale", "videoscale1");
    GstElement* display_meta = gst_element_factory_make("printmeta", "display_meta");
    GstElement* inference = gst_element_factory_make("pintelinference", "inference");
    GstElement* tracker1 = gst_element_factory_make("commontracker", "tracker1");
    //GstElement* tracker2 = gst_element_factory_make("commontracker", "tracker2");
    //g_assert(display_meta);

    g_object_set(G_OBJECT(source1), "location", "D:/videos/vid1.mp4", NULL);
    g_object_set(G_OBJECT(inference), "config_uri", "D:/Projects/GitLab/PintelGstreamerPlugins/phrd.json", NULL);
    g_object_set(G_OBJECT(tracker1), "config_uri", "D:/Projects/GitLab/PintelGstreamerPlugins/commontracker.json", NULL);
    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    bus_watch_id = gst_bus_add_watch(bus, bus_call, loop);
    gst_object_unref(bus);

    gst_bin_add_many(GST_BIN(pipeline),
        source1, decoder1, videoscale1, muxer, inference, tracker1, sink1, NULL);

    gst_element_link(source1, decoder1);

    g_signal_connect(decoder1, "pad-added", G_CALLBACK(on_pad_added), videoscale1);
    gst_element_link_many(videoscale1, muxer, inference, tracker1, sink1);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_main_loop_run(loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    g_source_remove(bus_watch_id);
    g_main_loop_unref(loop);

#if DEBUGGING
    fclose(stderr);
    fclose(stdout);
#endif
    return 0;
}