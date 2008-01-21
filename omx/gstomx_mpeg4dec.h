/*
 * Copyright (C) 2007 Nokia Corporation. All rights reserved.
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

#ifndef __GST_OMX_MPEG4DEC_H__
#define __GST_OMX_MPEG4DEC_H__

#include <gst/gst.h>

#include <config.h>

G_BEGIN_DECLS

#define GST_OMX_MPEG4DEC(obj) (GstOmxMpeg4Dec *) (obj)
#define GST_OMX_MPEG4DEC_TYPE (gst_omx_mpeg4dec_get_type ())

typedef struct GstOmxMpeg4Dec GstOmxMpeg4Dec;
typedef struct GstOmxMpeg4DecClass GstOmxMpeg4DecClass;

#include "gstomx_base_filter.h"

struct GstOmxMpeg4Dec
{
	GstOmxBaseFilter omx_base;
};

struct GstOmxMpeg4DecClass
{
	GstOmxBaseFilterClass parent_class;
};

GType gst_omx_mpeg4dec_get_type (void);

G_END_DECLS

#endif /* __GST_OMX_MPEG4DEC_H__ */
