  .text

/***************************************************************************/

.globl _dc_sin
_dc_sin:
  mova _dcmath_sinscale, r0
  fmov.s @r0, fr0
  fmul fr0, fr4
  ftrc fr4, fpul
  .short 0xF0FD
  rts
  nop

.globl _dc_cos
_dc_cos:
  mova _dcmath_sinscale, r0
  fmov.s @r0, fr0
  fmul fr0, fr4
  ftrc fr4, fpul
  .short 0xF0FD
  fmov fr1, fr0
  rts
  nop

.globl _dc_tan
_dc_tan:
  mova _dcmath_sinscale, r0
  fmov.s @r0, fr0
  fmul fr0, fr4
  ftrc fr4, fpul
  .short 0xF0FD
  fdiv fr1, fr0
  rts
  nop

.globl _dc_cot
_dc_cot:
  mova _dcmath_sinscale, r0
  fmov.s @r0, fr0
  fmul fr0, fr4
  ftrc fr4, fpul
  .short 0xF0FD
  fdiv fr0, fr1
  fmov fr1, fr0
  rts
  nop

.globl _dc_u16_sin
_dc_u16_sin:
  lds r4, fpul
  .short 0xF0FD
  rts
  nop

.globl _dc_u16_cos
_dc_u16_cos:
  lds r4, fpul
  .short 0xF0FD
  fmov fr1, fr0
  rts
  nop

.globl _dc_u16_tan
_dc_u16_tan:
  lds r4, fpul
  .short 0xF0FD
  fdiv fr1, fr0
  rts
  nop

.globl _dc_u16_cot
_dc_u16_cot:
  lds r4, fpul
  .short 0xF0FD
  fdiv fr0, fr1
  fmov fr1, fr0
  rts
  nop

.globl _dc_sqrt
_dc_sqrt:
  fmov fr4, fr0
  fsqrt fr0
  rts
  nop

.globl _dc_isqrt
_dc_isqrt:
  fmov fr4, fr0
  .short 0xF07D
  rts
  nop

.globl _dc_vector3_normalize
_dc_vector3_normalize:
  fmov.s @r4+, fr0
  fldi0 fr3
  fmov.s @r4+, fr1
  fmov.s @r4, fr2
  fipr fv0, fv0
  .short 0xF37D
  fmul fr3, fr2
  fmov.s fr2, @r4
  fmul fr3, fr1
  fmov.s fr1, @-r4
  fmul fr3, fr0
  fmov.s fr0, @-r4
  rts
  nop

/***************************************************************************/

  .align 2
_dcmath_sinscale: .float 10430.37835
