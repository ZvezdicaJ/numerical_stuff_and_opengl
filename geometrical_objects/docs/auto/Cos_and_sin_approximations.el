(TeX-add-style-hook
 "Cos_and_sin_approximations"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "openany" "longbibliography" "slovene" "a4paper" "12pt")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("geometry" "a4paper" "inner=3.5cm" "outer=2.5cm" "top=2.5cm" "bottom=2.5cm") ("amsmath" "tbtags") ("fontenc" "T1") ("epstopdf" "outdir=./slike/") ("grffile" "multidot") ("inputenc" "utf8") ("mathtools" "tbtags") ("placeins" "section") ("url" "hyphens" "spaces" "obeyspaces") ("appendix" "titletoc" "title") ("natbib" "sort" "numbers") ("hyperref" "pdfa") ("pdfx" "x-1a")))
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "path")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "url")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "nolinkurl")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperbaseurl")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperimage")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperref")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "path")
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
    "xparse")
   (TeX-add-symbols
    "dif"
    "myint"
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
    "svec")
   (LaTeX-add-labels
    "coeffm"
    "coeff0"))
 :latex)

