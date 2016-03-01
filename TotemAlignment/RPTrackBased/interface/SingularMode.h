/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*    
* $$RCSfile: StraightTrackAlignment.h,v $: $
* $Revision: 9977 $
* $Date: 2015-01-12 16:00:26 +0200 (pon, 12 sty 2015) $
*
****************************************************************************/

#ifndef _SingularMode_h_
#define _SingularMode_h_

#include <TVectorD.h>

/**
 *\brief 
 **/
struct SingularMode
{
  /// eigen value
  double val;

  /// eigen vector
  TVectorD vec;

  /// index
  unsigned int idx;
};

#endif
