#ifndef PndIOStreamUtils_HH
#define PndIOStreamUtils_HH

//--------------------------------------------------------------------------
//
// $Id: BbrIOStreamUtils.hh,v 1.1.1.1 2005/03/29 16:49:16 steinke Exp $
//
// Environment:
//      This software was developed for the BaBar collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 2002   California Institute of Technology
//
//  Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------
#include <iostream>


//		---------------------
// 		-- Class Interface --
//		---------------------

namespace panda {
  namespace IOStream {

    // Define data types for saving the formatting information.

    // Classic iostreams did not have a type defined for formatting flags.
#if defined(__linux__)
    // g++-2.95.3 defines a "fmtflags" typedef that is analogous to the
    // one in Standard iostreams.
    typedef std::ios::fmtflags fmtflags_type;
#else
    // "long" seems to have been the most common type used, and is the
    // type used in Sun Workshop.
    typedef long fmtflags_type;
#endif

    // Classic iostreams does not have a specific type defined for the
    // precision of a stream.  "int" is used on our platforms.
    typedef int precision_type;

    // Classic iostreams does not have a specific type defined for the
    // fill character of a stream.  Our platforms don't use a consistent
    // type.  We here assume that they can cast it to/from "char".
    typedef char fill_type;
    
    // Classic iostreams does not have a specific type defined for the
    // field width of a stream.  "int" is used on our platforms.
    typedef int width_type;

    /**
     *  This class allows the formatting flags of a "classic" i/std::ostream
     *  to be saved and restored, using the resource-acquisition-is-
     *  initialization pattern.  
     *
     *  This includes the std::ios::basefield (std::hex/std::dec/std::oct), adjustfield
     *  (left/right), and floatfield (scientific/fixed) flags, as well as
     *  the flags not in fields, such as skipws, showbase, uppercase, etc.
     *
     *  This software was developed for the BaBar collaboration.  If you
     *  use all or part of it, please give an appropriate acknowledgement.
     *
     *  Copyright (C) 2002   California Institute of Technology
     *
     *  @version $Id: BbrIOStreamUtils.hh,v 1.1.1.1 2005/03/29 16:49:16 steinke Exp $
     *
     *  @author Gregory Dubois-Felsmann  (originator)
     */

    class fmtflags_guard {
    public:
      /**
       *  Save a copy of the current formatting flags of a stream,
       *  along with a reference to the stream.
       */
      inline explicit fmtflags_guard( std::ios& stream );

      /**
       *  Save a copy of the current formatting flags of a stream,
       *  and give them a new value at the same time.
       */
      inline fmtflags_guard( std::ios& stream, fmtflags_type newflags );

      /**
       *  Restore the previously saved formatting flags to the referenced
       *  stream.
       */
      inline ~fmtflags_guard();

    private:
      std::ios&  stream_;
      const fmtflags_type saved_;
    };


    /**
     *  This class allows the numeric precision setting of a "classic"
     *  i/std::ostream to be saved and restored, using the resource-acquisition-
     *  is-initialization pattern.  
     *
     *  This software was developed for the BaBar collaboration.  If you
     *  use all or part of it, please give an appropriate acknowledgement.
     *
     *  Copyright (C) 2002   California Institute of Technology
     *
     *  @version $Id: BbrIOStreamUtils.hh,v 1.1.1.1 2005/03/29 16:49:16 steinke Exp $
     *
     *  @author Gregory Dubois-Felsmann  (originator)
     */

    class precision_guard {
    public:
      /**
       *  Save a copy of the current precision of a stream,
       *  along with a reference to the stream.
       */
      inline explicit precision_guard( std::ios& stream );

      /**
       *  Save a copy of the current precision of a stream,
       *  and give it a new value at the same time.
       */
      inline precision_guard( std::ios& stream, precision_type newprecision );

      /**
       *  Restore the previously saved precision to the referenced
       *  stream.
       */
      inline ~precision_guard();

    private:
      std::ios&  stream_;
      const precision_type saved_;
    };


    /**
     *  This class allows the fill character of a "classic" i/std::ostream
     *  to be saved and restored, using the resource-acquisition-is-
     *  initialization pattern.  
     *
     *  This software was developed for the BaBar collaboration.  If you
     *  use all or part of it, please give an appropriate acknowledgement.
     *
     *  Copyright (C) 2002   California Institute of Technology
     *
     *  @version $Id: BbrIOStreamUtils.hh,v 1.1.1.1 2005/03/29 16:49:16 steinke Exp $
     *
     *  @author Gregory Dubois-Felsmann  (originator)
     */

    class fill_guard {
    public:
      /**
       *  Save a copy of the current fill character of a stream,
       *  along with a reference to the stream.
       */
      inline explicit fill_guard( std::ios& stream );

      /**
       *  Save a copy of the current fill character of a stream,
       *  and give it a new value at the same time.
       */
      inline fill_guard( std::ios& stream, fill_type newfill );

      /**
       *  Restore the previously saved fill character to the referenced
       *  stream.
       */
      inline ~fill_guard();

    private:
      std::ios&  stream_;
      const fill_type saved_;
    };


    /**
     *  This class allows the field width setting of a "classic" i/std::ostream
     *  to be saved and restored, using the resource-acquisition-is-
     *  initialization pattern.
     *
     *  Note that this class is less useful than the ones for the format
     *  flags, precision, and fill character, because those settings are
     *  sticky and the width is not, but it still does have some applications,
     *  and its behavior is included in the all_format_guard class.
     *
     *  This software was developed for the BaBar collaboration.  If you
     *  use all or part of it, please give an appropriate acknowledgement.
     *
     *  Copyright (C) 2002   California Institute of Technology
     *
     *  @version $Id: BbrIOStreamUtils.hh,v 1.1.1.1 2005/03/29 16:49:16 steinke Exp $
     *
     *  @author Gregory Dubois-Felsmann (originator) */

    class width_guard {
    public:
      /**
       *  Save a copy of the current width character of a stream,
       *  along with a reference to the stream.
       */
      inline explicit width_guard( std::ios& stream );

      /**
       *  Save a copy of the current width character of a stream,
       *  and give it a new value at the same time.
       */
      inline width_guard( std::ios& stream, width_type newwidth );

      /**
       *  Restore the previously saved width character to the referenced
       *  stream.
       */
      inline ~width_guard();

    private:
      std::ios&  stream_;
      const width_type saved_;
    };


    /**
     *  This class allows all the formatting controls for a classic
     *  iostreams stream to be saved and later restored, using the
     *  resource-acquisition-is-initialization pattern.  The information
     *  is saved by the constructor of this class and restored by the
     *  destructor.
     *
     *  The formatting information saved includes the "flags" (see the
     *  fmtflags_guard class definition for the details), the numeric
     *  precision, the fill character, and the field width.
     *
     *  This software was developed for the BaBar collaboration.  If you
     *  use all or part of it, please give an appropriate acknowledgement.
     *
     *  Copyright (C) 2002   California Institute of Technology
     *
     *  @version $Id: BbrIOStreamUtils.hh,v 1.1.1.1 2005/03/29 16:49:16 steinke Exp $
     *
     *  @author Gregory Dubois-Felsmann  (originator)
     */

    class all_format_guard {
    public:
      /**
       *  Save a copy of the current formatting information for a stream,
       *  along with a reference to the stream.
       */
      inline explicit all_format_guard( std::ios& stream );

      /**
       *  Restore the previously saved formatting information to the referenced
       *  stream.
       */
      inline ~all_format_guard();

    private:
      std::ios&  stream_;
      const fmtflags_type  saved_fmtflags_;
      const precision_type saved_precision_;
      const fill_type      saved_fill_;
      const width_type     saved_width_;
    };


// Inline implementations

    inline fmtflags_guard::fmtflags_guard( std::ios& stream )
      : stream_(stream), saved_(stream.flags())
    { }
      
    inline fmtflags_guard::fmtflags_guard( std::ios& stream, 
					     fmtflags_type newflags )
      : stream_(stream), saved_(stream.flags(newflags))
    { }
      
    inline fmtflags_guard::~fmtflags_guard()
    {
      stream_.flags(saved_);
    }



    inline precision_guard::precision_guard( std::ios& stream )
      : stream_(stream), saved_(stream.precision())
    { }
      
    inline precision_guard::precision_guard( std::ios& stream, 
					     precision_type newprecision )
      : stream_(stream), saved_(stream.precision(newprecision))
    { }
      
    inline precision_guard::~precision_guard()
    {
      stream_.precision(saved_);
    }



    inline fill_guard::fill_guard( std::ios& stream )
      : stream_(stream), saved_(stream.fill())
    { }
      
    inline fill_guard::fill_guard( std::ios& stream, 
				   fill_type newfill )
      : stream_(stream), saved_(stream.fill(newfill))
    { }
      
    inline fill_guard::~fill_guard()
    {
      stream_.fill(saved_);
    }



    inline width_guard::width_guard( std::ios& stream )
      : stream_(stream), saved_(stream.width())
    { }
      
    inline width_guard::width_guard( std::ios& stream, 
				     width_type newwidth )
      : stream_(stream), saved_(stream.width(newwidth))
    { }
      
    inline width_guard::~width_guard()
    {
      stream_.width(saved_);
    }



    inline all_format_guard::all_format_guard( std::ios& stream )
      : stream_(stream)
      , saved_fmtflags_ ( stream.flags() )
      , saved_precision_( stream.precision() )
      , saved_fill_     ( stream.fill() )
      , saved_width_    ( stream.width() )
    { }
      
    inline all_format_guard::~all_format_guard()
    {
      stream_.flags    ( saved_fmtflags_ );
      stream_.precision( saved_precision_ );
      stream_.fill     ( saved_fill_ );
      stream_.width    ( saved_width_ );
    }


  }  // end namespace IOStream
}  // end namespace babar

#endif // BBRIOSTREAMUTILS_HH
