#if defined __cplusplus
extern "C"
{
#endif

#ifndef lan_AVUTIL_ERROR_H
#define lan_AVUTIL_ERROR_H

/* error handling */
#if lan_EDOM > 0
#define lan_AVERROR(e) (-(e))   ///< Returns a negative error code from a POSIX error code, to return from library functions.
#define lan_AVUNERROR(e) (-(e)) ///< Returns a POSIX error code from a library function error return value.
#else
/* Some platforms have E* and errno already negated. */
#define lan_AVERROR(e) (e)
#define lan_AVUNERROR(e) (e)
#endif


#define lan_MKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))

#define lan_FFERRTAG(a, b, c, d) (-(int)lan_MKTAG(a, b, c, d))

#define lan_AVERROR_BSF_NOT_FOUND      lan_FFERRTAG(0xF8,'B','S','F') ///< Bitstream filter not found
#define lan_AVERROR_BUG                lan_FFERRTAG( 'B','U','G','!') ///< Internal bug, also see lan_AVERROR_BUG2
#define lan_AVERROR_BUFFER_TOO_SMALL   lan_FFERRTAG( 'B','U','F','S') ///< Buffer too small
#define lan_AVERROR_DECODER_NOT_FOUND  lan_FFERRTAG(0xF8,'D','E','C') ///< Decoder not found
#define lan_AVERROR_DEMUXER_NOT_FOUND  lan_FFERRTAG(0xF8,'D','E','M') ///< Demuxer not found
#define lan_AVERROR_ENCODER_NOT_FOUND  lan_FFERRTAG(0xF8,'E','N','C') ///< Encoder not found
#define lan_AVERROR_EOF                lan_FFERRTAG( 'E','O','F',' ') ///< End of file
#define lan_AVERROR_EXIT               lan_FFERRTAG( 'E','X','I','T') ///< Immediate exit was requested; the called function should not be restarted
#define lan_AVERROR_EXTERNAL           lan_FFERRTAG( 'E','X','T',' ') ///< Generic error in an external library
#define lan_AVERROR_FILTER_NOT_FOUND   lan_FFERRTAG(0xF8,'F','I','L') ///< Filter not found
#define lan_AVERROR_INVALIDDATA        lan_FFERRTAG( 'I','N','D','A') ///< Invalid data found when processing input
#define lan_AVERROR_MUXER_NOT_FOUND    lan_FFERRTAG(0xF8,'M','U','X') ///< Muxer not found
#define lan_AVERROR_OPTION_NOT_FOUND   lan_FFERRTAG(0xF8,'O','P','T') ///< Option not found
#define lan_AVERROR_PATCHWELCOME       lan_FFERRTAG( 'P','A','W','E') ///< Not yet implemented in FFmpeg, patches welcome
#define lan_AVERROR_PROTOCOL_NOT_FOUND lan_FFERRTAG(0xF8,'P','R','O') ///< Protocol not found

#define lan_AVERROR_STREAM_NOT_FOUND   lan_FFERRTAG(0xF8,'S','T','R') ///< Stream not found

#endif /* lan_AVUTIL_ERROR_H */

#if defined __cplusplus
}
#endif
