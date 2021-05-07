/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#include "SDL_video.h"
#include "SDL_sysvideo.h"
#include "SDL_blit.h"
#include "SDL_RLEaccel_c.h"
#include "SDL_pixels_c.h"
//#included "memfuncs.h"

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)) && SDL_ASSEMBLY_ROUTINES
#define MMX_ASMBLIT
#if (__GNUC__ > 2)  /* SSE instructions aren't in GCC 2. */
#define SSE_ASMBLIT
#endif
#endif

#if defined(MMX_ASMBLIT)
#include "SDL_cpuinfo.h"
#include "mmx.h"
#endif
#include "memfuncs.h"

// 8-bit (1 bytes at a time)
// Len is (# of total bytes/1), so it's "# of 8-bits"
// Source and destination buffers must both be 1-byte aligned (aka no alignment)



// 16-bit (2 bytes at a time)
// Len is (# of total bytes/2), so it's "# of 16-bits"
// Source and destination buffers must both be 2-byte aligned

void * memcpy_16bit(void *dest, const void *src, size_t len)
{
  if(!len)
  {
    return dest;
  }

  const uint16_t* s = (uint16_t*)src;
  uint16_t* d = (uint16_t*)dest;

  uint32_t diff = (uint32_t)d - (uint32_t)(s + 1); // extra offset because input gets incremented before output is calculated
  // Underflow would be like adding a negative offset

  // Can use 'd' as a scratch reg now
  asm volatile (
    "clrs\n" // Align for parallelism (CO) - SH4a use "stc SR, Rn" instead with a dummy Rn
  ".align 2\n"
  "0:\n\t"
    "dt %[size]\n\t" // (--len) ? 0 -> T : 1 -> T (EX 1)
    "mov.w @%[in]+, %[scratch]\n\t" // scratch = *(s++) (LS 1/2)
    "bf.s 0b\n\t" // while(s != nexts) aka while(!T) (BR 1/2)
    " mov.w %[scratch], @(%[offset], %[in])\n" // *(datatype_of_s*) ((char*)s + diff) = scratch, where src + diff = dest (LS 1)
    : [in] "+&r" ((uint32_t)s), [scratch] "=&r" ((uint32_t)d), [size] "+&r" (len) // outputs
    : [offset] "z" (diff) // inputs
    : "t", "memory" // clobbers
  );

  return dest;
}

// 32-bit (4 bytes at a time)
// Len is (# of total bytes/4), so it's "# of 32-bits"
// Source and destination buffers must both be 4-byte aligned

void * memcpy_32bit(void *dest, const void *src, size_t len)
{
  if(!len)
  {
    return dest;
  }

  const uint32_t* s = (uint32_t*)src;
  uint32_t* d = (uint32_t*)dest;

  uint32_t diff = (uint32_t)d - (uint32_t)(s + 1); // extra offset because input gets incremented before output is calculated
  // Underflow would be like adding a negative offset

  // Can use 'd' as a scratch reg now
  asm volatile (
    "clrs\n" // Align for parallelism (CO) - SH4a use "stc SR, Rn" instead with a dummy Rn
  ".align 2\n"
  "0:\n\t"
    "dt %[size]\n\t" // (--len) ? 0 -> T : 1 -> T (EX 1)
    "mov.l @%[in]+, %[scratch]\n\t" // scratch = *(s++) (LS 1/2)
    "bf.s 0b\n\t" // while(s != nexts) aka while(!T) (BR 1/2)
    " mov.l %[scratch], @(%[offset], %[in])\n" // *(datatype_of_s*) ((char*)s + diff) = scratch, where src + diff = dest (LS 1)
    : [in] "+&r" ((uint32_t)s), [scratch] "=&r" ((uint32_t)d), [size] "+&r" (len) // outputs
    : [offset] "z" (diff) // inputs
    : "t", "memory" // clobbers
  );

  return dest;
}

// 64-bit (8 bytes at a time)
// Len is (# of total bytes/8), so it's "# of 64-bits"
// Source and destination buffers must both be 8-byte aligned

void * memcpy_64bit(void *dest, const void *src, size_t len)
{
  if(!len)
  {
    return dest;
  }

  const _Complex float* s = (_Complex float*)src;
  _Complex float* d = (_Complex float*)dest;

  _Complex float double_scratch;

  uint32_t diff = (uint32_t)d - (uint32_t)(s + 1); // extra offset because input gets incremented before output is calculated
  // Underflow would be like adding a negative offset

  asm volatile (
    "fschg\n\t"
    "clrs\n" // Align for parallelism (CO) - SH4a use "stc SR, Rn" instead with a dummy Rn
  ".align 2\n"
  "0:\n\t"
    "dt %[size]\n\t" // (--len) ? 0 -> T : 1 -> T (EX 1)
    "fmov.d @%[in]+, %[scratch]\n\t" // scratch = *(s++) (LS 1/2)
    "bf.s 0b\n\t" // while(s != nexts) aka while(!T) (BR 1/2)
    " fmov.d %[scratch], @(%[offset], %[in])\n\t" // *(datatype_of_s*) ((char*)s + diff) = scratch, where src + diff = dest (LS 1)
    "fschg\n"
    : [in] "+&r" ((uint32_t)s), [scratch] "=&d" (double_scratch), [size] "+&r" (len) // outputs
    : [offset] "z" (diff) // inputs
    : "t", "memory" // clobbers
  );

  return dest;
}

// 16 Bytes at a time
// Len is (# of total bytes/16), so it's "# of 16 Bytes"
// Source and destination buffers must both be 4-byte aligned

void * memcpy_32bit_16Bytes(void *dest, const void *src, size_t len)
{
  void * ret_dest = dest;

  if(!len)
  {
    return ret_dest;
  }

  uint32_t scratch_reg;
  uint32_t scratch_reg2;
  uint32_t scratch_reg3;
  uint32_t scratch_reg4;

  asm volatile (
    "clrs\n" // Align for parallelism (CO) - SH4a use "stc SR, Rn" instead with a dummy Rn
  ".align 2\n"
  "1:\n\t"
    // *dest++ = *src++
    "mov.l @%[in]+, %[scratch]\n\t" // (LS)
    "mov.l @%[in]+, %[scratch2]\n\t" // (LS)
    "mov.l @%[in]+, %[scratch3]\n\t" // (LS)
    "add #16, %[out]\n\t" // (EX)
    "mov.l @%[in]+, %[scratch4]\n\t" // (LS)
    "dt %[size]\n\t" // while(--len) (EX)
    "mov.l %[scratch4], @-%[out]\n\t" // (LS)
    "mov.l %[scratch3], @-%[out]\n\t" // (LS)
    "mov.l %[scratch2], @-%[out]\n\t" // (LS)
    "mov.l %[scratch], @-%[out]\n\t" // (LS)
    "bf.s 1b\n\t" // (BR)
    " add #16, %[out]\n" // (EX)
    : [in] "+&r" ((uint32_t)src), [out] "+&r" ((uint32_t)dest), [size] "+&r" (len),
    [scratch] "=&r" (scratch_reg), [scratch2] "=&r" (scratch_reg2), [scratch3] "=&r" (scratch_reg3), [scratch4] "=&r" (scratch_reg4) // outputs
    : // inputs
    : "t", "memory" // clobbers
  );

  return ret_dest;
}

// 32 Bytes at a time
// Len is (# of total bytes/32), so it's "# of 32 Bytes"
// Source and destination buffers must both be 8-byte aligned
// NOTE: Store Queues are better, though they require 32-byte alignment. This is
// just here for if you really need it.

void * memcpy_64bit_32Bytes(void *dest, const void *src, size_t len)
{
  void * ret_dest = dest;

  if(!len)
  {
    return ret_dest;
  }

  _Complex float double_scratch;
  _Complex float double_scratch2;
  _Complex float double_scratch3;
  _Complex float double_scratch4;

  asm volatile (
    "fschg\n\t" // Switch to pair move mode (FE)
    "clrs\n" // Align for parallelism (CO) - SH4a use "stc SR, Rn" instead with a dummy Rn
  ".align 2\n"
  "1:\n\t"
    // *dest++ = *src++
    "pref @%[out]\n\t" // pref dest (LS)
    "fmov.d @%[in]+, %[scratch]\n\t" // (LS)
    "fmov.d @%[in]+, %[scratch2]\n\t" // (LS)
    "fmov.d @%[in]+, %[scratch3]\n\t" // (LS)
    "fmov.d @%[in]+, %[scratch4]\n\t" // (LS)
    "add #32, %[out]\n\t" // (EX)
    "pref @%[in]\n\t" // pref the next iteration of src (LS)
    "dt %[size]\n\t" // while(--len) (EX)
    "fmov.d %[scratch4], @-%[out]\n\t" // (LS)
    "fmov.d %[scratch3], @-%[out]\n\t" // (LS)
    "fmov.d %[scratch2], @-%[out]\n\t" // (LS)
    "fmov.d %[scratch], @-%[out]\n\t" // (LS)
    "bf.s 1b\n\t" // (BR)
    " add #32, %[out]\n\t" // (EX)
    "fschg\n" // Switch back to single move mode (FE)
    : [in] "+&r" ((uint32_t)src), [out] "+&r" ((uint32_t)dest), [size] "+&r" (len),
    [scratch] "=&d" (double_scratch), [scratch2] "=&d" (double_scratch2), [scratch3] "=&d" (double_scratch3), [scratch4] "=&d" (double_scratch4) // outputs
    : // inputs
    : "t", "memory" // clobbers
  );

  return ret_dest;
}
/* The general purpose software blit routine */
static int SDL_SoftBlit(SDL_Surface *src, SDL_Rect *srcrect,
			SDL_Surface *dst, SDL_Rect *dstrect)
{
	int okay;
	int src_locked;
	int dst_locked;

	/* Everything is okay at the beginning...  */
	okay = 1;

	/* Lock the destination if it's in hardware */
	dst_locked = 0;
	if ( SDL_MUSTLOCK(dst) ) {
		if ( SDL_LockSurface(dst) < 0 ) {
			okay = 0;
		} else {
			dst_locked = 1;
		}
	}
	/* Lock the source if it's in hardware */
	src_locked = 0;
	if ( SDL_MUSTLOCK(src) ) {
		if ( SDL_LockSurface(src) < 0 ) {
			okay = 0;
		} else {
			src_locked = 1;
		}
	}

	/* Set up source and destination buffer pointers, and BLIT! */
	if ( okay  && srcrect->w && srcrect->h ) {
		SDL_BlitInfo info;
		SDL_loblit RunBlit;

		/* Set up the blit information */
		info.s_pixels = (Uint8 *)src->pixels +
				(Uint16)srcrect->y*src->pitch +
				(Uint16)srcrect->x*src->format->BytesPerPixel;
		info.s_width = srcrect->w;
		info.s_height = srcrect->h;
		info.s_skip=src->pitch-info.s_width*src->format->BytesPerPixel;
		info.d_pixels = (Uint8 *)dst->pixels +
				(Uint16)dstrect->y*dst->pitch +
				(Uint16)dstrect->x*dst->format->BytesPerPixel;
		info.d_width = dstrect->w;
		info.d_height = dstrect->h;
		info.d_skip=dst->pitch-info.d_width*dst->format->BytesPerPixel;
		info.aux_data = src->map->sw_data->aux_data;
		info.src = src->format;
		info.table = src->map->table;
		info.dst = dst->format;
		RunBlit = src->map->sw_data->blit;

		/* Run the actual software blit */
		RunBlit(&info);
	}

	/* We need to unlock the surfaces if they're locked */
	if ( dst_locked ) {
		SDL_UnlockSurface(dst);
	}
	if ( src_locked ) {
		SDL_UnlockSurface(src);
	}
	/* Blit is done! */
	return(okay ? 0 : -1);
}

#ifdef MMX_ASMBLIT
static __inline__ void SDL_memcpyMMX(Uint8 *to, const Uint8 *from, int len)
{
	int i;

	for(i=0; i<len/8; i++) {
		__asm__ __volatile__ (
		"	movq (%0), %%mm0\n"
		"	movq %%mm0, (%1)\n"
		: : "r" (from), "r" (to) : "memory");
		from+=8;
		to+=8;
	}
	if (len&7)
		SDL_memcpy(to, from, len&7);
}

#ifdef SSE_ASMBLIT
static __inline__ void SDL_memcpySSE(Uint8 *to, const Uint8 *from, int len)
{
	int i;

	__asm__ __volatile__ (
	"	prefetchnta (%0)\n"
	"	prefetchnta 64(%0)\n"
	"	prefetchnta 128(%0)\n"
	"	prefetchnta 192(%0)\n"
	: : "r" (from) );

	for(i=0; i<len/8; i++) {
		__asm__ __volatile__ (
		"	prefetchnta 256(%0)\n"
		"	movq (%0), %%mm0\n"
		"	movntq %%mm0, (%1)\n"
		: : "r" (from), "r" (to) : "memory");
		from+=8;
		to+=8;
	}
	if (len&7)
		SDL_memcpy(to, from, len&7);
}
#endif
#endif

static void SDL_BlitCopy(SDL_BlitInfo *info)
{
	Uint8 *src, *dst;
	int w, h;
	int srcskip, dstskip;

	w = info->d_width*info->dst->BytesPerPixel;
	h = info->d_height;
	src = info->s_pixels;
	dst = info->d_pixels;
	srcskip = w+info->s_skip;
	dstskip = w+info->d_skip;

#ifdef __DREAMCAST__
  // Find out which optimised memcpy to call
  void* (*memcpy_f)(void*, const void*, size_t) = NULL;
  size_t mlen = w;
  if (w%32 == 0) {
    //printf("BLIT memcpy_64bit_32Bytes\n");
    memcpy_f = memcpy_64bit_32Bytes;
    mlen = w/32;
  } else if (w%16 == 0) {
    //printf("BLIT memcpy_32bit_16Bytes\n");
    //memcpy_f = memcpy_32bit_16Bytes;
    //mlen = w/16;
    memcpy_f = memcpy_16bit;
    mlen = w/2;
  } else if (w%8 == 0) {
    //printf("BLIT memcpy_64bit\n");
    memcpy_f = memcpy_64bit;
    mlen = w/8;
  } else if (w%4 == 0) {
    //printf("BLIT memcpy_32bit\n");
    memcpy_f = memcpy_32bit;
    mlen = w/4;
  } else if (w%2 == 0) {
    //printf("BLIT memcpy_16bit\n");
    memcpy_f = memcpy_16bit;
    mlen = w/2;
  } else {
    //printf("BLIT memcpy\n");
    memcpy_f = SDL_memcpy;
    mlen = w;
  }

#endif

#ifdef SSE_ASMBLIT
	if(SDL_HasSSE())
	{
		while ( h-- ) {
			SDL_memcpySSE(dst, src, w);
			src += srcskip;
			dst += dstskip;
		}
		__asm__ __volatile__ (
		"	emms\n"
		::);
	}
	else
#endif
#ifdef MMX_ASMBLIT
	if(SDL_HasMMX())
	{
		while ( h-- ) {
			SDL_memcpyMMX(dst, src, w);
			src += srcskip;
			dst += dstskip;
		}
		__asm__ __volatile__ (
		"	emms\n"
		::);
	}
	else
#endif
	while ( h-- ) {
#ifdef __DREAMCAST__
    memcpy_f(dst, src, mlen);
#else
		SDL_memcpy(dst, src, w);
#endif
		src += srcskip;
		dst += dstskip;
	}
}

static void SDL_BlitCopyOverlap(SDL_BlitInfo *info)
{
	Uint8 *src, *dst;
	int w, h;
	int srcskip, dstskip;

	w = info->d_width*info->dst->BytesPerPixel;
	h = info->d_height;
	src = info->s_pixels;
	dst = info->d_pixels;
	srcskip = w+info->s_skip;
	dstskip = w+info->d_skip;
	if ( dst < src ) {
		while ( h-- ) {
			SDL_memcpy(dst, src, w);
			src += srcskip;
			dst += dstskip;
		}
	} else {
		src += ((h-1) * srcskip);
		dst += ((h-1) * dstskip);
		while ( h-- ) {
			SDL_revcpy(dst, src, w);
			src -= srcskip;
			dst -= dstskip;
		}
	}
}

/* Figure out which of many blit routines to set up on a surface */
int SDL_CalculateBlit(SDL_Surface *surface)
{
	int blit_index;

	/* Clean everything out to start */
	if ( (surface->flags & SDL_RLEACCEL) == SDL_RLEACCEL ) {
		SDL_UnRLESurface(surface, 1);
	}
	surface->map->sw_blit = NULL;

	/* Figure out if an accelerated hardware blit is possible */
	surface->flags &= ~SDL_HWACCEL;
	if ( surface->map->identity ) {
		int hw_blit_ok;

		if ( (surface->flags & SDL_HWSURFACE) == SDL_HWSURFACE ) {
			/* We only support accelerated blitting to hardware */
			if ( surface->map->dst->flags & SDL_HWSURFACE ) {
				hw_blit_ok = current_video->info.blit_hw;
			} else {
				hw_blit_ok = 0;
			}
			if (hw_blit_ok && (surface->flags & SDL_SRCCOLORKEY)) {
				hw_blit_ok = current_video->info.blit_hw_CC;
			}
			if ( hw_blit_ok && (surface->flags & SDL_SRCALPHA) ) {
				hw_blit_ok = current_video->info.blit_hw_A;
			}
		} else {
			/* We only support accelerated blitting to hardware */
			if ( surface->map->dst->flags & SDL_HWSURFACE ) {
				hw_blit_ok = current_video->info.blit_sw;
			} else {
				hw_blit_ok = 0;
			}
			if (hw_blit_ok && (surface->flags & SDL_SRCCOLORKEY)) {
				hw_blit_ok = current_video->info.blit_sw_CC;
			}
			if ( hw_blit_ok && (surface->flags & SDL_SRCALPHA) ) {
				hw_blit_ok = current_video->info.blit_sw_A;
			}
		}
		if ( hw_blit_ok ) {
			SDL_VideoDevice *video = current_video;
			SDL_VideoDevice *this  = current_video;
			video->CheckHWBlit(this, surface, surface->map->dst);
		}
	}
	
	/* if an alpha pixel format is specified, we can accelerate alpha blits */
	if (((surface->flags & SDL_HWSURFACE) == SDL_HWSURFACE )&&(current_video->displayformatalphapixel)) 
	{
		if ( (surface->flags & SDL_SRCALPHA) ) 
			if ( current_video->info.blit_hw_A ) {
				SDL_VideoDevice *video = current_video;
				SDL_VideoDevice *this  = current_video;
				video->CheckHWBlit(this, surface, surface->map->dst);
			}
	}

	/* Get the blit function index, based on surface mode */
	/* { 0 = nothing, 1 = colorkey, 2 = alpha, 3 = colorkey+alpha } */
	blit_index = 0;
	blit_index |= (!!(surface->flags & SDL_SRCCOLORKEY))      << 0;
	if ( surface->flags & SDL_SRCALPHA
	     && (surface->format->alpha != SDL_ALPHA_OPAQUE
		 || surface->format->Amask) ) {
	        blit_index |= 2;
	}

	/* Check for special "identity" case -- copy blit */
	if ( surface->map->identity && blit_index == 0 ) {
	        surface->map->sw_data->blit = SDL_BlitCopy;

		/* Handle overlapping blits on the same surface */
		if ( surface == surface->map->dst ) {
		        surface->map->sw_data->blit = SDL_BlitCopyOverlap;
		}
	} else {
		if ( surface->format->BitsPerPixel < 8 ) {
			surface->map->sw_data->blit =
			    SDL_CalculateBlit0(surface, blit_index);
		} else {
			switch ( surface->format->BytesPerPixel ) {
			    case 1:
				surface->map->sw_data->blit =
				    SDL_CalculateBlit1(surface, blit_index);
				break;
			    case 2:
			    case 3:
			    case 4:
				surface->map->sw_data->blit =
				    SDL_CalculateBlitN(surface, blit_index);
				break;
			    default:
				surface->map->sw_data->blit = NULL;
				break;
			}
		}
	}
	/* Make sure we have a blit function */
	if ( surface->map->sw_data->blit == NULL ) {
		SDL_InvalidateMap(surface->map);
		SDL_SetError("Blit combination not supported");
		return(-1);
	}

	/* Choose software blitting function */
	if(surface->flags & SDL_RLEACCELOK
	   && (surface->flags & SDL_HWACCEL) != SDL_HWACCEL) {

	        if(surface->map->identity
		   && (blit_index == 1
		       || (blit_index == 3 && !surface->format->Amask))) {
		        if ( SDL_RLESurface(surface) == 0 )
			        surface->map->sw_blit = SDL_RLEBlit;
		} else if(blit_index == 2 && surface->format->Amask) {
		        if ( SDL_RLESurface(surface) == 0 )
			        surface->map->sw_blit = SDL_RLEAlphaBlit;
		}
	}
	
	if ( surface->map->sw_blit == NULL ) {
		surface->map->sw_blit = SDL_SoftBlit;
	}
	return(0);
}

