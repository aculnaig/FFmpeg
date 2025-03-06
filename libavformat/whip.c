/*
 * WHIP protocol
 *
 * Copyright (c) Gianluca Cannata <gcannata23@gmail.com>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "avformat.h"
#include "http.h"
#include "version.h"

#include "libavutil/log.h"
#include "libavutil/error.h"

typedef struct WHIPContext {
    const AVClass *class;
    URLContext *url;
} WHIPContext;

static int whip_open(URLContext *h, const char *url, int flags)
{
    return AVERROR(ENOSYS);
}

static int whip_read(URLContext *h, unsigned char *buf, int size)
{
    return AVERROR(ENOSYS);
}

static int whip_write(URLContext *h, const unsigned char *buf, int size)
{
    return AVERROR(ENOSYS);
}

static int whip_close(URLContext *h)
{
    return AVERROR(ENOSYS);
}

static const AVClass whip_context_class = {
    .class_name = "whip",
    .item_name = av_default_item_name,
    .version = LIBAVFORMAT_VERSION_INT,
};

const URLProtocol ff_whip_protocol = {
    .name = "whip",
    .url_open = whip_open,
    .url_read = whip_read,
    .url_write = whip_write,
    .url_close = whip_close,
    .priv_data_size = sizeof(WHIPContext),
    .flags = URL_PROTOCOL_FLAG_NETWORK,
    .priv_data_class = &whip_context_class,
    .default_whitelist = "http,https,tcp,tls",
};
