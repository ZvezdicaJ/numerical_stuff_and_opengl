cmake_minimum_required(VERSION 3.2)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
#set (CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native")
set(CMAKE_C_COMPILER "gcc")
add_library(cephes STATIC
acoshf.c
airyf.c
asinf.c
asinhf.c
atanf.c
atanhf.c
bdtrf.c
betaf.c
cbrtf.c
chbevlf.c
chdtrf.c
clogf.c
cmplxf.c
constf.c
coshf.c
dawsnf.c
drand.c
ellief.c
ellikf.c
ellpef.c
ellpjf.c
ellpkf.c
exp10f.c
exp2f.c
expf.c
expnf.c
expx2f.c
facf.c
fdtrf.c
floorf.c
fresnlf.c
gammaf.c
gdtrf.c
hyp2f1f.c
hypergf.c
i0f.c
i1f.c
igamf.c
igamif.c
incbet.c
incbetf.c
incbif.c
ivf.c
j0f.c
j0tst.c
j1f.c
jnf.c
jvf.c
k0f.c
k1f.c
knf.c
log10f.c
log2f.c
logf.c
mtherr.c
mtstf.c
nantst.c
nbdtrf.c
ndtrf.c
ndtrif.c
pdtrf.c
polevlf.c
polynf.c
powf.c
powif.c
powtst.c
psif.c
rgammaf.c
setprec.c
shichif.c
sicif.c
sindgf.c
sinf.c
sinhf.c
spencef.c
sqrtf.c
stdtrf.c
struvef.c
tandgf.c
tanf.c
tanhf.c
ynf.c
zetacf.c
zetaf.c
)
