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
#include "libavutil/mem.h"

typedef struct OggKateDemuxerContext {
    AVClass *class;
} OggKateDemuxerContext;

static int parse_kate_header(AVFormatContext *s, int idx)
{
    struct ogg *ogg = s->priv_data;
    struct ogg_stream *os = ogg->streams + idx;
    AVStream *st = s->streams[idx];
    OggKateDemuxerContext *kate_ctx = os->private;
    int packet_type = 0;
    uint8_t *buffer = os->buf;

    if (!kate_ctx) {
        kate_ctx = av_mallocz(sizeof(*kate_ctx));
        if (!kate_ctx)
            return AVERROR(ENOMEM);
        os->private = kate_ctx;
    }

    packet_type = bytestream_get_byte(&buffer);

    switch (packet_type) {
        case 0x80:
            st->codecpar->codec_type = AVMEDIA_TYPE_SUBTITLE;
            st->codecpar->codec_id = AV_CODEC_ID_KATE;
            return 1;
        case 0x81:
            return 1;
        case 0x82:
            return 1;
        case 0x83:
            return 1;
        case 0x84:
            return 1;
        case 0x85:
            return 1;
        case 0x86:
            return 1;
        case 0x87:
            return 1;
        case 0x88:
            return 1;
        case 0x00:
            return 0;
        case 0x01:
            return 0;
        case 0x02:
            return 0;
        case 0x7F:
            return 0;
        default:
            return AVERROR_INVALIDDATA;
    }
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
