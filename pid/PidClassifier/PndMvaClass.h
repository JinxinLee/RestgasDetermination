/*************************************
 * The structure of the classes.     *
 * Version:                          *
 * License:                          *
 *************************************/
//#pragma once
#ifndef PND_MVA_CLASS_H
#define PND_MVA_CLASS_H

/**
 * Struct to describe a class of particles. It stores the name of the class,
 * the number of examples available and their indices.
 */
struct PndMvaClass
{
  //! Constructor.
  /**
   *@Param name Class name.
   */
  PndMvaClass(const std::string& name);
  
  std::string Name;  /**< Name of the class. */
  size_t NExamples;  /**< Number of examples available of the class. */
  size_t StartIdx;   /**< Start index of events of this class. */
  size_t EndIdx;     /**< End index of events of this class. */
};

/**
 * Constructor implementation.
 *@Param name Class name.
*/
inline PndMvaClass::PndMvaClass(const std::string& name)
:
    Name(name),
    NExamples(0),
    StartIdx(0),
    EndIdx(0)
{}

#endif
