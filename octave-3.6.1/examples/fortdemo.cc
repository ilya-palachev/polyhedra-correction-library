#include <octave/oct.h>
#include <octave/f77-fcn.h>

extern "C"
{
  F77_RET_T
  F77_FUNC (fortsub, FORTSUB)
        (const int&, double*, F77_CHAR_ARG_DECL
         F77_CHAR_ARG_LEN_DECL);
}

DEFUN_DLD (fortdemo , args , , "Fortran Demo.")
{
  octave_value_list retval;
  int nargin = args.length();
  if (nargin != 1)
    print_usage ();
  else
    {
      NDArray a = args(0).array_value ();
      if (! error_state)
        {
          double *av = a.fortran_vec ();
          octave_idx_type na = a.nelem ();
          OCTAVE_LOCAL_BUFFER (char, ctmp, 128);

          F77_XFCN (fortsub, FORTSUB, (na, av, ctmp
                    F77_CHAR_ARG_LEN (128)));

          retval(1) = std::string (ctmp);
          retval(0) = a;
        }
    }
  return retval;
}
