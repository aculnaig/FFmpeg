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

#include <ctype.h>

#include "avformat.h"
#include "oggdec.h"
#include "version.h"

#include "libavcodec/bytestream.h"
#include "libavutil/dict.h"
#include "libavutil/error.h"
#include "libavutil/mem.h"

typedef struct OggKateDemuxerContext {
    const AVClass *class;
    char magic[7];
    int major, minor;
    uint32_t gps_num;
    uint32_t gps_den;
    int granule_shift;

    int canvas_width;
    int canvas_height;

    int num_headers;
    int text_encoding;
    int directionality;

    char language[16];
    char category[16];
} OggKateDemuxerContext;

static int parse_kate_header(AVFormatContext *s, int idx)
{
    struct ogg *ogg = s->priv_data;
    struct ogg_stream *os = ogg->streams + idx;
    AVStream *st = s->streams[idx];
    OggKateDemuxerContext *kate_ctx = os->private;
    int packet_type = 0;
    uint16_t canvas_shift = 0;
    uint8_t *buffer = os->buf + os->pstart;

    if (!kate_ctx) {
        kate_ctx = av_mallocz(sizeof(*kate_ctx));
        if (!kate_ctx)
            return AVERROR(ENOMEM);
        os->private = kate_ctx;
    }

    packet_type = bytestream_get_byte(&buffer);

    switch (packet_type) {
        case 0x80:
            if (os->psize < 63) {
                av_log(s, AV_LOG_ERROR, "Invalid Kate BOS header size: (%u)\n", os->psize);
                return AVERROR_INVALIDDATA;
            }

            bytestream_get_buffer(&buffer, kate_ctx->magic, 7);

            if (strncmp(kate_ctx->magic, "kate\0\0\0", 7)) {
                av_log(s, AV_LOG_ERROR, "Invalid Kate BOS header signature: (%7s)\n", kate_ctx->magic);
                return AVERROR_INVALIDDATA;
            }

            (void) bytestream_get_byte(&buffer); // reserved (0 byte)

            kate_ctx->major = bytestream_get_byte(&buffer);
            kate_ctx->minor = bytestream_get_byte(&buffer);

            kate_ctx->num_headers = bytestream_get_byte(&buffer);
            kate_ctx->text_encoding = bytestream_get_byte(&buffer);
            kate_ctx->directionality = bytestream_get_byte(&buffer);

            (void) bytestream_get_byte(&buffer); // reserved (0 byte)

            kate_ctx->granule_shift = bytestream_get_byte(&buffer);

            canvas_shift = bytestream_get_le16(&buffer);
            kate_ctx->canvas_width = (canvas_shift & ((1 << 12) - 1)) << (canvas_shift >> 12);

            canvas_shift = bytestream_get_le16(&buffer);
            kate_ctx->canvas_height = (canvas_shift & ((1 << 12) - 1)) << (canvas_shift >> 12);

            (void) bytestream_get_byte(&buffer); // reserved (0 byte)

            kate_ctx->gps_num = bytestream_get_le32(&buffer);
            kate_ctx->gps_den = bytestream_get_le32(&buffer);

            bytestream_get_buffer(&buffer, kate_ctx->language, 16);
            bytestream_get_buffer(&buffer, kate_ctx->category, 16);

            if (kate_ctx->major > 0) {
                av_log(s, AV_LOG_ERROR, "Unsupported Kate bitstream major version: (%d)\n", kate_ctx->major);
                return AVERROR_INVALIDDATA;
            }

            if (kate_ctx->minor > 7) {
                av_log(s, AV_LOG_ERROR, "Unsupported Kate bitstream minor version: (%d)\n", kate_ctx->minor);
                return AVERROR_INVALIDDATA;
            }

            if (kate_ctx->text_encoding != 0) {
                av_log(s, AV_LOG_ERROR, "Unsupported Kate text encoding: (%d)\n", kate_ctx->text_encoding);
                return AVERROR_INVALIDDATA;
            }

            if (kate_ctx->granule_shift >= 64) {
                av_log(s, AV_LOG_ERROR, "Invalid Kate granule shift: (%d)\n", kate_ctx->granule_shift);
                return AVERROR_INVALIDDATA;
            }

            if (kate_ctx->gps_num == 0 || kate_ctx->gps_den == 0) {
                av_log(s, AV_LOG_ERROR, "Invalid Kate GPS ratio: (%u/%u)\n", kate_ctx->gps_num, kate_ctx->gps_den);
                return AVERROR_INVALIDDATA;
            }

            if (kate_ctx->language[15]) {
                av_log(s, AV_LOG_ERROR, "Invalid Kate language: (%15s)\n", kate_ctx->language);
                return AVERROR_INVALIDDATA;
            }

            if (kate_ctx->category[15]) {
                av_log(s, AV_LOG_ERROR, "Invalid Kate category: (%15s)\n", kate_ctx->category);
                return AVERROR_INVALIDDATA;
            }

            st->codecpar->codec_type = AVMEDIA_TYPE_SUBTITLE;
            st->codecpar->codec_id = AV_CODEC_ID_KATE;
            st->time_base.num = kate_ctx->gps_den;
            st->time_base.den = kate_ctx->gps_num;

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

const AVClass ff_kate_demuxer_class = {
    .class_name = "oggkate",
    .category = AV_CLASS_CATEGORY_DEMUXER,
    .item_name = av_default_item_name,
    .version = LIBAVFORMAT_VERSION_INT,
};

const struct ogg_codec ff_kate_codec = {
    .magic = "\200kate\0\0\0",
    .magicsize = 8,
    .header = parse_kate_header,
    .packet = parse_kate_packet,
    .gptopts = parse_kate_gptopts,
    .nb_header = 1,
    .granule_is_start = 1,
};
