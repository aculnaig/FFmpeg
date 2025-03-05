/*
 * OggKate subtitles demuxer
 *
 * Copyright (c) 2025 Gianluca Cannata <gcannata23@gmail.com>
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
#include "oggdec.h"

#include "libavcodec/bytestream.h"
#include "libavutil/error.h"

static int parse_kate_header(AVFormatContext *s, int idx)
{
    return AVERROR(ENOSYS);
}

static int parse_kate_packet(AVFormatContext *s, int idx)
{
    return AVERROR(ENOSYS);
}

static uint64_t parse_kate_gptopts(AVFormatContext *s, int idx, uint64_t gp, int64_t *dts)
{
    return AV_NOPTS_VALUE;
}

const struct ogg_codec ff_kate_codec = {
    .magic = "\200kate\0\0\0",
    .magicsize = 8,
    .header = parse_kate_header,
    .packet = parse_kate_packet,
    .gptopts = parse_kate_gptopts,
    .nb_header = 1,
    .granule_is_start = 1,
};
