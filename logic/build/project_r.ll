Revision 3
; Created by bitgen M.81d at Mon Jan 30 10:05:43 2012
; Bit lines have the following form:
; <offset> <frame address> <frame offset> <information>
; <information> may be zero or more <kw>=<value> pairs
; Block=<blockname     specifies the block associated with this
;                      memory cell.
;
; Latch=<name>         specifies the latch associated with this memory cell.
;
; Net=<netname>        specifies the user net associated with this
;                      memory cell.
;
; COMPARE=[YES | NO]   specifies whether or not it is appropriate
;                      to compare this bit position between a
;                      "program" and a "readback" bitstream.
;                      If not present the default is NO.
;
; Ram=<ram id>:<bit>   This is used in cases where a CLB function
; Rom=<ram id>:<bit>   generator is used as RAM (or ROM).  <Ram id>
;                      will be either 'F', 'G', or 'M', indicating
;                      that it is part of a single F or G function
;                      generator used as RAM, or as a single RAM
;                      (or ROM) built from both F and G.  <Bit> is
;                      a decimal number.
;
; Info lines have the following form:
; Info <name>=<value>  specifies a bit associated with the LCA
;                      configuration options, and the value of
;                      that bit.  The names of these bits may have
;                      special meaning to software reading the .ll file.
;
Info STARTSEL0=1
Bit    18728 0x00040000    104 Block=P3 Latch=I Net=addr_pr<5>_IBUF
Bit    18767 0x00040000    143 Block=P2 Latch=I Net=addr_pr<4>_IBUF
Bit    18856 0x00040000    232 Block=P5 Latch=I Net=sram_data<6>_IBUF
Bit    18895 0x00040000    271 Block=P4 Latch=I Net=sram_data<7>_IBUF
Bit    19944 0x00040000   1320 Block=P10 Latch=I Net=sram_data<4>_IBUF
Bit    19983 0x00040000   1359 Block=P9 Latch=I Net=sram_data<5>_IBUF
Bit    20072 0x00040000   1448 Block=P12 Latch=I Net=sram_data<2>_IBUF
Bit    20111 0x00040000   1487 Block=P11 Latch=I Net=sram_data<3>_IBUF
Bit    20200 0x00040000   1576 Block=P16 Latch=I Net=sram_data<0>_IBUF
Bit    20239 0x00040000   1615 Block=P15 Latch=I Net=sram_data<1>_IBUF
Bit    77679 0x00060000     79 Block=P98 Latch=I Net=addr_pr<6>_IBUF
Bit   670504 0x001a0200     40 Block=P95 Latch=I Net=addr_pr<7>_IBUF
Bit   670543 0x001a0200     79 Block=P94 Latch=I Net=addr_pr<8>_IBUF
Bit   909416 0x00220200   3048 Block=P30 Latch=I Net=rst_IBUF
Bit   965383 0x00240200     39 Block=P92 Latch=I Net=addr_pr<9>_IBUF
Bit  1024360 0x00260200     40 Block=P91 Latch=I Net=addr_pr<11>_IBUF
Bit  1024399 0x00260200     79 Block=P90 Latch=I Net=addr_pr<12>_IBUF
Bit  1083336 0x00280200     40 Block=P89 Latch=I Net=nwe1_IBUF
Bit  1083375 0x00280200     79 Block=P88 Latch=I Net=nwe_IBUF
Bit  1145455 0x002a0400     79 Block=P85 Latch=I Net=addr_pr<10>_IBUF
Bit  1148463 0x002a0400   3087 Block=P38 Latch=I Net=clk_BUFGP/IBUFG
Bit  1263368 0x002e0400     40 Block=P84 Latch=I Net=addr_pr<0>_IBUF
Bit  1263407 0x002e0400     79 Block=P83 Latch=I Net=addr_pr<1>_IBUF
Bit  1319152 0x00300000   3056 Block=P44 Latch=O2 Net=Result<24>
Bit  1380797 0x00320200   2621 Block=SLICE_X43Y12 Latch=XQ Net=counter<24>
Bit  1380826 0x00320200   2650 Block=SLICE_X43Y11 Latch=YQ Net=counter<23>
Bit  1380829 0x00320200   2653 Block=SLICE_X43Y11 Latch=XQ Net=counter<22>
Bit  1380858 0x00320200   2682 Block=SLICE_X43Y10 Latch=YQ Net=counter<21>
Bit  1380861 0x00320200   2685 Block=SLICE_X43Y10 Latch=XQ Net=counter<20>
Bit  1380890 0x00320200   2714 Block=SLICE_X43Y9 Latch=YQ Net=counter<19>
Bit  1380893 0x00320200   2717 Block=SLICE_X43Y9 Latch=XQ Net=counter<18>
Bit  1380922 0x00320200   2746 Block=SLICE_X43Y8 Latch=YQ Net=counter<17>
Bit  1380925 0x00320200   2749 Block=SLICE_X43Y8 Latch=XQ Net=counter<16>
Bit  1380954 0x00320200   2778 Block=SLICE_X43Y7 Latch=YQ Net=counter<15>
Bit  1380957 0x00320200   2781 Block=SLICE_X43Y7 Latch=XQ Net=counter<14>
Bit  1380986 0x00320200   2810 Block=SLICE_X43Y6 Latch=YQ Net=counter<13>
Bit  1380989 0x00320200   2813 Block=SLICE_X43Y6 Latch=XQ Net=counter<12>
Bit  1381018 0x00320200   2842 Block=SLICE_X43Y5 Latch=YQ Net=counter<11>
Bit  1381021 0x00320200   2845 Block=SLICE_X43Y5 Latch=XQ Net=counter<10>
Bit  1381050 0x00320200   2874 Block=SLICE_X43Y4 Latch=YQ Net=counter<9>
Bit  1381053 0x00320200   2877 Block=SLICE_X43Y4 Latch=XQ Net=counter<8>
Bit  1381082 0x00320200   2906 Block=SLICE_X43Y3 Latch=YQ Net=counter<7>
Bit  1381085 0x00320200   2909 Block=SLICE_X43Y3 Latch=XQ Net=counter<6>
Bit  1381114 0x00320200   2938 Block=SLICE_X43Y2 Latch=YQ Net=counter<5>
Bit  1381117 0x00320200   2941 Block=SLICE_X43Y2 Latch=XQ Net=counter<4>
Bit  1381146 0x00320200   2970 Block=SLICE_X43Y1 Latch=YQ Net=counter<3>
Bit  1381149 0x00320200   2973 Block=SLICE_X43Y1 Latch=XQ Net=counter<2>
Bit  1381178 0x00320200   3002 Block=SLICE_X43Y0 Latch=YQ Net=counter<1>
Bit  1381181 0x00320200   3005 Block=SLICE_X43Y0 Latch=XQ Net=counter<0>
Bit  1678237 0x02000000   2077 Block=SLICE_X53Y29 Latch=XQ Net=ctrl0/hcounter<10>
Bit  1678266 0x02000000   2106 Block=SLICE_X53Y28 Latch=YQ Net=ctrl0/hcounter<9>
Bit  1678269 0x02000000   2109 Block=SLICE_X53Y28 Latch=XQ Net=ctrl0/hcounter<8>
Bit  1678298 0x02000000   2138 Block=SLICE_X53Y27 Latch=YQ Net=ctrl0/hcounter<7>
Bit  1678301 0x02000000   2141 Block=SLICE_X53Y27 Latch=XQ Net=ctrl0/hcounter<6>
Bit  1678330 0x02000000   2170 Block=SLICE_X53Y26 Latch=YQ Net=ctrl0/hcounter<5>
Bit  1678333 0x02000000   2173 Block=SLICE_X53Y26 Latch=XQ Net=ctrl0/hcounter<4>
Bit  1678362 0x02000000   2202 Block=SLICE_X53Y25 Latch=YQ Net=ctrl0/hcounter<3>
Bit  1678365 0x02000000   2205 Block=SLICE_X53Y25 Latch=XQ Net=ctrl0/hcounter<2>
Bit  1678394 0x02000000   2234 Block=SLICE_X53Y24 Latch=YQ Net=ctrl0/hcounter<1>
Bit  1678397 0x02000000   2237 Block=SLICE_X53Y24 Latch=XQ Net=ctrl0/hcounter<0>
Bit  1737146 0x02002600   2010 Block=SLICE_X55Y31 Latch=YQ Net=ctrl0/blank
Bit  1737277 0x02002600   2141 Block=SLICE_X55Y27 Latch=XQ Net=ctrl0/vcounter<10>
Bit  1737306 0x02002600   2170 Block=SLICE_X55Y26 Latch=YQ Net=ctrl0/vcounter<9>
Bit  1737309 0x02002600   2173 Block=SLICE_X55Y26 Latch=XQ Net=ctrl0/vcounter<8>
Bit  1737338 0x02002600   2202 Block=SLICE_X55Y25 Latch=YQ Net=ctrl0/vcounter<7>
Bit  1737341 0x02002600   2205 Block=SLICE_X55Y25 Latch=XQ Net=ctrl0/vcounter<6>
Bit  1737370 0x02002600   2234 Block=SLICE_X55Y24 Latch=YQ Net=ctrl0/vcounter<5>
Bit  1737373 0x02002600   2237 Block=SLICE_X55Y24 Latch=XQ Net=ctrl0/vcounter<4>
Bit  1737402 0x02002600   2266 Block=SLICE_X55Y23 Latch=YQ Net=ctrl0/vcounter<3>
Bit  1737405 0x02002600   2269 Block=SLICE_X55Y23 Latch=XQ Net=ctrl0/vcounter<2>
Bit  1737434 0x02002600   2298 Block=SLICE_X55Y22 Latch=YQ Net=ctrl0/vcounter<1>
Bit  1737437 0x02002600   2301 Block=SLICE_X55Y22 Latch=XQ Net=ctrl0/vcounter<0>
Bit  2092136 0x02027400     40 Block=P79 Latch=I Net=addr_pr<2>_IBUF
Bit  2092175 0x02027400     79 Block=P78 Latch=I Net=addr_pr<3>_IBUF
Bit  2149338 0x04000000   1370 Block=SLICE_X67Y51 Latch=YQ Net=clk2<1>
Bit  2149370 0x04000000   1402 Block=SLICE_X67Y50 Latch=YQ Net=clk2<0>
Bit  2205104 0x04002400   1264 Block=P71 Latch=O2 Net=GLOBAL_LOGIC0
Bit  2205127 0x04002400   1287 Block=P70 Latch=O2 Net=GLOBAL_LOGIC1
Bit  2205232 0x04002400   1392 Block=P68 Latch=O2 Net=GLOBAL_LOGIC1
Bit  2205255 0x04002400   1415 Block=P67 Latch=O2 Net=GLOBAL_LOGIC0
Bit  2205360 0x04002400   1520 Block=P66 Latch=O2 Net=GLOBAL_LOGIC0
Bit  2205383 0x04002400   1543 Block=P65 Latch=O2 Net=GLOBAL_LOGIC0
Bit  2205488 0x04002400   1648 Block=P63 Latch=O2 Net=GLOBAL_LOGIC0
Bit  2205511 0x04002400   1671 Block=P62 Latch=O2 Net=GLOBAL_LOGIC0
Bit  2205616 0x04002400   1776 Block=P61 Latch=O2 Net=GLOBAL_LOGIC0
Bit  2205639 0x04002400   1799 Block=P60 Latch=O2 Net=GLOBAL_LOGIC0
Bit  2211367 0x04020200   1319 Block=P69 Latch=I Net=ncs_IBUF
