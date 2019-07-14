(TeX-add-style-hook
 "commonly_used_spacemacs_commands"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "openany" "longbibliography" "slovene" "a4paper" "12pt")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("geometry" "a4paper" "inner=2.5cm" "outer=1.5cm" "top=2.5cm" "bottom=2.5cm") ("amsmath" "tbtags") ("fontenc" "T1") ("epstopdf" "outdir=./slike/") ("grffile" "multidot") ("inputenc" "utf8") ("mathtools" "tbtags") ("placeins" "section") ("url" "hyphens" "spaces" "obeyspaces") ("appendix" "titletoc" "title") ("natbib" "sort" "numbers") ("hyperref" "pdfa") ("pdfx" "x-1a")))
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art12"
    "geometry"
    "braket"
    "float"
    "afterpage"
    "graphicx"
    "amssymb"
    "amsmath"
    "fontenc"
    "epstopdf"
    "grffile"
    "inputenc"
    "makeidx"
    "enumerate"
    "caption"
    "subcaption"
    "mathtools"
    "wrapfig"
    "placeins"
    "url"
    "breakurl"
    "ragged2e"
    "fancyhdr"
    "appendix"
    "natbib"
    "hyperref"
    "pdfx"
    "pdfpages"
    "breqn"
    "xparse"
    "blindtext")
   (TeX-add-symbols
    "dif"
    "Alpha"
    "Beta"
    "Epsilon"
    "Kappa"
    "epsfg"
    "legendamp"
    "bi"
    "oldvec"
    "vec"
    "pol"
    "svec"))
 :latex)

