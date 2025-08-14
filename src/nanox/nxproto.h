/*
 * Copyright (c) 1999, 2003, 2010 Greg Haerr <greg@censoft.com>
 * Copyright (c) 2000 Alex Holden <alex@linuxhacker.org>
 * Portions Copyright (c) 2002, 2003 by Koninklijke Philips Electronics N.V.
 *
 * Nano-X Core Protocol Header
 *
 * These structures define the Nano-X client/server protocol.
 * Much of this has been modeled after the X11 implementation.
 * Note that all NX Protocol requests should have all data quantities
 * properly aligned. This is assured by hand-coding each NX request
 * structure.  Unlike Xlib, fixed size request structs don't have to
 * be a multiple of 4 bytes, since the length field is a byte count
 * and GetReq() automatically pads all requests to 4-byte boundaries.
 * Request structs for variable size data, however, must be hand-padded
 * to 4-byte alignment, as variable data starts after sizeof(structure).
 * Also, the hilength/length fields store the unaligned byte count, so
 * that extra code isn't required to de-crypt extra data size or
 * big packets.
 */

/*
 * The following is provided to allow limiting the maximum
 * request size that will be sent (not received) using this protocol.
 * The protocol allows for 2^24 byte maximum, but the
 * server currently allocates the MAXREQUESTSZ in a stack buffer.
 * Also, the client realloc's the request queue to
 * the largest size asked for, and currently never reduces it.
 *
 * Routines like GrArea will split packets to be <= MAXREQUESTSZ
 * automatically.
 *
 * NOTE: MAXREQUESTSZ must be an _aligned_ multiple of 4, meaning
 * that MAXREQUESTSZ = (MAXREQUESTSZ + 3) & ~3.
 */
#if ELKS
#define MAXREQUESTSZ	512             /* max request size FIXME */
#define SZREQBUF        512	        /* initial request buffer size*/
#else
#define MAXREQUESTSZ	 2572864          /* max request size (65532)*/
#define SZREQBUF        2048	        /* initial request buffer size*/
#endif

typedef unsigned char	BYTE8;		/* 1 byte*/
typedef unsigned short	UINT16;		/* 2 bytes*/
typedef short		INT16;		/* 2 bytes*/
typedef unsigned long	UINT32;		/* 4 bytes*/

#if ELKS
typedef UINT16		IDTYPE;
#define ALIGNSZ		2	/* 2 byte packet alignment*/
#else
typedef UINT32		IDTYPE;
#define ALIGNSZ		4	/* 4 byte packet alignment*/
#endif

/* all requests share this header*/
 #pragma pack(1)
 typedef struct {
	BYTE8	reqType;	/* request code*/
	BYTE8	hilength;	/* upper 24 bits of unaligned length*/
	UINT16	length;		/* lower 16 bits of unaligned length*/
} nxReq;

/* Allocate a fixed size request from request buffer*/
#define AllocReq(name) \
	((nx##name##Req *)nxAllocReq(GrNum##name,sizeof(nx##name##Req), 0))

/* Allocate a request, but allocate n extra bytes*/
#define AllocReqExtra(name,n) \
	((nx##name##Req *)nxAllocReq(GrNum##name,sizeof(nx##name##Req), n))

/* return pointer to variable length data*/
#define GetReqData(req)		((void *)((char *)req + sizeof(* (req))))

/* FIXME fails when sizeof(int) == 2*/
/* get request total valid data length, including header*/
#if ELKS
#define GetReqLen(req)		((req)->length)     /* FIXME check hilength */
#else
#define GetReqLen(req)		(((req)->hilength << 16) | (req)->length)
#endif

/* get request variable data length, not including fixed size structure*/
#define GetReqVarLen(req)	(GetReqLen(req) - sizeof(* (req)))

/* get request total aligned length*/
#define GetReqAlignedLen(req)	((GetReqLen(req) + (ALIGNSZ-1)) & ~(ALIGNSZ-1))

void * 	nxAllocReq(int type, unsigned long size, long extra);
void	nxFlushReq(unsigned long newsize, int reply_needed);
void 	nxAssignReqbuffer(char *buffer, unsigned long size);
void 	nxWriteSocket(char *buf, int todo);
int	nxCalcStringBytes(void *str, int count, GR_TEXTFLAGS flags);

#if notyet
/* all replies share this header*/
#pragma pack(1)
typedef struct {
	BYTE8	repType;	/* reply code*/
	BYTE8	hilength;	/* upper 24 bits of unaligned length*/
	UINT16	length;		/* lower 16 bits of unaligned length*/
} nxReply;

/* reply types if not equal to request type*/
#define GrNumErrorReply		255
#define GrNumEventReply		254
#endif /* notyet*/

#define GrNumOpen               0
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	UINT32	pid;
} nxOpenReq;

#define GrNumClose              1
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxCloseReq;

#define GrNumGetScreenInfo      2
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxGetScreenInfoReq;

#define GrNumNewWindow          3
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	parentid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
	UINT32	backgroundcolor;
	UINT32	bordercolor;
	INT16	bordersize;
} nxNewWindowReq;

#define GrNumNewInputWindow     4
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	parentid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
} nxNewInputWindowReq;

#define GrNumDestroyWindow      5
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxDestroyWindowReq;

#define GrNumNewGC              6
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxNewGCReq;

#define GrNumCopyGC		7
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	gcid;
} nxCopyGCReq;

#define GrNumGetGCInfo          8
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	gcid;
} nxGetGCInfoReq;

#define GrNumDestroyGC          9
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	gcid;
} nxDestroyGCReq;

#define GrNumMapWindow          10
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxMapWindowReq;

#define GrNumUnmapWindow        11
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxUnmapWindowReq;

#define GrNumRaiseWindow        12
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxRaiseWindowReq;

#define GrNumLowerWindow        13
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxLowerWindowReq;

#define GrNumMoveWindow         14
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
	INT16	x;
	INT16	y;
} nxMoveWindowReq;

#define GrNumResizeWindow       15
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
	INT16	width;
	INT16	height;
} nxResizeWindowReq;

#define GrNumGetWindowInfo      16
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxGetWindowInfoReq;

#define GrNumGetFontInfo        17
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	fontid;
} nxGetFontInfoReq;

#define GrNumSetFocus           18
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxSetFocusReq;

#define GrNumSetWindowCursor    19
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
	IDTYPE	cursorid;
} nxSetWindowCursorReq;

#define GrNumClearArea          20
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
	UINT16	exposeflag;
} nxClearAreaReq;

#define GrNumSelectEvents       21
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
	UINT32	eventmask;
} nxSelectEventsReq;

#define GrNumGetNextEvent       22
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxGetNextEventReq;

#define GrNumCheckNextEvent     23
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxCheckNextEventReq;

#define GrNumPeekEvent          24
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxPeekEventReq;

#define GrNumLine               25
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x1;
	INT16	y1;
	INT16	x2;
	INT16	y2;
} nxLineReq;

#define GrNumPoint              26
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
} nxPointReq;

#define GrNumRect               27
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
} nxRectReq;

#define GrNumFillRect           28
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
} nxFillRectReq;

#define GrNumPoly               29
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	/*INT16 pointtable[];*/
} nxPolyReq;

#define GrNumFillPoly           30
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	/*INT16 pointtable[];*/
} nxFillPolyReq;

#define GrNumEllipse            31
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	rx;
	INT16	ry;
} nxEllipseReq;

#define GrNumFillEllipse        32
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	rx;
	INT16	ry;
} nxFillEllipseReq;

#define GrNumSetGCForeground    33
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	gcid;
	UINT32	color;
} nxSetGCForegroundReq;

#define GrNumSetGCBackground    34
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	gcid;
	UINT32	color;
} nxSetGCBackgroundReq;

#define GrNumSetGCUseBackground 35
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	gcid;
	UINT16	flag;
} nxSetGCUseBackgroundReq;

#define GrNumSetGCMode          36
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	gcid;
	UINT16	mode;
} nxSetGCModeReq;

#define GrNumSetGCFont          37
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	gcid;
	IDTYPE	fontid;
} nxSetGCFontReq;

#define GrNumGetGCTextSize      38
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	gcid;
	UINT32	flags;
	UINT32	charcount;
	/*BYTE8	text[];*/
} nxGetGCTextSizeReq;

#define GrNumReadArea           39
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
} nxReadAreaReq;

#define GrNumArea               40
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
	INT16	pixtype;
	INT16	pad;
	/*UINT32 pixels[];*/
} nxAreaReq;

#define GrNumBitmap             41
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
	/*UINT16 bitmaptable[];*/
} nxBitmapReq;

#define GrNumText               42
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	count;
	INT16	pad;
	UINT32	flags;
	/*BYTE8	text[];*/
} nxTextReq;

#define GrNumNewCursor          43
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	INT16	width;
	INT16	height;
	INT16	hotx;
	INT16	hoty;
	UINT32	fgcolor;
	UINT32	bgcolor;
	/*UINT16 fgbitmap[];*/
	/*UINT16 bgbitmap[];*/
} nxNewCursorReq;

#define GrNumMoveCursor         44
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	INT16	x;
	INT16	y;
} nxMoveCursorReq;

#define GrNumGetSystemPalette      45
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxGetSystemPaletteReq;

#define GrNumFindColor             46
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	UINT32	color;
} nxFindColorReq;

#define GrNumReparentWindow        47
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
	IDTYPE	parentid;
	INT16	x;
	INT16	y;
} nxReparentWindowReq;

#define GrNumDrawImageFromFile     48
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
	IDTYPE	flags;
	/*char path[];*/
} nxDrawImageFromFileReq;

#define GrNumLoadImageFromFile     49
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	INT16	flags;
	INT16	pad;
} nxLoadImageFromFileReq;

#define GrNumNewPixmapEx          50
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	INT16	width;
	INT16	height;
	UINT32	format;
/* FIXME: Add support for passing shared memory info */
} nxNewPixmapExReq;

#define GrNumCopyArea          51
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
	IDTYPE	srcid;
	INT16	srcx;
	INT16	srcy;
	UINT32	op;
} nxCopyAreaReq;

#define GrNumSetFontSizeEx      52
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	fontid;
	INT16	height;
	INT16	width;
} nxSetFontSizeExReq;

#define GrNumCreateFontEx		53
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	INT16	height;
	INT16	width;
} nxCreateFontExReq;

#define GrNumDestroyFont	54
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	fontid;
} nxDestroyFontReq;

#define GrNumReqShmCmds         55
#pragma pack(1)
typedef struct {
	BYTE8   reqType;
	BYTE8   hilength;
	UINT16  length;
	UINT32  size;
} nxReqShmCmdsReq;

#define GrNumShmCmdsFlush       56
#pragma pack(1)
typedef struct {
	BYTE8   reqType;
	BYTE8   hilength;
	UINT16  length;
	UINT32  size;
	UINT32  reply;
} nxShmCmdsFlushReq;

#define GrNumSetFontRotation    57
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	fontid;
	INT16	tenthdegrees;
} nxSetFontRotationReq;

#define GrNumSetFontAttr        58
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	fontid;
	INT16	setflags;
	INT16	clrflags;
} nxSetFontAttrReq;

#define GrNumSetSystemPalette   59
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	INT16	first;
	INT16	count;
	MWPALENTRY palette[256];
} nxSetSystemPaletteReq;

#define GrNumInjectEvent	60
#define GR_INJECT_EVENT_POINTER		0
#define GR_INJECT_EVENT_KEYBOARD	1
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	union {
		struct {
			INT16	x;
			INT16	y;
			UINT16	button;
			BYTE8	visible;
		} pointer;
		struct {
			IDTYPE	wid;
			UINT16	keyvalue;
			UINT16	modifier;
			BYTE8	scancode;
			BYTE8	pressed;
		} keyboard;
	} event;
	UINT16	event_type;
} nxInjectEventReq;

#define GrNumNewRegion		61
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxNewRegionReq;

#define GrNumDestroyRegion	62
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	regionid;
} nxDestroyRegionReq;

#define GrNumUnionRectWithRegion	63
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	regionid;
	GR_RECT	rect;
} nxUnionRectWithRegionReq;

#define GrNumUnionRegion	64
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	regionid;
	IDTYPE	srcregionid1;
	IDTYPE	srcregionid2;
} nxUnionRegionReq;

#define GrNumIntersectRegion	65
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	regionid;
	IDTYPE	srcregionid1;
	IDTYPE	srcregionid2;
} nxIntersectRegionReq;

#define GrNumSetGCRegion	66
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	gcid;
	IDTYPE	regionid;
} nxSetGCRegionReq;

#define GrNumSubtractRegion	67
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	regionid;
	IDTYPE	srcregionid1;
	IDTYPE	srcregionid2;
} nxSubtractRegionReq;

#define GrNumXorRegion		68
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	regionid;
	IDTYPE	srcregionid1;
	IDTYPE	srcregionid2;
} nxXorRegionReq;

#define GrNumPointInRegion	69
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	regionid;
	INT16	x;
	INT16	y;
} nxPointInRegionReq;

#define GrNumRectInRegion	70
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	regionid;
	INT16	x;
	INT16	y;
	INT16	w;
	INT16	h;
} nxRectInRegionReq;

#define GrNumEmptyRegion	71
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	regionid;
} nxEmptyRegionReq;

#define GrNumEqualRegion	72
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	region1;
	IDTYPE	region2;
} nxEqualRegionReq;

#define GrNumOffsetRegion	73
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	region;
	INT16	dx;
	INT16	dy;
} nxOffsetRegionReq;

#define GrNumGetRegionBox	74
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	regionid;
} nxGetRegionBoxReq;

#define GrNumNewPolygonRegion	75
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	UINT16	mode;
	UINT16	pad;
	/*INT16 points[];*/
} nxNewPolygonRegionReq;

#define GrNumArc		76
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	rx;
	INT16	ry;
	INT16	ax;
	INT16	ay;
	INT16	bx;
	INT16	by;
	INT16	type;
} nxArcReq;

#define GrNumArcAngle		77
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	rx;
	INT16	ry;
	INT16	angle1;
	INT16	angle2;
	INT16	type;
} nxArcAngleReq;

#define GrNumSetWMProperties	78
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
	/* GR_WM_PROPERTIES props */
	/* GR_CHAR *title */
} nxSetWMPropertiesReq;

#define GrNumGetWMProperties	79
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxGetWMPropertiesReq;

#define GrNumCloseWindow	80
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxCloseWindowReq;

#define GrNumKillWindow		81
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxKillWindowReq;

#define GrNumDrawImageToFit     82
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
	IDTYPE	imageid;
} nxDrawImageToFitReq;

#define GrNumFreeImage          83
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	id;
} nxFreeImageReq;

#define GrNumGetImageInfo       84
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	id;
} nxGetImageInfoReq;

#define GrNumDrawImageBits      85
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	flags;		/* MWIMAGEHDR start*/
	INT16	width;
	INT16	height;
	INT16	planes;
	INT16	bpp;
	INT16	palsize;
	UINT32	data_format;
	UINT32	pitch;
	UINT32	transcolor;
	/*MWIMAGEBITS imagebits[];*/
	/*MWPALENTRY palette[palsize];*/
} nxDrawImageBitsReq;

#define GrNumPoints             86
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	/*INT16 pointtable[];*/
} nxPointsReq;

#define GrNumGetFocus           87
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxGetFocusReq;

#define GrNumGetSysColor        88
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	UINT16	index;
} nxGetSysColorReq;

#define GrNumSetScreenSaverTimeout	89
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	UINT32	timeout;
} nxSetScreenSaverTimeoutReq;

#define GrNumSetSelectionOwner	90
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	wid;
	/* GR_CHAR *typelist */
} nxSetSelectionOwnerReq;

#define GrNumGetSelectionOwner	91
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxGetSelectionOwnerReq;

#define GrNumRequestClientData	92
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	wid;
	IDTYPE	rid;
	UINT32	serial;
	UINT16	mimetype;
} nxRequestClientDataReq;

#define GrNumSendClientData	93
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	wid;
	IDTYPE	did;
	UINT32	serial;
	UINT32	len;
	/* void *data */
} nxSendClientDataReq;

#define GrNumBell		94
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxBellReq;

#define GrNumSetBackgroundPixmap 95
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	wid;
	IDTYPE	pixmap;
	UINT32	flags;
} nxSetBackgroundPixmapReq;

#define GrNumDestroyCursor	96
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	cursorid;
} nxDestroyCursorReq;

#define GrNumQueryTree   	97
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	windowid;
} nxQueryTreeReq;

#define GrNumCreateTimer	98
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	wid;
	UINT32	period;
} nxCreateTimerReq;

#define GrNumDestroyTimer	99
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	timerid;
} nxDestroyTimerReq;

#define GrNumSetPortraitMode	100
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	UINT32	portraitmode;
} nxSetPortraitModeReq;

#define GrNumImageBufferAlloc   101

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	UINT32	size;
} nxImageBufferAllocReq;

#define GrNumImageBufferSend    102

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	UINT32	buffer_id;
	UINT32	size;
} nxImageBufferSendReq;

#define GrNumLoadImageFromBuffer 103
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	UINT32	buffer;
	INT16	flags;
	INT16	pad;
} nxLoadImageFromBufferReq;

#define GrNumDrawImageFromBuffer 104
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	x;
	INT16	y;
	INT16	width;
	INT16	height;
	UINT32	buffer;
	IDTYPE	flags;
} nxDrawImageFromBufferReq;

#define GrNumGetFontList        105
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxGetFontListReq;

#define GrNumSetGCClipOrigin    106

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE  gcid;
	UINT32  xoff;
	UINT32  yoff;
} nxSetGCClipOriginReq;

#define GrNumSetGCGraphicsExposure 107

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE  gcid;
        UINT16  exposure;
} nxSetGCGraphicsExposureReq;

#define GrNumQueryPointer       108

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
} nxQueryPointerReq;

#define GrNumSetGCLineAttributes 109

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
  	IDTYPE	gcid;
        UINT16  linestyle;
} nxSetGCLineAttributesReq;

#define GrNumSetGCDash          110

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
        IDTYPE	gcid;
        UINT16	count;
} nxSetGCDashReq;

#define GrNumSetGCFillMode      111

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
        IDTYPE	gcid;
        UINT16	fillmode;
} nxSetGCFillModeReq;

#define GrNumSetGCStipple       112

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
        IDTYPE	gcid;
        INT16	width;
        INT16	height;
} nxSetGCStippleReq;

#define GrNumSetGCTSOffset      113

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
        IDTYPE	gcid;
        INT16	xoffset;
        INT16	yoffset;
} nxSetGCTSOffsetReq;

#define GrNumSetGCTile          114

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
        IDTYPE  gcid;
        IDTYPE  pixmap;
        INT16	width;
        INT16	height;
} nxSetGCTileReq;

#define GrNumNewBitmapRegion    115

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	INT16	width;
	INT16	height;
	/* GR_BITMAP bitmap[]*/
} nxNewBitmapRegionReq;

#define GrNumSetWindowRegion    116

#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	wid;
	IDTYPE	rid;
	UINT16	type;
} nxSetWindowRegionReq;

#define GrNumSetGCForegroundPixelVal    117

#pragma pack(1)
typedef struct {
	BYTE8 reqType;
	BYTE8 hilength;
	UINT16 length;
	IDTYPE gcid;
	UINT32 pixelval;
} nxSetGCForegroundPixelValReq;

#define GrNumSetGCBackgroundPixelVal    118

#pragma pack(1)
typedef struct {
	BYTE8 reqType;
	BYTE8 hilength;
	UINT16 length;
	IDTYPE gcid;
	UINT32 pixelval;
} nxSetGCBackgroundPixelValReq;

#define GrNumCreateLogFont      119
#pragma pack(1)
typedef struct {
	BYTE8 reqType;
	BYTE8 hilength;
	UINT16 length;
	MWLOGFONT lf;
} nxCreateLogFontReq;

#define GrNumStretchArea        120
#pragma pack(1)
typedef struct {
	BYTE8 reqType;
	BYTE8 hilength;
	UINT16 length;
	IDTYPE drawid;
	IDTYPE gcid;
	INT16 dx1;
	INT16 dy1;
	INT16 dx2;
	INT16 dy2;
	IDTYPE srcid;
	INT16 sx1;
	INT16 sy1;
	INT16 sx2;
	INT16 sy2;
	UINT32 op;
} nxStretchAreaReq;

#define GrNumGrabKey            121
#pragma pack(1)
typedef struct {
        BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
        IDTYPE	wid;
        INT16	type;
        UINT16	key;
} nxGrabKeyReq;

#define GrNumSetTransform       122
#pragma pack(1)
typedef struct {
        BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	UINT32	mode;
	UINT32	trans_a;
	UINT32	trans_b;
	UINT32	trans_c;
	UINT32	trans_d;
	UINT32	trans_e;
	UINT32	trans_f;
	UINT32	trans_s;
} nxSetTransformReq;

#define GrNumCreateFontFromBuffer	123
#pragma pack(1)
typedef struct {
	BYTE8 reqType;
	BYTE8 hilength;
	UINT16 length;
	UINT32 buffer_id;
	BYTE8 format[16];
	INT16 height;
	INT16 width;
} nxCreateFontFromBufferReq;

#define GrNumCopyFont		124
#pragma pack(1)
typedef struct {
	BYTE8 reqType;
	BYTE8 hilength;
	UINT16 length;
	IDTYPE fontid;
	INT16 height;
	INT16 width;
} nxCopyFontReq;

#define GrNumDrawImagePartToFit     125
#pragma pack(1)
typedef struct {
	BYTE8	reqType;
	BYTE8	hilength;
	UINT16	length;
	IDTYPE	drawid;
	IDTYPE	gcid;
	INT16	dx;
	INT16	dy;
	INT16	dwidth;
	INT16	dheight;
	INT16   sx;
	INT16   sy;
	INT16   swidth;
	INT16   sheight;
	IDTYPE	imageid;
} nxDrawImagePartToFitReq;

#define GrTotalNumCalls         126
